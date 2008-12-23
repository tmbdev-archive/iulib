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
// File: imgbthin.cc
// Purpose:
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de, www.ocropus.org


/* Copyright (c) Thomas M. Breuel */

#include "bithacks.h"
#include "colib/colib.h"
#include "colib/narray.h"
#include "colib/narray-util.h"
#include "quicksort.h"
#include "imgmorph.h"
#include "imgops.h"
#include "imgbits.h"
#include "imgbitptr.h"
#include "imgbrushfire.h"
#include "imgmisc.h"
#include "imgio.h"
#include "io_png.h"
// #include "dgraphics.h"

#define D if(0)

namespace imgbits {
    using namespace colib;
    using namespace iulib;

    ////////////////////////////////////////////////////////////////
    // mask parsing for binary morphology
    ////////////////////////////////////////////////////////////////

    void bits_parse_mask(BitImage &hit,const char *mask) {
        int rows = 0;
        int cols = 0;
        int n = strlen(mask);
        int col = 0;
        for(int i=0;i<n;i++) {
            if(mask[i]=='\n') {
                if(col>cols) cols=col;
                col = 0;
                rows++;
                continue;
            }
            col++;
        }
        if(mask[n-1]!='\n') rows++;
        hit.resize(cols,rows);
        hit.fill(0);
        int row = 0;
        col = 0;
        for(int i=0;i<n;i++) {
            if(mask[i]=='\n') {
                col = 0;
                row++;
                continue;
            }
            if(mask[i]=='+') {
                hit.set(col,rows-row-1,1);
            } else if(mask[i]==' '||mask[i]=='.') {
                // nothing
            } else {
                throw "bad mask";
            }
            col++;
        }
    }
 
    void bits_parse_hitmiss(BitImage &hit,BitImage &miss,const char *mask) {
        int rows = 0;
        int cols = 0;
        int n = strlen(mask);
        int col = 0;
        for(int i=0;i<n;i++) {
            if(mask[i]=='\n') {
                if(col>cols) cols=col;
                col = 0;
                rows++;
                continue;
            }
            col++;
        }
        if(mask[n-1]!='\n') rows++;
        hit.resize(cols,rows);
        hit.fill(0);
        miss.resize(cols,rows);
        miss.fill(0);
        int row = 0;
        col = 0;
        for(int i=0;i<n;i++) {
            if(mask[i]=='\n') {
                col = 0;
                row++;
                continue;
            }
            if(mask[i]=='+') {
                hit.set(col,rows-row-1,1);
            } else if(mask[i]=='-') {
                miss.set(col,rows-row-1,1);
            } else if(mask[i]==' '||mask[i]=='.') {
                // nothing
            } else {
                throw "bad mask";
            }
            col++;
        }
        for(int i=0;i<hit.dim(0);i++) for(int j=0;j<hit.dim(1);j++)
            CHECK_ARG(!(hit(i,j) && miss(i,j)));
    }

    ////////////////////////////////////////////////////////////////
    // thinning
    ////////////////////////////////////////////////////////////////

    struct MaskDef {
        const char *mask;
        int pass;
    }; 

    static MaskDef thinning_masks_trivial[] = {
        { "  +\n"
          "-++\n"
          "  +\n", 0},
        { "  +\n"
          "-++\n"
          " - \n", 0},
        { " - \n"
          "-++\n"
          "  +\n", 0},
        {0,0},
    };

    static void apply_masks_trivial(BitImage &image,MaskDef *masks) {
        int nmasks=0;
        while(masks[nmasks].mask) nmasks++;
        narray<BitImage> hits(nmasks),misses(nmasks);
        intarray passes(nmasks);
        for(int i=0;i<nmasks;i++) {
            bits_parse_hitmiss(hits[i],misses[i],masks[i].mask);
            passes[i] = masks[i].pass;
        }
        BitImage candidates;
        BitImage hit,miss;
        bool done = false;
        while(!done) {
            done = true;
            for(int angle=0;angle<360;angle+=90) {
                for(int i=0;i<nmasks;i++) {
                    candidates.copy(image);
                    hit.copy(hits[i]);
                    miss.copy(misses[i]);
                    bits_rotate_rect(hit,angle);
                    bits_rotate_rect(miss,angle);
                    bits_mask_hitmiss(candidates,hit,miss,1,1);
                    if(bits_non_empty(candidates)) done = false;
                    bits_andnot(image,candidates);
                }
            }
        }
    }

    // FIXME this thinning implementation is really just there
    // for working the hitmiss implementation a little; finish
    // this some time

    void bits_thin(BitImage &image,int neighborhood) {
        switch(neighborhood) {
        case -1:
            apply_masks_trivial(image,thinning_masks_trivial);
            break;
        case 4:
            throw "4-neighborhood thinning not implemented yet";
        case 6:
            throw "6-neighborhood thinning not implemented yet";
        case 8:
            throw "8-neighborhood thinning not implemented yet";
        default:
            throw "must call with a 4, 6, or 8 neighborhood (-1 for testing)";
        }
    }
}
