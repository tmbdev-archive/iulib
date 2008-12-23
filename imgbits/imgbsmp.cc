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
// File: imgbsmp.cc
// Purpose:
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de, www.ocropus.org


/* Copyright (c) Thomas M. Breuel */

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
// #include "ocrinterfaces.h"
// #include "dgraphics.h"

#define D if(0)

namespace imgbits {
    using namespace colib;
    using namespace iulib;

    static unsigned char counts[256];

    static void init_counts() {
        if(counts[1]) return;
        for(int i=0;i<256;i++) {
            int total = 0;
            unsigned t = i;
            for(int j=0;j<8;j++) {
                if(t&1) total++;
                t >>= 1;
            }
            counts[i] = total;
        }
    }

    static inline int bitcount(word32 w) {
        unsigned char *p = (unsigned char *)&w;
        return counts[p[0]] + counts[p[1]] + counts[p[2]] + counts[p[3]];
    }

    ////////////////////////////////////////////////////////////////
    // on-the-fly resampling of bit images into grayscale image
    ////////////////////////////////////////////////////////////////

    void bits_resample_normed(bytearray &out,BitImage &in,int scale,bool norm) {
        init_counts();
        CHECK_ARG(scale>=1 && scale<=32);
        int w = in.dim(0);
        int h = in.dim(1);
        int ow = (in.dim(0)+scale-1)/scale;
        int oh = (in.dim(1)+scale-1)/scale;
        out.resize(ow,oh);
        fill(out,0);
        int i,di,row;
        int max = 0,min = 255;
        for(row=0;row<w;row++) {
            int drow = row/scale;
            word32 *p = in.get_line(row);
            for(i=0,di=0;i<h;i+=scale,di++) {
                int word = i>>5;
                int start = i&0x1f;
                int end = start+scale;
                int center = end<32?end:32;
                word32 mask;
                if(start==0) mask = ~0;
                else mask = (1<<(32-start))-1;
                mask &= ~((1<<(32-center))-1);
                int total = bitcount(p[word] & mask);
                if(end>32 && word+1 < in.words_per_row) {
                    mask = ~((1<<(64-end))-1);
                    total += bitcount(p[word+1] & mask);
                }
                out(drow,di) = total;
                if(norm) {
                    if(total>max) max = total;
                    if(total<min) min = total;
                }
            }
        }
        if(norm) {
            int diff = max-min;
            if(diff<=0) diff=1;
            int n = out.length();
            for(i=0;i<n;i++) out[i] = (out[i]*255-min)/diff;
        }
    }

    // code for resampling by small powers of two; this uses tables
    // for precomputing the outputs for groups of 16 bits

    namespace {
        static unsigned char *Resampler_tables[17];

        template <class C>
            struct Resampler {
                static const int out_bytes = C::out_bytes;
                typedef unsigned char (*Table)[out_bytes];

                static Table init() {
                    init_counts();
                    if(Resampler_tables[C::scale]) return (Table)Resampler_tables[C::scale];
                    Resampler_tables[C::scale] = new unsigned char[65536 * C::out_bytes];
                    Table table = (Table)Resampler_tables[C::scale];
                    for(int i=0;i<65536;i++) {
                        word32 w = i;
                        for(int j=0;j<C::out_bytes;j++) {
                            table[i][j] = bitcount(w & C::top_mask);
                            w <<= C::scale;
                        }
                    }
                    return (Table)Resampler_tables[C::scale];
                }

                // perform the resampling

                static void go(bytearray &out,BitImage &image) {
                    Table table = init();
                    out.resize((image.dim(0)+C::scale-1)/C::scale,
                            (image.dim(1)+C::scale-1)/C::scale);
                    fill(out,0);
                    for(int i=0,out_i=0;i<image.dim(0);i+=C::scale,out_i++) {
                        for(int k=i;k<i+C::scale && k<image.dim(0);k++) {
                            BitSrc p(image.get_line(k),image.dim(1));
                            int out_j = 0;
                            for(;;) {
                                // get a chunk of 16 bits; note that get16 does allow
                                // us to get a few bits past the end of the array to
                                // get a whole chunk

                                int index = p.get16();

                                // get the corresponding resampled values

                                unsigned char *bytes = &table[index][0];

                                // now, add them into the output array

                                for(int j=0;j<C::out_bytes;j++) {
                                    if(out_j>=out.dim(1)) break;
                                    if(C::scale==16) {
                                        // could overflow to 256 when scale==16
                                        unsigned v = out(out_i,out_j) + *bytes++;
                                        if(v>255) v = 255;
                                        out(out_i,out_j++) = v;
                                    } else {
                                        out(out_i,out_j++) += *bytes++;
                                    }
                                }
                                if(out_j>=out.dim(1)) break;
                            }
                        }
                    }
                }
            };

        struct Res_2 { enum { scale = 2, out_bytes = 8, top_mask = 0xC000 }; };
        struct Res_4 { enum { scale = 4, out_bytes = 4, top_mask = 0xF000 }; };
        struct Res_8 { enum { scale = 8, out_bytes = 2, top_mask = 0xFF00 }; };
        struct Res_16 { enum { scale = 16, out_bytes = 1, top_mask = 0xFFFF }; };
    }

    void bits_resample(bytearray &out,BitImage &in,int scale) {
        if(scale==2) Resampler<Res_2>::go(out,in);
        else if(scale==4) Resampler<Res_4>::go(out,in);
        else if(scale==8) Resampler<Res_8>::go(out,in);
        else if(scale==16) Resampler<Res_16>::go(out,in);
        else bits_resample_normed(out,in,scale,false);
    }

    // this is specialized code for factor two binary reductions, combining
    // bits with "and"

    namespace {
        unsigned char reduce2_and_table[65536];

        void init_reduce2() {
            if(reduce2_and_table[65535]) return;
            for(unsigned i=0;i<65536;i++) {
                unsigned result;
                result = 0;
                for(int j=0;j<8;j++) {
                    result <<= 1;
                    if(i<<(2*j) & (i<<(2*j+1)) && 0x8000) result |= 1;
                }
                reduce2_and_table[i] = result;
            }
        }

        void bits_reduce2_and_h(word32 *out,word32 *in,int nbits) {
            while(nbits>0) {
                word32 u,w;
                u = *in++;
                w = reduce2_and_table[u>>16];
                w <<= 8;
                w |= reduce2_and_table[u&0xffff];
                if(nbits>=32) u = *in++; else u = 0;
                w <<= 8;
                w |= reduce2_and_table[u>>16];
                w <<= 8;
                w |= reduce2_and_table[u&0xffff];
                *out++ = w;
                nbits -= 64;
            }
        }
    }

    void bits_reduce2_and(BitImage &out,BitImage &image) {
        out.resize(image.dim(0),image.dim(1));
        int i,j;
        dyna<word32> r1(new word32[out.words_per_row]);
        dyna<word32> r2(new word32[out.words_per_row]);
        for(i=0;i<out.dim(0);i++) {
            bits_reduce2_and_h(r1,image.get_line(2*i),image.dim(1));
            bits_reduce2_and_h(r2,image.get_line(2*i+1),image.dim(1));
            word32 *p = out.get_line(i);
            for(j=0;j<out.words_per_row;j++)
                p[i] = r1[i] & r2[i];
        }
    }
}
