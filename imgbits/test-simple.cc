// Copyright 2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
// Copyright 1992-2007 Thomas M. Breuel
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
// Project: imgbits
// File: test-simple.cc
// Purpose:
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de, www.ocropus.org


/* Copyright (c) Thomas M. Breuel */

#include <stdlib.h>
#include <map>
#define DEBUG_TEST abort()
#include "colib/checks.h"
#include "colib/narray.h"
#include "colib/narray-util.h"
#include "colib/colib.h"
#include "coords.h"
#include "imgio.h"
#include "io_png.h"
#include "imgbitptr.h"
#include "imgbits.h"
#include "imgrle.h"
#include "imgmisc.h"
#include "imgmorph.h"
#include "imgops.h"
//#include "ocrcomponents.h"
//#include "dgraphics.h"
using namespace std;
using namespace colib;
using namespace iulib;
using namespace imgrle;

int urand(int lo,int hi) {
    return rand()%(hi-lo)+lo;
}

int main(int argc,char **argv) {
    try {
        srand(0);
        // dinit(400,400);
        RLEImage ri,ri2,temp,temp2;

        // Test the "put" routine... there are many
        // different special cases, that's why there are
        // so many tests here.

        ri.resize(8,8);
        ri.fill(0);
        TEST_EQ(rle_count_bits(ri),0);
        rle_invert(ri);
        TEST_EQ(rle_count_bits(ri),64);
        ri.fill(0);
        TEST_EQ(rle_count_bits(ri),0);
        ri.put(2,2,1);
        TEST_EQ(rle_count_bits(ri),1);
        ri.put(2,6,1);
        TEST_EQ(rle_count_bits(ri),2);
        ri.put(6,2,1);
        TEST_EQ(rle_count_bits(ri),3);
        ri.put(2,3,1);
        TEST_EQ(rle_count_bits(ri),4);
        ri.put(2,4,1);
        TEST_EQ(rle_count_bits(ri),5);
        ri.put(2,3,0);
        TEST_EQ(rle_count_bits(ri),4);
        ri.put(2,3,1);
        TEST_EQ(rle_count_bits(ri),5);

        // Check whether inverting images gives the right number of pixels.

        for(int round=0;round<100;round++) {
            ri.resize(urand(4,100),urand(4,100));
            int npixels = urand(5,50);
            for(int pixel=0;pixel<npixels;pixel++)
                ri.put(rand()%ri.dim(0),rand()%ri.dim(1),1);
            int before = rle_count_bits(ri);
            TEST_ASSERT(before>0 && before<=npixels);
            rle_invert(ri);
            TEST_EQ(ri.dim(0)*ri.dim(1)-rle_count_bits(ri),before);
        }

        // Check whether pixels move around correctly under shift.

        ri.resize(7,7);
        ri.fill(0);
        ri.put(4,4,1);
        for(int dx=-2;dx<=2;dx++) {
            for(int dy=-2;dy<=2;dy++) {
                rle_shift(ri,dx,dy);
                TEST_EQ(ri.at(dx+4,dy+4),1);
                if(dx!=0 || dy!=0) TEST_EQ(ri.at(4,4),0);
                rle_shift(ri,-dx,-dy);
                if(dx!=0 || dy!=0) TEST_EQ(ri.at(dx+4,dy+4),0);
                TEST_EQ(ri.at(4,4),1);
            }
        }

        // Check boundary conditions (incomplete).
        
        ri.resize(10,10);
        ri.fill(1);
        rle_erode_runs(ri,4);
        TEST_EQ(rle_count_bits(ri),70); // zero boundary conditions (?)

        // Check both transpose implementations and make sure that
        // transposing twice gives the original

        for(int trial=0;trial<100;trial++) {
            int w = urand(5,77);
            int h = urand(5,77);
            ri.resize(w,h);
            ri.fill(0);
            for(int k=0;k<100;k++)
                ri.put(urand(0,w),urand(0,h),1);
            ri2.copy(ri);
            TEST_ASSERT(ri2.equals(ri));
            rle_transpose_table(temp,ri);
            rle_transpose_table(ri,temp);
            TEST_ASSERT(ri2.equals(ri));
            rle_transpose_runs(temp2,ri);
            TEST_ASSERT(temp2.equals(temp));
            rle_transpose_runs(ri,temp2);
            TEST_ASSERT(ri2.equals(ri));
        }

        // Some simple tests for the rle_or etc. bit blit routines.

        ri.resize(15,15);
        for(int rx=-5;rx<5;rx++) {
            for(int ry=-5;ry<5;ry++) {
                ri.fill(0);
                ri.put(8,8,1);
                rle_or(ri,ri,rx,ry);
                if(rx==0&&ry==0) TEST_EQ(rle_count_bits(ri),1);
                else TEST_EQ(rle_count_bits(ri),2);
            }
        }

        // Check runlength-only dilations and erosions by checking how
        // many pixels are set after dilations and erosions.

        ri.resize(100,100);
        ri.fill(0);
        ri.put(50,50,1);
        rle_dshow(ri,"a");
        rle_dilate_rect_runlength(ri,4,4);
        rle_dshow(ri,"b");
        TEST_EQ(rle_count_bits(ri),16);
        rle_erode_rect_runlength(ri,4,4);
        TEST_EQ(rle_count_bits(ri),1);
        TEST_EQ(ri.at(50,50),1);

        ri.resize(100,100);
        for(int rx=1;rx<30;rx++) {
            for(int ry=1;ry<30;ry++) {
                ri.fill(0);
                int x = urand(40,60);
                int y = urand(40,60);
                ri.put(x,y,1);
                rle_dilate_rect_runlength(ri,rx,ry);
                TEST_EQ(rle_count_bits(ri),rx*ry);
                rle_erode_rect_runlength(ri,rx,ry);
                TEST_EQ(rle_count_bits(ri),1);
                TEST_EQ(ri.at(x,y),1);
            }
        }

        // Bruteforce dilations use direct repeated applications
        // of the bit blit routines; make sure they give the right
        // number of pixels for simple images.
        
        ri.resize(15,15);
        for(int rx=1;rx<5;rx++) {
            for(int ry=1;ry<5;ry++) {
                ri.fill(0);
                int x = urand(rx,ri.dim(0)-rx);
                int y = urand(ry,ri.dim(1)-ry);
                ri.put(x,y,1);
                rle_dilate_rect_bruteforce(ri,rx,ry);
                TEST_EQ(rle_count_bits(ri),rx*ry);
                rle_erode_rect_bruteforce(ri,rx,ry);
                TEST_EQ(rle_count_bits(ri),1);
                TEST_EQ(ri.at(x,y),1);
            }
        }

        // Same test for decomposed versions.
        
        ri.resize(15,15);
        for(int rx=1;rx<5;rx++) {
            for(int ry=1;ry<2;ry++) {
                ri.fill(0);
                int x = urand(rx,ri.dim(0)-rx);
                int y = urand(ry,ri.dim(1)-ry);
                ri.put(x,y,1);
                rle_dilate_rect_decomp(ri,rx,ry);
                TEST_EQ(rle_count_bits(ri),rx*ry);
                rle_erode_rect_decomp(ri,rx,ry);
                TEST_EQ(rle_count_bits(ri),1);
                TEST_EQ(ri.at(x,y),1);
            }
        }
    } catch(const char *message) {
        fprintf(stderr,"oops: %s\n",message);
    }
}
