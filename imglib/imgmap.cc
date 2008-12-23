// -*- C++ -*-

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
// File: imgmap.cc
// Purpose: geometric transformations on images
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

extern "C" {
#include <math.h>
}

#include "colib/colib.h"
#include "imglib.h"


using namespace colib;

namespace iulib {

    template<class T>
    inline void swap(T &a, T &b) {
        T temp = a;
        a = b;
        b = temp;
    }

    template<class T>
    void rotate_direct_sample(narray<T> &out, narray<T> &in,
            float angle, float cx, float cy) {
        out.resize(in.dim(0), in.dim(1));
        fill(out, 0);
        float c = cos(angle);
        float s = sin(angle);
        int w = out.dim(0), h = out.dim(1);
        if (cx>1e30)
            cx = w/2.0;
        if (cy>1e30)
            cy = h/2.0;
        for (int i=0; i<w; i++) {
            for (int j=0; j<h; j++) {
                float x = c * (i-cx) - s * (j-cy) + cx;
                float y = s * (i-cx) + c * (j-cy) + cy;
                out(i, j) = xref(in,(int)round(x+0.5),(int)round(y+0.5));
            }
        }
    }
template     void rotate_direct_sample(bytearray &, bytearray &, float, float, float);
template     void rotate_direct_sample(intarray &, intarray &, float, float, float);
template     void rotate_direct_sample(floatarray &, floatarray &, float, float, float);

    // FIXME: Does not do the right thing when given a color image as an
    // intarray. It just takes the values as integers and interpolates them
    template<class T>
    void rotate_direct_interpolate(narray<T> &out,
            narray<T> &in, float angle, float cx, float cy) {
        out.resize(in.dim(0), in.dim(1));
        fill(out, 0);
        float c = cos(angle);
        float s = sin(angle);
        int w = out.dim(0), h = out.dim(1);
        if (cx>1e30)
            cx = w/2.0;
        if (cy>1e30)
            cy = h/2.0;
        for (int i=0; i<w; i++) {
            for (int j=0; j<h; j++) {
                float x = c * (i-cx) - s * (j-cy) + cx;
                float y = s * (i-cx) + c * (j-cy) + cy;
                out(i, j) = bilin(in,x,y);
            }
        }
    }
template     void rotate_direct_interpolate(bytearray &, bytearray &, float, float,
            float);
template     void rotate_direct_interpolate(intarray &, intarray &, float, float, float);
template     void rotate_direct_interpolate(floatarray &, floatarray &, float, float,
            float);

    template<class T>
    void scale_sample(narray<T> &out, narray<T> &in,
            float sx, float sy) {
        int nx = max(1, int(sx*in.dim(0)));
        int ny = max(1, int(sy*in.dim(1)));
        scale_sample(out, in, nx, ny);
    }
template     void scale_sample(bytearray &, bytearray &, float, float);
template     void scale_sample(intarray &, intarray &, float, float);
template     void scale_sample(floatarray &, floatarray &, float, float);

    template<class T>
    void scale_sample(narray<T> &out, narray<T> &in, int nx,
            int ny) {
        out.resize(max(1, nx), max(1, ny));
        float sx = fmax(1, nx)/in.dim(0);
        float sy = fmax(1, ny)/in.dim(1);
        fill(out, 0);
        for (int i=0; i<out.dim(0); i++) {
            for (int j=0; j<out.dim(1); j++) {
                out(i, j) = xref(in,int(0.5+i/sx),int(0.5+j/sy));
            }
        }
    }
template     void scale_sample(bytearray &, bytearray &, int, int);
template     void scale_sample(intarray &, intarray &, int, int);
template     void scale_sample(floatarray &, floatarray &, int, int);

    template<class T>
    void scale_interpolate(narray<T> &out, narray<T> &in,
            float sx, float sy) {
        int nx = max(1, int(sx*in.dim(0)));
        int ny = max(1, int(sy*in.dim(1)));
        scale_interpolate(out, in, nx, ny);
    }
template     void scale_interpolate(bytearray &, bytearray &, float, float);
template     void scale_interpolate(intarray &, intarray &, float, float);
template     void scale_interpolate(floatarray &, floatarray &, float, float);

    template<class T>
    void scale_interpolate(narray<T> &out, narray<T> &in,
            int nx, int ny) {
        out.resize(max(1, nx), max(1, ny));
        float sx = fmax(1, nx)/in.dim(0);
        float sy = fmax(1, ny)/in.dim(1);
        fill(out, 0);
        for (int i=0; i<out.dim(0); i++) {
            for (int j=0; j<out.dim(1); j++) {
                out(i, j) = bilin(in,int(0.5+i/sx),int(0.5+j/sy));
            }
        }
    }
template     void scale_interpolate(bytearray &, bytearray &, int, int);
template     void scale_interpolate(intarray &, intarray &, int, int);
template     void scale_interpolate(floatarray &, floatarray &, int, int);
}
