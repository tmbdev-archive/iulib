// Copyright 2006-2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
// or its licensors, as applicable.
// Copyright 1995-2005 Thomas M. Breuel
// 
// You may not use this file except under the terms of the accompanying license.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// 
// Project: iulib -- image understanding library
// File: imgrescale.cc
// Purpose: bicubic (and other) scaling
// Responsible: mezhirov
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#include <stdlib.h>
#include <stdio.h>

#include "imgrescale.h"


using namespace colib;

namespace iulib {

    enum {N = 4};

    // cubic1D_coefs[i] are coefficients of a polynomial
    // that is equal to 0 in 0,1,2,3 except for i, where it's equal to 1.
    static float cubic1D_coefs[N][N] = { { 1, -11./6, 1, -1./6 }, { 0, 3, -2.5,
            .5 }, { 0, -1.5, 2, -.5 }, { 0, 1./3, -.5, 1./6 } };

    void cubic1d_find(float coefs[N], float values[N]) {
        for (int j = 0; j < N; j++)
            coefs[j] = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++)
                coefs[j] += values[i] * cubic1D_coefs[i][j];
        }
    }

    void cubic1d_find_narray(floatarray &coefs, floatarray &values) {
        for (int j = 0; j < N; j++)
            coefs[j] = 0;
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < N; j++)
                coefs[j] += values[i] * cubic1D_coefs[i][j];
        }
    }

    float cubic1d_calculate(float coefs[N], float x) {
        float s = coefs[N-1];
        for (int i = N - 1; i; i--)
            s = s * x + coefs[i - 1];
        return s;
    }

    static void transpose(float dst[N][N], float src[N][N]) {
        for (int i = 0; i < N; i++)
            for (int j = 0; j < N; j++) {
                dst[i][j] = src[j][i];
            }
    }

    void cubic2d_find(float coefs[N][N], float values[N][N]) {
        float tmp[N][N];
        for (int i=0; i<N; i++)
            cubic1d_find(coefs[i], values[i]);
        transpose(tmp, coefs);
        for (int i=0; i<N; i++)
            cubic1d_find(coefs[i], tmp[i]);
    }

    float cubic2d_calculate(float coefs[N][N], float x, float y) {
        float c[N];
        for (int i = 0; i < N; i++)
            c[i] = cubic1d_calculate(coefs[i], y);
        return cubic1d_calculate(c, x);
    }

    float cubic2d_interpolate(float values[N][N], float x, float y) {
        float coefs[N][N];
        cubic2d_find(coefs, values);
        return cubic2d_calculate(coefs, x, y);
    }

    inline int int_into_range(int x, int a, int b) {
        ASSERT(a <= b);
        if (x < a)
            return a;
        if (x > b)
            return b;
        return x;
    }

    float bicubic_interpolate(const floatarray &src, float x, float y) {
        ASSERT(src.dim(0) >= N);
        ASSERT(src.dim(1) >= N);
        int x0 = int(x) - 1;
        int y0 = int(y) - 1;
        // x0 = int_into_range(x_base, 0, src.dim(0) - N);
        // y0 = int_into_range(y_base, 0, src.dim(1) - N);
        float values[N][N];
        if (x0 > 0 && y0 > 0 && x0 <= src.dim(0) - N && y0 <= src.dim(1) - N) {
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++)
                    values[j][i] = src(i + x0, j + y0);
        } else {
            for (int i = 0; i < N; i++)
                for (int j = 0; j < N; j++) {
                    int xx = i + x0;
                    int yy = j + y0;
                    values[j][i] = src(int_into_range(xx, 0, src.dim(0) - 1),
                            int_into_range(yy, 0, src.dim(1) - 1));
                }

        }
        return cubic2d_interpolate(values, x - x0, y - y0);
    }

    template<class T>
    void stretch(floatarray &out /* should be set to the new size */,
                 const narray<T> &in) {
        ASSERT(out.rank() == 1);
        ASSERT(in.rank() == 1);
        fill(out, 0);
        int i = 0, j = 0;
        int N = in.dim(0), M = out.dim(0);
        double pos_i = 1./N, pos_j = 1./M;
        double pos = 0;
        while (i < N && j < M) {
            if (pos_i < pos_j) {
                out(j) += M * (pos_i - pos) * in(i);
                pos = pos_i;
                i++;
                pos_i = double(i + 1) / N;
            } else {
                out(j) += M * (pos_j - pos) * in(i);
                pos = pos_j;
                j++;
                pos_j = double(j + 1) / M;
            }
        }
    }

    template<class T>
    void load_row(narray<T> &row, const narray<T> &a,
            int index0) {
        for (int i = 0; i < a.dim(1); i++)
            row(i) = a(index0, i);
    }

    template<class T>
    void store_row(narray<T> &a, const narray<T> &row,
            int index0) {
        for (int i = 0; i < a.dim(1); i++)
            a(index0, i) = row(i);
    }

    template<class T>
    void stretch_rows(floatarray &dest, const narray<T> &src) {
        floatarray row_dest;
        narray<T> row_src;
        ASSERT(dest.dim(0) == src.dim(0));
        row_dest.resize(dest.dim(1));
        row_src.resize(src.dim(1));
        for (int i = 0; i < src.dim(0); i++) {
            load_row(row_src, src, i);
            stretch(row_dest, row_src);
            store_row(dest, row_dest, i);
        }
    }

    template<class T>
    void transpose(narray<T> &a) {
        narray<T> t;
        t.resize(a.dim(1), a.dim(0));
        for (int x = 0; x < a.dim(0); x++) {
            for (int y = 0; y < a.dim(1); y++)
                t(y, x) = a(x,y);
        }
        move(a, t);
    }

    void rough_rescale(floatarray &out, const floatarray &in, int new_w,
            int new_h) {
        ASSERT(in.rank() == 2);
        floatarray temp;
        temp.resize(in.dim(0), new_h);
        stretch_rows(temp, in);
        transpose(temp);
        out.resize(new_h, new_w);
        stretch_rows(out, temp);
        transpose(out);
    }

    void bicubic_rescale(floatarray &dst, const floatarray &src, int w, int h) {
        if (src.dim(0) < N || src.dim(1) < N) {
            rough_rescale(dst, src, w, h);
            return;
        }

        dst.resize(w, h);
        for (int x = 0; x < w; x++)
            for (int y = 0; y < h; y++) {
                dst(x, y) = bicubic_interpolate(src, float(x) * src.dim(0) / w,
                        float(y) * src.dim(1) / h);
            }
    }

    template<class T>
    void trim_range(bytearray &a, narray<T> &c) {
        makelike(a, c);
        for (int i = 0; i < c.length1d(); i++) {
            if (c.at1d(i) < 0)
                a.at1d(i) = 0;
            else if (c.at1d(i) > 255)
                a.at1d(i) = 255;
            else
                a.at1d(i) = byte(c.at1d(i));
        }
    }

    void rescale(floatarray &dst, const floatarray &src, int w, int h) {
        if (w > src.dim(0) && h > src.dim(1))
            bicubic_rescale(dst, src, w, h);
        else
            rough_rescale(dst, src, w, h);
    }

    void rescale(bytearray &dst, const bytearray &src, int w, int h) {
        floatarray a, b;
        copy(a, src);
        rescale(b, a, w, h);
        trim_range(dst, b);
    }

    void rescale_to_width(floatarray &dst, const floatarray &src, int w) {
        rescale(dst, src, w, max(1, src.dim(1) * w / src.dim(0)));
    }

    void rescale_to_width(bytearray &dst, const bytearray &src, int w) {
        rescale(dst, src, w, max(1, src.dim(1) * w / src.dim(0)));
    }

    void rescale_to_height(floatarray &dst, const floatarray &src, int h) {
        rescale(dst, src, max(1, src.dim(0) * h / src.dim(1)), h);
    }

    void rescale_to_height(bytearray &dst, const bytearray &src, int h) {
        rescale(dst, src, max(1, src.dim(0) * h / src.dim(1)), h);
    }

} // namespace

