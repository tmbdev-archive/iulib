// -*- C++ -*-

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
// Copyright 1995-2005 Thomas M. Breuel.
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
// File: coords.h
// Purpose: points and rectangles with integer coordinates
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file coords.h
/// \brief Points and rectangles with integer coordinates.

#ifndef h_coords_
#define h_coords_

#include <stdio.h>
#include "misc.h"

namespace colib {

    struct point {
        int x,y;
        point() {}
        point(int x,int y)
            :x(x),y(y) {
        }
    };

    struct rectangle {
        int x0,y0,x1,y1;
        rectangle()
            :x0(0),y0(0),x1(-1),y1(-1) {
        }
        rectangle(const rectangle &r)
            :x0(r.x0),y0(r.y0),x1(r.x1),y1(r.y1) {
        }
        rectangle(int x0,int y0,int x1,int y1)
            :x0(x0),y0(y0),x1(x1),y1(y1) {
        }
        bool empty() {
            return x0>=x1 || y0>=y1;
        }
        void pad_by(int dx,int dy) {
            ASSERT(!empty());
            x0 -= dx;
            y0 -= dy;
            x1 += dx;
            y1 += dy;
        }
        void shift_by(int dx,int dy) {
            ASSERT(!empty());
            x0 += dx;
            y0 += dy;
            x1 += dx;
            y1 += dy;
        }
        int width() {
            int w = x1-x0;
            return (w<0)?0:w;
        }
        int height() {
            int h = y1-y0;
            return (h<0)?0:h;
        }
        void include(int x,int y) {
            if(empty()) {
                x0 = x;
                y0 = y;
                x1 = x+1;
                y1 = y+1;
            } else {
                x0 = min(x,x0);
                y0 = min(y,y0);
                x1 = max(x+1,x1);
                y1 = max(y+1,y1);
            }
        }
        bool contains(int x,int y) {
            return x>=x0 && x<x1 && y>=y0 && y<y1;
        }
        bool contains(point p) {
            return p.x>=x0 && p.x<x1 && p.y>=y0 && p.y<y1;
        }
        void intersect(rectangle other) {
            if(empty()) return;
            x0 = max(x0,other.x0);
            y0 = max(y0,other.y0);
            x1 = min(x1,other.x1);
            y1 = min(y1,other.y1);
        }
        void include(rectangle other) {
            if(empty()) {
                *this = other;
            } else {
                x0 = min(x0,other.x0);
                y0 = min(y0,other.y0);
                x1 = max(x1,other.x1);
                y1 = max(y1,other.y1);
            }
        }
        rectangle intersection(rectangle other) {
            if(empty()) return *this;
            return rectangle(
                max(x0,other.x0),
                max(y0,other.y0),
                min(x1,other.x1),
                min(y1,other.y1));
        }
        rectangle inclusion(rectangle other) {
            if(empty()) return other;
            return rectangle(
                min(x0,other.x0),
                min(y0,other.y0),
                max(x1,other.x1),
                max(y1,other.y1));
        }
        // FIXME see what that should do if empty()
        rectangle grow(int offset) {
            if(empty()) throw "grow: rectangle is empty";
            return rectangle(x0-offset,y0-offset,x1+offset,y1+offset);
        }
        int xcenter(){
            return (x0+x1)/2;
        }
        int ycenter(){
            return (y0+y1)/2;
        }
        void print(FILE *stream=stdout) {
            fprintf(stream,"%d %d %d %d",x0,y0,x1,y1);
        }
        void println(FILE *stream=stdout) {
            fprintf(stream,"%d %d %d %d\n",x0,y0,x1,y1);
        }
        int area() {
            return heaviside(x1-x0)*heaviside(y1-y0);
        }
        bool overlaps(const rectangle &other) {
            return
                x0<=other.x1 && x1>=other.x0 &&
                y0<=other.y1 && y1>=other.y0;
        }
        bool includes(int x,int y) {
            return (x >= x0 && x <= x1 && y >= y0 && y <= y1);
        }

        bool includes(float x,float y) {
            return (x >= x0 && x <= x1 && y >= y0 && y <= y1);
        }

        bool includes(const rectangle &other) {
            return this->includes(other.x0,other.y0) && this->includes(other.x1,other.y1);
        }
        rectangle dilated_by(int dx0,int dy0,int dx1,int dy1) {
            return rectangle(x0-dx0,y0-dy0,x1+dx1,y1+dy1);
        }
        float aspect() {
            return (y1-y0)/(float)(x1-x0);
        }
        float centricity(const rectangle &other) {
            float width = x1-x0;
            float height = y1-y0;
            return
                (other.x1-x0)/width *
                (other.y1-y0)/height *
                (x1-other.x0)/width *
                (y1-other.y0)/height;
        }
        float fraction_covered_by(const rectangle &other) {
            rectangle isect = this->intersection(other);
            if(area())
                return isect.area()/(float)area();
            else
                return -1;
        }
    };

    typedef narray<rectangle> rectarray;

}

#endif
