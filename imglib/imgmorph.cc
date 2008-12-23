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
// File: imgmorph.cc
// Purpose: simple grayscale morphology based on local min/max
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

    inline byte bc(int c) {
        if(c<0)
            return 0;
        if(c>255)
            return 255;
        return c;
    }

    void make_binary(bytearray &image) {
        for(int i=0; i<image.length1d(); i++)
            image.at1d(i) = image.at1d(i) ? 255 : 0;
    }

    void check_binary(bytearray &image) {
        for(int i=0; i<image.length1d(); i++) {
            int value = image.at1d(i);
            CHECK_ARG(value==0 || value==255);
        }
    }

    void binary_invert(bytearray &image) {
        check_binary(image);
        for(int i=0; i<image.length1d(); i++)
            image.at1d(i) = 255-image.at1d(i);
    }

    void binary_autoinvert(bytearray &image) {
        check_binary(image);
        int count = 0;
        for(int i=0; i<image.length1d(); i++)
            if(image.at1d(i)) count++;
        if(count>image.length1d()/2)
            binary_invert(image);
    }

    void binary_and(bytearray &image, bytearray &image2, int dx, int dy) {
        int w = image.dim(0);
        int h = image.dim(1);
        for(int i=0; i<w; i++)
            for(int j=0; j<h; j++) {
                image.at(i, j) = min(image(i, j), ext(image2, i-dx, j-dy));
            }
    }

    void binary_or(bytearray &image, bytearray &image2, int dx, int dy) {
        int w = image.dim(0);
        int h = image.dim(1);
        for(int i=0; i<w; i++)
            for(int j=0; j<h; j++) {
                image.at(i, j) = max(image.at(i, j), ext(image2, i-dx, j-dy));
            }
    }

    void binary_erode_circle(bytearray &image, int r) {
        if(r==0)
            return;
        bytearray out;
        copy(out, image);
        for(int i=-r; i<=r; i++)
            for(int j=-r; j<=r; j++) {
                if(i*i+j*j<=r*r)
                    binary_and(out, image, i, j);
            }
        move(image, out);
    }

    void binary_dilate_circle(bytearray &image, int r) {
        if(r==0)
            return;
        bytearray out;
        copy(out, image);
        for(int i=-r; i<=r; i++)
            for(int j=-r; j<=r; j++) {
                if(i*i+j*j<=r*r)
                    binary_or(out, image, i, j);
            }
        move(image, out);
    }

    void binary_open_circle(bytearray &image, int r) {
        if(r==0)
            return;
        binary_erode_circle(image, r);
        binary_dilate_circle(image, r);
    }

    void binary_close_circle(bytearray &image, int r) {
        if(r==0)
            return;
        binary_dilate_circle(image, r);
        binary_erode_circle(image, r);
    }

    void binary_erode_rect(bytearray &image, int rw, int rh) {
        if(rw==0&&rh==0)
            return;
        bytearray out;
        copy(out, image);
        for(int i=0; i<rw; i++)
            binary_and(out, image, i-rw/2, 0);
        for(int j=0; j<rh; j++)
            binary_and(image, out, 0, j-rh/2);
    }

    void binary_dilate_rect(bytearray &image, int rw, int rh) {
        if(rw==0&&rh==0)
            return;
        bytearray out;
        copy(out, image);
        // note that we handle the even cases complementary
        // to erode_rect; this makes open_rect and close_rect
        // do the right thing
        for(int i=0; i<rw; i++)
            binary_or(out, image, i-(rw-1)/2, 0);
        for(int j=0; j<rh; j++)
            binary_or(image, out, 0, j-(rh-1)/2);
    }

    void binary_open_rect(bytearray &image, int rw, int rh) {
        if(rw==0&&rh==0)
            return;
        binary_erode_rect(image, rw, rh);
        binary_dilate_rect(image, rw, rh);
    }

    void binary_close_rect(bytearray &image, int rw, int rh) {
        if(rw==0&&rh==0)
            return;
        binary_dilate_rect(image, rw, rh);
        binary_erode_rect(image, rw, rh);
    }

}
