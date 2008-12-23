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
// File: imgblit.cc
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
// #include "dgraphics.h"

namespace imgbits {
    using namespace colib;
    using namespace iulib;

    static void set_bits_row_checked(word32 *row,int n,int from,int to,word32 value) {
        if(from<0) from = 0;
        if(to>n) to = n;
        if(from>=to) return;
        BitSnk bp(row,to,from);
        while(bp.has_bits() && !bp.is_wordaligned()) bp.putbit(value);
        while(bp.has_words()) bp.putword_aligned(value);
        while(bp.has_bits()) bp.putbit(value);
    }

    static void fix_boundary(word32 *dest,int destbits,int maskbits,int dy,BlitBoundary op) {
        switch(op) {
        case BLITB_KEEP:
            break;
        case BLITB_CLEAR:
            set_bits_row_checked(dest,destbits,0,dy,0);
            set_bits_row_checked(dest,destbits,maskbits+dy,destbits,0);
            break;
        case BLITB_SET:
            set_bits_row_checked(dest,destbits,0,dy,0);
            set_bits_row_checked(dest,destbits,maskbits+dy,destbits,0);
            break;
        }
    }

    struct OpSet {
        static inline word32 combine(word32 &,word32 source) {return source;}};
    struct OpSetNot {
        static inline word32 combine(word32 &,word32 source) {return ~source;}};
    struct OpAnd {
        static inline word32 combine(word32 &dest,word32 source) {return dest&source;}};
    struct OpOr {
        static inline word32 combine(word32 &dest,word32 source) {return dest|source;}};
    struct OpAndNot {
        static inline word32 combine(word32 &dest,word32 source) {return dest&(~source);}};
    struct OpOrNot {
        static inline word32 combine(word32 &dest,word32 source) {return dest|(~source);}};
    struct OpXor {
        static inline word32 combine(word32 &dest,word32 source) {return dest^source;}};

    template <class C>
    struct RowOpBitwise {
        static void go(word32 *dest,int enddestbits,
                       word32 *mask,int endmaskbits,
                       int shift) {
            ASSERT(dest!=mask);
            int db = (shift>0)?shift:0;
            int mb = (shift<0)?-shift:0;

            BitSrc sp(dest,enddestbits,db);
            BitSrc mp(mask,endmaskbits,mb);
            BitSnk dp(dest,enddestbits,db);

            while(sp.has_bits() && mp.has_bits()) {
                word32 dbit = sp.getbit();
                word32 mbit = mp.getbit();
                word32 out = (C::combine(dbit,mbit)&1);
                dp.putbit(out);
            }
        }
    };

    template <class C>
    struct RowOpWordwise {
        static void go(word32 *dest,int enddestbits,
                       word32 *mask,int endmaskbits,
                       int shift) {
            ASSERT(dest!=mask);
            int db = (shift>0)?shift:0;
            int mb = (shift<0)?-shift:0;

            BitSrc sp(dest,enddestbits,db);
            BitSrc mp(mask,endmaskbits,mb);
            BitSnk dp(dest,enddestbits,db);

            // first, perform bitwise operations until sp (and dp)
            // are wordaligned

            while(sp.has_bits() && mp.has_bits() && !sp.is_wordaligned()) {
                word32 dbit = sp.getbit();
                word32 mbit = mp.getbit();
                word32 out = (C::combine(dbit,mbit)&1);
                dp.putbit(out);
            }
            ASSERT(sp.is_wordaligned() == dp.is_wordaligned());

            // next, for the main loop, process 32bit words as whole
            // chunks (note that the mask is not necessary wordaligned)

            while(sp.has_words() && mp.has_words()) {
                word32 dbit = sp.getword_aligned();
                word32 mbit = mp.getword();
                word32 out = C::combine(dbit,mbit);
                dp.putword_aligned(out);
            }

            // last, perform bitwise operations for any remaining bits

            while(sp.has_bits() && mp.has_bits()) {
                word32 dbit = sp.getbit();
                word32 mbit = mp.getbit();
                word32 out = (C::combine(dbit,mbit)&1);
                dp.putbit(out);
            }
        }
    };

    template < template <class T> class RowOp >
    struct Blit1D : IBlit1D {
        void blit1d(word32 *dest,int enddestbits,
                    word32 *mask,int endmaskbits,
                    int shift,
                    BlitOp op) {
            word32 *temp = 0;

            // check for equality to make horizontal self-blits work
            if(dest==mask) {
                int nwords = (enddestbits+31)/32;
                temp = new word32[nwords];
                memcpy(temp,mask,nwords * sizeof *temp);
                mask = temp;
            } else {
#if 0
                // the 2D blit routines may not give us overlapping but non-identical
                // FIXME this sometimes triggers... why?
                CHECK_ARG((unsigned long)(dest-mask)>(enddestbits+31)/32);
                CHECK_ARG((unsigned long)(dest-mask)>(endmaskbits+31)/32);
#endif
            }

            switch(op) {
            case BLIT_SET:
                RowOp<OpSet>::go(dest,enddestbits,mask,endmaskbits,shift);
                break;
            case BLIT_SETNOT:
                RowOp<OpSetNot>::go(dest,enddestbits,mask,endmaskbits,shift);
                break;
            case BLIT_AND:
                RowOp<OpAnd>::go(dest,enddestbits,mask,endmaskbits,shift);
                break;
            case BLIT_OR:
                RowOp<OpOr>::go(dest,enddestbits,mask,endmaskbits,shift);
                break;
            case BLIT_ANDNOT:
                RowOp<OpAndNot>::go(dest,enddestbits,mask,endmaskbits,shift);
                break;
            case BLIT_ORNOT:
                RowOp<OpOrNot>::go(dest,enddestbits,mask,endmaskbits,shift);
                break;
            default:
                CHECK_ARG(("bad blit type"&&0));
            }

            if(temp) delete temp;
        }
    };

    IBlit1D *make_Blit1DBitwise() {
        return new Blit1D<RowOpBitwise>();
    }

    IBlit1D *make_Blit1DWordwise() {
        return new Blit1D<RowOpWordwise>();
    }

    struct Blit2D : IBlit2D {
        int count;
        autodel<IBlit1D> lineblit;
        Blit2D(IBlit1D *blit) {
            lineblit = blit;
            count = 0;
        }
        IBlit1D &blit1d() {
            return *lineblit;
        }
        void blit2d(BitImage &image,BitImage &other,int dx,int dy,BlitOp op,BlitBoundary bop) {
            // choose the order of doing the rows so that image.bit_*(image,dx,dy) works
            int start = 0, end = image.dim(0), delta = 1;
            if(dx>0) { start = image.dim(0)-1; end = -1; delta = -1; }
            for(int i=start;i!=end;i+=delta) {
                int oi = i-dx;
                word32 *dest = image.get_line(i);
                int enddestbits = image.dim(1);
                if(unsigned(oi)>=unsigned(other.dim(0))) {
                    fix_boundary(dest,enddestbits,0,0,bop);
                } else {
                    word32 *mask = other.get_line(oi);
                    int endmaskbits = other.dim(1);
                    lineblit->blit1d(dest,enddestbits,mask,endmaskbits,dy,op);
                    fix_boundary(dest,enddestbits,endmaskbits,dy,bop);
                }
            }
            count++;
        }
        int getBlitCount() {
            return count;
        }
    };

    IBlit2D *make_Blit2D(IBlit1D *blit) {
        return new Blit2D(blit);
    }
}
