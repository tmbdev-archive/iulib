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
// File: imgthin.cc
// Purpose: image thinning
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#include "colib/colib.h"
#include "imglib.h"


using namespace colib;

namespace iulib {

    void thin(bytearray &uci) {
        enum {OFF=0, ON=1, SKEL=2, DEL=3};

        static char ttable[256] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
                0, 0, /* 00 */
                0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, /* 10 */
                0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 20 */
                0, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, /* 30 */
                0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, /* 40 */
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, /* 50 */
                0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, /* 60 */
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, /* 70 */
                0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, /* 80 */
                1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, /* 90 */
                1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* a0 */
                1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, /* b0 */
                0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, /* c0 */
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, /* d0 */
                0, 0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, /* e0 */
                0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0 /* f0 */
        };

        static int nx[]= { 1, 1, 0, -1, -1, -1, 0, 1 };
        static int ny[]= { 0, 1, 1, 1, 0, -1, -1, -1 };

        int w = uci.dim(0)-1;
        int h = uci.dim(1)-1;

        for (int i=0, n=uci.length1d(); i<n; i++) {
            if (uci.at1d(i))
                uci.at1d(i)=ON;
            else
                uci.at1d(i)=OFF;
        }

        int flag;
        do {
            flag=0;
            for (int j=0; j<8; j+=2) {
                for (int x=1; x<w; x++)
                    for (int y=1; y<h; y++) {
                        if (uci(x, y)!=ON)
                            continue;
                        if (uci(x+nx[j], y+ny[j])!=OFF)
                            continue;
                        int b = 0;
                        for (int i=7; i>=0; i--) {
                            b<<=1;
                            b|=(uci(x+nx[i], y+ny[i])!=OFF);
                        }
                        if (ttable[b])
                            uci(x, y)=SKEL;
                        else {
                            uci(x, y)=DEL;
                            flag=1;
                        }
                    }
                if (!flag)
                    continue;
                for (int x=1; x<w; x++)
                    for (int y=1; y<h; y++)
                        if (uci(x, y)==DEL)
                            uci(x, y)=OFF;
            }
        } while (flag);

        for (int i=0, n=uci.length1d(); i<n; i++) {
            if (uci.at1d(i)==SKEL)
                uci.at1d(i)=255;
            else
                uci.at1d(i)=0;
        }
    }

}
