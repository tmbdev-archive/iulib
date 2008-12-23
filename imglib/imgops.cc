// -*- C++ -*-

// Copyright 2006-2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors,  as applicable.
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
// File: imgops.cc
// Purpose: basic image ops: extracting rows, cols, subimages; padding; ...
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

extern "C" {
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
}

#include "colib/colib.h"
#include "imglib.h"
#include "colib/checks.h"
#include "colib/narray-util.h"

using namespace colib;

namespace iulib {

    template<class T>
    inline void swap(T &a,T &b) {
        T temp = a;
        a = b;
        b = temp;
    }

    template<class T>
    void flip_d0(narray<T> &image) {
        int w = image.dim(0);
        narray<T> slice1,slice2;
        for (int i=0; i<w/2; i++) {
            getd0(slice1,image,i);
            getd0(slice2,image,w-i-1);
            putd0(image,slice2,i);
            putd0(image,slice1,w-i-1);
        }
    }

    template<class T>
    void flip_d1(narray<T> &image) {
        int h = image.dim(1);
        narray<T> slice1,slice2;
        for (int j=0; j<h/2; j++) {
            getd1(slice1,image,j);
            getd1(slice2,image,h-j-1);
            putd1(image,slice2,j);
            putd1(image,slice1,h-j-1);
        }
    }

    template<class T,class S>
    void getd0(narray<T> &image,narray<S> &slice,
            int index) {
        slice.resize(image.dim(1));
        for (int i=0; i<image.dim(1); i++)
            slice.unsafe_at(i) = (S)image.unsafe_at(index,i);
    }
    template void getd0(bytearray &,bytearray &,int);
    template void getd0(floatarray &,floatarray &,int);
    template void getd0(bytearray &,floatarray &,int);
    template void getd0(floatarray &,bytearray &,int);

    template<class T,class S>
    void getd1(narray<T> &image,narray<S> &slice,int index) {
        slice.resize(image.dim(0));
        for (int i=0; i<image.dim(0); i++)
            slice.unsafe_at(i) = (S)image.unsafe_at(i,index);
    }
    template void getd1(bytearray &,bytearray &,int);
    template void getd1(floatarray &,floatarray &,int);
    template void getd1(bytearray &,floatarray &,int);
    template void getd1(floatarray &,bytearray &,int);

    template<class T,class S>
    void putd0(narray<T> &image,narray<S> &slice,int index) {
        ASSERT(slice.rank()==1 && slice.dim(0)==image.dim(1));
        for (int i=0; i<image.dim(1); i++)
            image.unsafe_at(index,i) = (T)slice.unsafe_at(i);
    }
    template void putd0(bytearray &,bytearray &,int);
    template void putd0(floatarray &,floatarray &,int);
    template void putd0(bytearray &,floatarray &,int);
    template void putd0(floatarray &,bytearray &,int);

    template<class T,class S>
    void putd1(narray<T> &image,narray<S> &slice,
            int index) {
        ASSERT(slice.rank()==1 && slice.dim(0)==image.dim(0));
        for (int i=0; i<image.dim(0); i++)
            image.unsafe_at(i,index) = (T)slice.unsafe_at(i);
    }
    template void putd1(bytearray &,bytearray &,int);
    template void putd1(floatarray &,floatarray &,int);
    template void putd1(bytearray &,floatarray &,int);
    template void putd1(floatarray &,bytearray &,int);

    float gradx(floatarray &image,int x,int y) {
        return image(x+1,y)-image(x,y);
    }
    float grady(floatarray &image,int x,int y) {
        return image(x,y+1)-image(x,y);
    }
    float gradmag(floatarray &image,int x,int y) {
        return hypot(gradx(image,x,y),grady(image,x,y));
    }
    float gradang(floatarray &image,int x,int y) {
        return atan2(grady(image,x,y),gradx(image,x,y));
    }

    template<class T,class V>
    void addscaled(narray<T> &dest,narray<T> &src,
            V scale=1,int dx=0,int dy=0) {
        for (int i=0; i<dest.dim(0); i++)
            for (int j=0; j<dest.dim(1); j++)
                dest.unsafe_at(i,j) += (T)(scale*xref(src,i+dx,j+dy));
    }
    template void addscaled(bytearray &,bytearray &,float,int,int);
    template void addscaled(intarray &,intarray &,float,int,int);
    template void addscaled(floatarray &,floatarray &,float,int,int);

    template<class T>
    void tighten(narray<T> &image) {
        int x0=999999,x1=0,y0=99999,y1=0;
        for (int x=0; x<image.dim(0); x++) {
            for (int y=0; y<image.dim(1); y++) {
                if (!image(x,y))
                    continue;
                x0 = min(x0,x);
                y0 = min(y0,y);
                x1 = max(x1,x);
                y1 = max(y1,y);
            }
        }
        if (x0>x1||y0>y1)
            return;
        narray<T> temp(x1-x0,y1-y0);
        for (int x=0; x<temp.dim(0); x++) {
            for (int y=0; y<temp.dim(1); y++) {
                temp(x,y) = image(x+x0,y+y0);
            }
        }
        move(image,temp);
    }
    template void tighten(bytearray &);
    template void tighten(intarray &);
    template void tighten(floatarray &);

    template<class T>
    void shift_by(narray<T> &image,int dx,int dy,T value) {
        if (dx==0&&dy==0)
            return;
        int w = image.dim(0),h = image.dim(1);
        narray<T> temp(w,h);
        fill(temp,value);
        for (int i=0; i<image.dim(0); i++) {
            if (unsigned(i+dx)>=unsigned(w))
                continue;
            for (int j=0; j<image.dim(1); j++) {
                if (unsigned(j+dy)>=unsigned(h))
                    continue;
                temp(i+dx,j+dy) = image(i,j);
            }
        }
        move(image,temp);
    }
    template void shift_by(bytearray &,int,int,byte);
    template void shift_by(intarray &,int,int,int);
    template void shift_by(floatarray &,int,int,float);

    template<class T>
    void circ_by(narray<T> &image,int dx,int dy,T value) {
        if (dx==0&&dy==0)
            return;
        int w = image.dim(0),h = image.dim(1);
        narray<T> temp(w,h);
        fill(temp,value);
        for (int i=0; i<image.dim(0); i++) {
            for (int j=0; j<image.dim(1); j++) {
                temp((i+dx)%w,(j+dy)%h) = image(i,j);
            }
        }
        move(image,temp);
    }
    template void circ_by(bytearray &,int,int,byte);
    template void circ_by(intarray &,int,int,int);
    template void circ_by(floatarray &,int,int,float);

    template<class T>
    void pad_by(narray<T> &image,int px,int py,T value) {
        if (px==0&&py==0)
            return;
        int w = image.dim(0),h = image.dim(1);
        narray<T> temp(w+2*px,h+2*py);
        fill(temp,value);
        for (int i=0; i<image.dim(0); i++) {
            for (int j=0; j<image.dim(1); j++) {
                if (unsigned(i+px)>=unsigned(temp.dim(0))||unsigned(j+py)>=unsigned(temp.dim(1)))
                    continue;
                temp(i+px,j+py) = image(i,j);
            }
        }
        move(image,temp);
    }
    template void pad_by(bytearray &,int,int,byte);
    template void pad_by(intarray &,int,int,int);
    template void pad_by(floatarray &,int,int,float);

    template<class T>
    void erase_boundary(narray<T> &image,int px,int py,T value) {
        int w = image.dim(0),h = image.dim(1);
        for (int i=0; i<px; i++) {
            for (int j=0; j<h; j++) {
                image(i,j) = value;
                image(w-i-1,j) = value;
            }
        }
        for (int j=0; j<py; j++) {
            for (int i=px; i<w-px; i++) {
                image(i,j) = value;
                image(i,h-j-1) = value;
            }
        }
    }

    template void erase_boundary(bytearray &,int,int,byte);
    template void erase_boundary(intarray &,int,int,int);
    template void erase_boundary(floatarray &,int,int,float);

    template<class T>
    void resize_to(narray<T> &image,int w,int h,T value) {
        if (image.dim(0)==w&&image.dim(1)==h)
            return;
        narray<T> temp(w,h);
        fill(temp,value);
        for (int i=0; i<image.dim(0); i++) {
            if (i>=w)
                continue;
            for (int j=0; j<image.dim(1); j++) {
                if (j>=h)
                    continue;
                temp(i,j) = image(i,j);
            }
        }
        move(image,temp);
    }
    template void resize_to(bytearray &,int,int,byte);
    template void resize_to(intarray &,int,int,int);
    template void resize_to(floatarray &,int,int,float);

    template<class T>
    void extract(narray<T> &dest,narray<T> &src,int x0,int y0,int x1,int y1) {
        CHECK_ARG(x0>=0 && x1<src.dim(0));
        CHECK_ARG(y0>=0 && y1<src.dim(1));
        dest.resize(x1-x0,y1-y0);
        for (int i=0; i<dest.dim(0); i++)
            for (int j=0; j<dest.dim(1); j++)
                dest(i,j) = src(i+x0,j+y0);
    }
    template void extract(bytearray &,bytearray &,int,int,int,int);
    template void extract(intarray &,intarray &,int,int,int,int);
    template void extract(floatarray &,floatarray &,int,int,int,int);

    void compose_at(bytearray &image,bytearray &source,int x,int y,
            int value,int conflict) {
        for (int i=0; i<source.dim(0); i++) {
            if (unsigned(i+x)>=unsigned(image.dim(0)))
                continue;
            for (int j=0; j<source.dim(1); j++) {
                if (unsigned(j+y)>=unsigned(image.dim(1)))
                    continue;
                if (source(i,j)) {
                    if (image(i+x,j+y))
                        image(i+x,j+y) = conflict;
                    else
                        image(i+x,j+y) = value;
                }
            }
        }
    }

    template<class T,class U,class V,class W>
    void ifelse(narray<T> &dest,narray<U> &cond,narray<V> &iftrue,narray<W> &iffalse) {
        makelike(dest,cond);
        ASSERT(samedims(cond,iftrue));
        ASSERT(samedims(cond,iffalse));
        int n = cond.length1d();
        for (int i=0; i<n; i++)
            dest.at1d(i) = cond.at1d(i) ? iftrue.at1d(i) : iffalse.at1d(i);
    }
    template void ifelse(bytearray &,bytearray &,bytearray &,bytearray &);
    template void ifelse(floatarray &,bytearray &,floatarray &,floatarray &);
    template void ifelse(floatarray &,floatarray &,floatarray &,floatarray &);

    void blend(floatarray &dest,floatarray &cond,floatarray &iftrue,floatarray &iffalse) {
        makelike(dest,cond);
        CHECK_ARG(samedims(cond,iftrue));
        CHECK_ARG(samedims(cond,iffalse));
        int n = cond.length1d();
        for (int i=0; i<n; i++)
            dest.at1d(i) = cond.at1d(i) * iftrue.at1d(i) + (1-cond.at1d(i))
                * iffalse.at1d(i);
    }

    template<class T,class S>
    void extract_subimage(narray<T> &out,narray<S> &image,int x0,int y0,int x1,int y1) {
        x0 = max(x0,0);
        y0 = max(y0,0);
        x1 = min(x1,image.dim(0));
        y1 = min(y1,image.dim(1));
        int w = x1-x0;
        int h = y1-y0;
        out.resize(w,h);
        for (int i=0; i<w; i++)
            for (int j=0; j<h; j++)
                out(i,j) = (T)image(x0+i,y0+j);
    }
    template void extract_subimage(bytearray &out,bytearray &image,int x0,int y0,int x1,int y1);
    template void extract_subimage(floatarray &out,floatarray &image,int x0,int y0,int x1,int y1);
    template void extract_subimage(intarray &out,intarray &image,int x0,int y0,int x1,int y1);
    template void extract_subimage(floatarray &out,bytearray &image,int x0,int y0,int x1,int y1);
    template void extract_subimage(bytearray &out,floatarray &image,int x0,int y0,int x1,int y1);
    template void extract_subimage(intarray &out,bytearray &image,int x0,int y0,int x1,int y1);
    template void extract_subimage(bytearray &out,intarray &image,int x0,int y0,int x1,int y1);
    template void extract_subimage(intarray &out,floatarray &image,int x0,int y0,int x1,int y1);
    template void extract_subimage(floatarray &out,intarray &image,int x0,int y0,int x1,int y1);

    template<class T,class S,class U>
    void extract_bat(narray<T> &out,narray<S> &image,int x0,int y0,int x1,int y1,U dflt) {
        x0 = max(x0,0);
        y0 = max(y0,0);
        x1 = min(x1,image.dim(0));
        y1 = min(y1,image.dim(1));
        int w = x1-x0;
        int h = y1-y0;
        out.resize(w,h);
        for (int i=0; i<w; i++)
            for (int j=0; j<h; j++)
                out(i,j) = (T)bat(image,x0+i,y0+j,dflt);
    }
    template void extract_bat(bytearray &out,bytearray &image,int x0,int y0,int x1,int y1,int value);
    template void extract_bat(floatarray &out,floatarray &image,int x0,int y0,int x1,int y1,float value);
    template void extract_bat(intarray &out,intarray &image,int x0,int y0,int x1,int y1,int value);
    template void extract_bat(floatarray &out,bytearray &image,int x0,int y0,int x1,int y1,int value);
    template void extract_bat(bytearray &out,floatarray &image,int x0,int y0,int x1,int y1,float value);
    template void extract_bat(intarray &out,bytearray &image,int x0,int y0,int x1,int y1,int value);
    template void extract_bat(bytearray &out,intarray &image,int x0,int y0,int x1,int y1,int value);
    template void extract_bat(intarray &out,floatarray &image,int x0,int y0,int x1,int y1,float value);
    template void extract_bat(floatarray &out,intarray &image,int x0,int y0,int x1,int y1,int value);

    namespace {
        float clip(float x,float lo,float hi) {
            if (x<lo)
                return lo;
            if (x>hi)
                return hi;
            return x;
        }
    }

    template<class T>
    void linearly_transform_intensity(narray<T> &image,float m,float b,float lo,float hi) {
        for (int i=0; i<image.length1d(); i++)
            image.at1d(i) = T(clip(image.at1d(i) * m + b,lo,hi));
    }
    template void linearly_transform_intensity(bytearray &,float,float,float,float);
    template void linearly_transform_intensity(intarray &,float,float,float,float);
    template void linearly_transform_intensity(floatarray &,float,float,float,float);

    template<class T>
    void gamma_transform(narray<T> &image,float gamma,float c,float lo,float hi) {
        for (int i=0; i<image.length1d(); i++)
            image.at1d(i) = T(clip(c * pow(image.at1d(i),gamma),lo,hi));
    }
    template void gamma_transform(bytearray &,float,float,float,float);
    template void gamma_transform(intarray &,float,float,float,float);
    template void gamma_transform(floatarray &,float,float,float,float);

    template<class T>
    void expand_range(narray<T> &image,float lo,float hi) {
        float mn = min(image);
        float mx = max(image);
        if (mn==mx) {
            fill(image,T(lo));
            return;
        }
        for (int i=0; i<image.length1d(); i++) {
            float v = image.at1d(i);
            v = clip((v-mn) * (hi-lo) / (mx-mn) + lo,lo,hi);
            image.at1d(i) = T(v);
        }
    }
    template void expand_range(bytearray &,float,float);
    template void expand_range(intarray &,float,float);
    template void expand_range(floatarray &,float,float);
}
