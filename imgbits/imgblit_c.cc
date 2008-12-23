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
// File: imgblit_c.cc
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
#include "imgbits.h"
#include "imgbitptr.h"
#include "imgmorph.h"
#include "imgops.h"
// #include "dgraphics.h"

#define SIMPLE_BIT_COPIES 0

namespace imgbits {
    using namespace colib;
    using namespace iulib;
    using namespace imgbits;

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

    // simple bit operations

    static inline bool getbit(word32 *w,int i) {
        int r = i>>5;
        i &= 0x1f;
        return w[r] & (0x80000000>>i);
    }

    static inline void setbit(word32 *w,int i,bool value) {
        int r = i>>5;
        i &= 0x1f;
        if(value) {
            w[r] |= (0x80000000>>i);
        } else {
            w[r] &= ~(0x80000000>>i);
        }
    }

    // minimal 64 bit word datatype, used for simplifying a special case below

    struct word64 {
        word32 left,right;
        word64() {}
        word64(word32 left,word32 right):left(left),right(right) {};
        word32 &operator[](int i) {
            if(i==0) return left;
            else return right;
        }
        void operator<<=(int n) {
            left <<= n;
            left |= (right>>(32-n));
            right <<= n;
        }
        void operator>>=(int n) {
            right >>= n;
            right |= (left<<(32-n));
            left >>= n;
        }
        void operator|=(word64 other) {
            left |= other.left;
            right |= other.right;
        }
        void operator&=(word64 other) {
            left &= other.left;
            right &= other.right;
        }
        word64 operator~() {
            return word64(~left,~right);
        }
        void setbits(int start,int end) {
            ASSERT(start<end);
            if(start<0) start = 0;
            if(end>=64) end = 64;
            if(start>=32) {
                int n = end-start;
                left = 0;
                if(n==32) right = ~0;
                else right = ((1<<n)-1)<<(64-start-n);
            } else if(end<=32) {
                int n = end-start;
                if(n==32) left = ~0;
                else left = ((1<<n)-1)<<(32-start-n);
                right = 0;
            } else {
                if(start==0) left = ~0;
                else left = (1<<(32-start))-1;
                if(end==64) right = ~0;
                else right = ((1<<end)-1)<<(64-end);
            }
        }
    };

    // global variables used for some bounds checking

#ifndef UNSAFE
    static word32 *endmask,*enddest;
#endif

    template <class C>
    struct RowOpBitwiseC {
        static void go(word32 *dest,int enddestbits,
                       word32 *mask,int endmaskbits,
                       int shift) {
            int db = (shift>0)?shift:0;
            int mb = (shift<0)?-shift:0;

            for(;db<enddestbits && mb<endmaskbits;db++,mb++) {
                word32 dbit = !!getbit(dest,db);
                word32 mbit = !!getbit(mask,mb);
                word32 out = C::combine(dbit,mbit);
                setbit(dest,db,out);
            }
        }
    };

    template <class C>
    struct RowOpWordwiseC {
        // combine bits from mask with dest, starting at bit db/mb
        // and going up to enddestbits/endmaskbits
        //
        // trivial_copy_bits should only be called for less than
        // 32 bits of work, and all the pointers should already
        // have been adjusted so that the db and mb bit offsets
        // fall within the first word

        static void trivial_copy_bits(word32 *dest,int enddestbits,int &db,
                                      word32 *mask,int endmaskbits,int &mb) {
            ASSERT(unsigned(db<32));
            ASSERT(db<=enddestbits);
            ASSERT(unsigned(mb)<32);
            ASSERT(mb<=endmaskbits);
            ASSERT(enddestbits<64||endmaskbits<64);
            if(SIMPLE_BIT_COPIES) {
                for(;db<enddestbits && mb<endmaskbits;db++,mb++) {
                    word32 dbit = !!getbit(dest,db);
                    word32 mbit = !!getbit(mask,mb);
                    word32 out = C::combine(dbit,mbit);
                    setbit(dest,db,out);
                }
            } else {
                int n = min(enddestbits-db,endmaskbits-mb);
                if(n==0) return;

                word64 dv,mv,dmsk,mmsk;
                dv[0] = dest[0];
                if(enddestbits>=32) dv[1] = dest[1]; else dv[1] = 0;
                dmsk.setbits(db,enddestbits);

                mv[0] = mask[0];
                if(endmaskbits>=32) mv[1] = mask[1]; else mv[1] = 0;
                mmsk.setbits(mb,endmaskbits);
            
                if(db<mb) {mv <<= (mb-db); mmsk <<= (mb-db);}
                else if(db>mb) {mv >>= (db-mb); mmsk >>= (db-mb); }
                dmsk &= mmsk;

                if(dmsk[0])
                    dest[0] = (C::combine(dv[0],mv[0]) & dmsk[0]) | (dv[0] & ~dmsk[0]);
                if(dmsk[1])
                    dest[1] = (C::combine(dv[1],mv[1]) & dmsk[1]) | (dv[1] & ~dmsk[1]);

                db += n;
                mb += n;
            }
        }

        static void go(word32 *dest,int enddestbits,
                       word32 *mask,int endmaskbits,
                       int shift) {

#ifndef UNSAFE
            enddest = dest + ((enddestbits+31)/32);
            endmask = mask + ((endmaskbits+31)/32);
#endif

            // db and mb are "pointers" into the dest
            // and mask arrays
        
            int db = (shift>0)?shift:0;
            int mb = (shift<0)?-shift:0;

            // adjust the bit pointers so that we only have to
            // deal with values in the range of 0..31
        
            while(db>=32) {dest++; db-=32; enddestbits-=32;}
            while(mb>=32) {mask++; mb-=32; endmaskbits-=32;}

            // we use a fast loop with no shifts when things are
            // word aligned and when we have whole words to
            // process

            if(db==0 && mb==0) {
                while(enddestbits>=32 && endmaskbits>=32) {
                    word32 value = C::combine(*dest,*mask++);
#ifndef UNSAFE
                    ASSERT(dest<enddest);
#endif
                    *dest++ = value;
                    enddestbits -= 32;
                    endmaskbits -= 32;
                }
            }

            // advance db to the next word boundary; this converts
            // the case of negative shifts into the same case as postive
            // shifts

            if(db>0) {
                if(enddestbits>=32)
                    trivial_copy_bits(dest,32,db,mask,endmaskbits,mb);
                if(db<32) {
                    // I think the old assertion just said that there shouldn't be "too many"
                    // slowly copied bits, but it was failing sometimes. I think we can get
                    // 32 bits from "either side", so there could be as many as 64 bits. --tmb
                    // ASSERT(db+(endmaskbits-mb)<32); <-- this was the old assertion
                    ASSERT(db+(endmaskbits-mb)<64);
                    // couldn't copy enough bits (ran out of either
                    // dest or mask bits), so let's just finish up and quit
                    trivial_copy_bits(dest,enddestbits,db,mask,endmaskbits,mb);
                    return;
                }
                dest++;
                db -= 32;
                enddestbits -= 32;
            }

            ASSERT(db==0);
            ASSERT(unsigned(mb)<32);

            // do block copies for the bulk of the bits
        
            if(db==0 && enddestbits>=32 && (endmaskbits>=64 || (endmaskbits-mb)>=32)) {
                // above, the second condition assures that we have a
                // whole word to write into, and the third condition assures
                // that we have a whole mask word to read
                int rmb = 32 - mb;
                while(enddestbits>=32 && (endmaskbits>=64 || (endmaskbits-mb)>=32)) {
#ifndef UNSAFE
                    ASSERT(mask<endmask);
#endif
                    word32 current = *mask++;
#ifndef UNSAFE
                    ASSERT(mask<endmask);
#endif
                    // NB: we could avoid a memory reference here
                    // by having special purpose startup code and
                    // then saying "current = next" at the beginning
                    // (TODO?)
                    word32 next = *mask;
                    word32 shmask = ((current << mb) | (next >> rmb));
                    word32 value = C::combine(*dest,shmask);
#ifndef UNSAFE
                    ASSERT(dest<enddest);
#endif
                    *dest++ = value;
                    enddestbits -= 32;
                    endmaskbits -= 32;
                }
            }

            // take care of any leftover bits

            if(db<enddestbits && mb<endmaskbits)
                trivial_copy_bits(dest,enddestbits,db,mask,endmaskbits,mb);
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
                CHECK_ARG(0);
                //CHECK_ARG(("bad blit type",0));
            }

            if(temp) delete temp;
        }
    };

    IBlit1D *make_Blit1DBitwiseC() {
        return new Blit1D<RowOpBitwiseC>();
    }

    IBlit1D *make_Blit1DWordwiseC() {
        return new Blit1D<RowOpWordwiseC>();
    }
}
