// -*- C++ -*-

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
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
// File: imgops.h
// Purpose: interface to corresponding .cc file
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de
#ifndef h_imgops___
#define h_imgops___

#include "colib/colib.h"

namespace iulib {

    template<class T, class S>
    void getd0(colib::narray<T> &image, colib::narray<S> &slice, int index);
    template<class T, class S>
    void getd1(colib::narray<T> &image, colib::narray<S> &slice, int index);
    template<class T, class S>
    void putd0(colib::narray<T> &image, colib::narray<S> &slice, int index);
    template<class T, class S>
    void putd1(colib::narray<T> &image, colib::narray<S> &slice, int index);
    float gradx(colib::floatarray &image, int x, int y);
    float grady(colib::floatarray &image, int x, int y);
    float gradmag(colib::floatarray &image, int x, int y);
    float gradang(colib::floatarray &image, int x, int y);

    template<class T>
    inline T &xref(colib::narray<T> &a, int x, int y) {
        if(x<0) x = 0;
        else if(x>=a.dim(0)) x = a.dim(0)-1;
        if(y<0) y = 0;
        else if(y>=a.dim(1)) y = a.dim(1)-1;
        return a.unsafe_at(x, y);
    }

    template<class T>
    inline T bilin(colib::narray<T> &a, float x, float y) {
        int i = (int)x;
        int j = (int)y;
        float l = x-i;
        float m = y-j;
        float s00 = xref(a,i,j);
        float s01 = xref(a,i,j+1);
        float s10 = xref(a,i+1,j);
        float s11 = xref(a,i+1,j+1);
        return (T)((1.0-l) * ((1.0-m) * s00 + m * s01) +
                         l * ((1.0-m) * s10 + m * s11));
    }

    template<class T, class V>
    void addscaled(colib::narray<T> &, colib::narray<T> &, V, int, int);
    template<class T>
    void tighten(colib::narray<T> &image);
    template<class T>
    void circ_by(colib::narray<T> &image, int dx, int dy, T value=0);
    template<class T>
    void shift_by(colib::narray<T> &image, int dx, int dy, T value=0);
    template<class T>
    void pad_by(colib::narray<T> &image, int px, int py, T value=0);
    template<class T>
    void erase_boundary(colib::narray<T> &, int, int, T);
    template<class T, class S>
    void extract_subimage(colib::narray<T> &subimage,
                          colib::narray<S> &image, int x0, int y0, int x1, int y1);
    template<class T, class S,class U>
    void extract_bat(colib::narray<T> &subimage,
                          colib::narray<S> &image, int x0, int y0, int x1, int y1,U dflt);
    template<class T>
    void resize_to(colib::narray<T> &image, int w, int h, T value=0);
    void compose_at(colib::bytearray &image, colib::bytearray &source, int x, int y,
                    int value, int conflict);

    template<class T, class U, class V, class W>
    void ifelse(colib::narray<T> &dest,
                colib::narray<U> &cond, colib::narray<V> &iftrue, colib::narray<W> &iffalse);
    void blend(colib::floatarray &dest, colib::floatarray &cond, colib::floatarray &iftrue,
               colib::floatarray &iffalse);

    template<class T>
    void linearly_transform_intensity(colib::narray<T> &image,
                                      float m, float b, float lo, float hi);
    template<class T>
    void gamma_transform(colib::narray<T> &image, float gamma,
                         float c, float lo, float hi);
    template<class T>
    void expand_range(colib::narray<T> &image, float lo, float hi);
}

#endif
