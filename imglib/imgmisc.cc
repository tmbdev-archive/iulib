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
// File: imgmisc.cc
// Purpose: misc operations (peaks, valleys, etc.)
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

#include "colib/colib.h"
#include "imgmisc.h"
#include "imglib.h"

using namespace colib;

namespace iulib {

    /// Find local minima in a 1D array.

    void valleys(intarray &locations, floatarray &a, int minsize, int maxsize,
            float sigma) {
        locations.clear();
        floatarray v;
        copy(v, a);
        if (sigma>0)
            gauss1d(v, sigma);
        int i0 = max(1, minsize);
        int i1 = min(v.length()-1, maxsize);
        bool negslope = false; // Flag to check if a negative slope has occurred
        for (int i=i0; i<i1; i++) {
            if (v[i]<v[i-1])
                negslope = true;
            if (negslope && v[i]<v[i+1]) {
                locations.push(i);
                negslope = false;
            }
        }
    }

    /// Find local maxima in a 1D array.

    void peaks(intarray &locations, floatarray &a, int minsize, int maxsize,
            float sigma) {
        locations.clear();
        floatarray v;
        copy(v, a);
        if (sigma>0)
            gauss1d(v, sigma);
        int i0 = max(1, minsize);
        int i1 = min(v.length()-1, maxsize);
        bool posslope = false;
        for (int i=i0; i<i1; i++) {
            if (v[i]>v[i-1])
                posslope = true;
            if (posslope && v[i]>v[i+1]) {
                locations.push(i);
                posslope = false;
            }
        }
    }

    /// Compute the histogram of an image.

    void hist(floatarray &hist, bytearray &image) {
        hist.resize(256);
        fill(hist, 0);
        for (int i=0; i<image.length1d(); i++)
            hist(image.at1d(i))++;
    }

    // FIXME comments

    void binarize_by_threshold(bytearray &image) {
        int threshold = (min(image) + max(image)) / 2;
        for(int i = 0; i < image.length1d(); i++) {
            image.at1d(i) = image.at1d(i) < threshold ? 0 : 255;
        }
    }


    void check_page_segmentation(intarray &pseg) {
        bool allow_zero = true;
        narray<bool> used(5000);
        fill(used,false);
        int nused = 0;
        int mused = 0;
        for(int i=0;i<pseg.length1d();i++) {
            unsigned pixel = (unsigned)pseg.at1d(i);
            CHECK_ARG(allow_zero || pixel!=0);
            if(pixel==0 || pixel==0xffffff) continue;
            int column = 0xff & (pixel >> 16);
            int paragraph = 0xff & (pixel >> 8);
            int line = 0xff & pixel;
            CHECK_ARG((column > 0 && column < 32) || column == 254 || column == 255);
            CHECK_ARG((paragraph >= 0 && paragraph < 64) || (paragraph >=250 && paragraph <= 255));
            if (column < 32) {
                if(!used(line)) nused++;
                used(line) = true;
                if(line>mused) mused = line;
            }
        }
        // character segments need to be numbered sequentially (no gaps)
        // (gaps usually happen when someone passes a binary image instead of a segmentation)
        if(!(nused==mused || nused==mused+1))
            debugf("warn","check_page_segmentation found non-sequentially numbered segments\n");
    }

    void make_page_segmentation_black(intarray &a) {
        check_page_segmentation(a);
        replace_values(a, 0xFFFFFF, 0);
    }

    void make_page_segmentation_white(intarray &a) {
        replace_values(a, 0, 0xFFFFFF);
        check_page_segmentation(a);
    }

    // A valid line segmentation may contain 0 or 0xffffff as the
    // background, and otherwise numbers components starting at 1.
    // The segmentation consists of segmented background pixels
    // (0x80xxxx) and segmented foreground pixels (0x00xxxx).  The
    // segmented foreground pixels should constitute a usable
    // binarization of the original image.

    void check_line_segmentation(intarray &cseg) {
        if(cseg.length1d()==0) return;
        CHECK_ARG(cseg.rank()==2);
        for(int i=0;i<cseg.length1d();i++) {
            int value = cseg.at1d(i);
            if(value==0) continue;
            if(value==0xffffff) continue;
            if(value&0x800000)
                CHECK_ARG((value&~0x800000)<100000);
            else
                CHECK_ARG(value<100000);
        }
    }

    void make_line_segmentation_black(intarray &a) {
        check_line_segmentation(a);
        replace_values(a, 0xFFFFFF, 0);
        for(int i = 0; i < a.length1d(); i++)
            a.at1d(i) &= 0xFFF;
    }

    void make_line_segmentation_white(intarray &a) {
        replace_values(a, 0, 0xFFFFFF);
        //for(int i = 0; i < a.length1d(); i++)
        //    a.at1d(i) = (a.at1d(i) & 0xFFF) | 0x1000;
        check_line_segmentation(a);
    }
}
