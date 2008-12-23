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
// File: imgbitptr.h
// Purpose:
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de, www.ocropus.org


// -*- C++ -*-
/* Copyright (c) Thomas M. Breuel */

#ifndef imgbitptr_h__
#define imgbitptr_h__

#include "colib/checks.h"

namespace imgbits {
    typedef unsigned int word32;

    ////////////////////////////////////////////////////////////////
    // a simple "dynamic array pointer" smart pointer, used by the blit
    ////////////////////////////////////////////////////////////////

    template <class T>
        struct dyna {
            T *p;
            operator T*() {return p;}
            dyna(T *p=0):p(p) {}
            ~dyna() {if(p) delete [] p; p = 0;}
            void operator=(T *other) {if(p) delete p; p = other;}
            T &operator[](int i) { CHECK_CONDITION(p); return p[i]; }
            T *steal() { T *temp = p; p = 0; return temp; }
            private:
            dyna(dyna<T> &);
            void operator=(dyna<T> &);
            T *operator&();
            operator T&();
        };


    ////////////////////////////////////////////////////////////////
    // bit pointer data structures; allows us to step through the
    // bits of an array of words relatively painlessly.
    ////////////////////////////////////////////////////////////////

    struct BitSrc {
        word32 *p;
        int nbits;
        word32 cur;
        int ncur;

        // Initialize the BitSrc to point to nbits bits starting at an
        // offset of skip inside p.

        BitSrc(word32 *p_,int nbits_,int skip=0):p(p_),nbits(nbits_) {
//#ifndef UNSAFE
            // shut up bogus GNU C++ warnings
            cur = 0;
            ncur = 0;
//#endif
            if(skip<0) throw "negative skip not allowed";
            if(skip>=nbits) {nbits = 0; return;}
            while(skip>=32) {
                skip-=32; nbits-=32; p++;
            }
            //cur = 0;
            //ncur = 0;
            if(skip>0) getbits(skip);
        }

        // Check whether there are any bits left.

        bool has_bits() {
            return nbits>0;
        }

        // Check whether there are any words left.

        bool has_words() {
            return nbits>=32;
        }

        // Check whether the pointer is word-aligned.

        bool is_wordaligned() {
            ASSERT(ncur<32);
            return ncur==0;
        }

        // Make sure there are bits left to be used.

        void reload() {
            if(ncur==0) {
                cur = *p++;
                ncur = 32;
            }
        }

        // Number of bits that can be requested without moving to a new
        // word.

        int bits_left_in_word() {
            return ncur<nbits ? ncur : nbits;
        }

        // Get one bit from the bit sequence.

        word32 getbit() {
            ASSERT(nbits>0 && ncur<32);
            reload();
            word32 result = !!(cur & 0x80000000);
            cur <<= 1;
            ncur--;
            nbits--;
            return result;
        }

        word32 peekbit() {
            ASSERT(nbits>0 && ncur<32);
            if(nbits==0) {
                return !!(p[1] & 0x80000000);
            } else {
                return !!(cur & 0x80000000);
            }
        }

        void skipbit() {
            reload();
            cur <<= 1;
            ncur--;
            nbits--;
        }

        bool get_run(int &start,int &end) {
            if(nbits<1) return false;
            start = 0;
            word32 bit = 0;
            while(nbits>0) {
                bit = getbit();
                if(bit) break;
                if(is_wordaligned()) {
                    while(nbits>=32 && peekword_aligned()==0) {
                        skipword_aligned();
                        start += 32;
                    }
                } else {
                    start++;
                }
            }
            end = start;
            while(nbits>0) {
                bit = peekbit();
                if(!bit) break;
                skipbit();
                if(is_wordaligned()) {
                    while(nbits>=32 && peekword_aligned()==0xffffffff) {
                        skipword_aligned();
                        end += 32;
                    }
                } else {
                    end++;
                }
            }
            return true;
        }

        // Get 16 bits at a time; the word must be aligned.

        word32 get16() {
            ASSERT(nbits>0 && ncur<32);
            reload();
            if(ncur==32) {
                ncur = 16;
                word32 result = cur>>16;
                cur <<= 16;
                return result;
            } else if(ncur==16) {
                ncur = 0;
                word32 result = cur>>16;
                return result;
            } else {
                throw "not 16bit aligned";
            }
            return 0;
        }

        // Get 32 bits from the bit sequence, assuming the bit pointer 
        // is 32 bit aligned.  This is the fast case for inner loops.

        word32 getword_aligned() {
            ASSERT(nbits>=32 && ncur==0);
            nbits -= 32;
            return *p++;
        }

        word32 peekword_aligned() {
            ASSERT(nbits>=32 && ncur==0);
            return *p;
        }

        void skipword_aligned() {
            ASSERT(nbits>=32 && ncur==0);
            nbits -= 32;
            p++;
        }

        // Get 32 bits from the bit sequence; the sequence need not be aligned.
        // This is the second performance critical case for the inner loop.

        word32 getword() {
            ASSERT(nbits>=32 && ncur<32);
#if 1
            if(ncur==0) return getword_aligned();
            word32 result = cur;        // already shifted
            word32 temp = *p++;
            result |= (temp >> ncur);
            cur = (temp << (32-ncur));
            nbits -= 32;
            return result;
#else
            // slow code for testing
            word32 result = 0;
            for(int i=0;i<32;i++)
                result = (result<<1) | getbit();
            return result;
#endif
        }

        // Get multiple bits (<=32).

        word32 getbits(int n) {
            ASSERT(nbits>=n && n<=32 && n>0);
            reload();
            if(n<=ncur) {
                word32 result = (cur >> (32 - n));
                cur <<= n;
                ncur -= n;
                nbits -= n;
                return result;
            }
            word32 result = (cur >> (32 - n));
            int remaining = n-ncur;
            cur = *p++;
            ncur = 32;
            result |= getbits(remaining);
            nbits -= n;
            return result;
        }
    };

    struct BitSnk {
        word32 *p;
        int nbits;
        word32 cur;
        int ncur;

        static inline word32 rightmask(int nbits) {
            return (1<<nbits)-1;
        }

        static inline word32 leftmask(int nbits) {
            return ~rightmask(32-nbits);
        }

        // Create a bit pointer for writing. This starts skip bits into
        // the sequence of 32 bit words pointed to by p and contains exactly
        // nbits bits.

        BitSnk(word32 *p_,int nbits_,int skip=0):p(p_),nbits(nbits_) {
//#ifndef UNSAFE
            // shut up bogus GNU C++ warnings
            cur = 0;
            ncur = 0;
//#endif
            if(skip<0) throw "negative skip not allowed";
            if(skip>=nbits) {nbits = 0; return;}
            while(skip>=32) { skip-=32; nbits-=32; p++; }
            if(skip>0) {
                cur = *p;
                ncur = skip;
                cur >>= 32-skip;
                nbits -= skip;
//            } else {
//                ncur = 0;
//                cur = 0;
            }
        }
        ~BitSnk() {
            if(p) close();
        }

        // Write out any remaining bits.

        void close() {
            flush();
            if(ncur==0) return;
            *p = (cur << (32-ncur)) | (*p & rightmask(32-ncur));
            ncur = 0;
            p = 0;
        }

        // Check whether there is room to write any more bits.

        bool has_bits() {
            return nbits>0;
        }

        // Check whether there is room to write any more complete 32bit words.

        bool has_words() {
            return nbits>=32;
        }

        // Check whether the bit pointer is currently word aligned.

        bool is_wordaligned() {
            return ncur==0 || ncur==32;
        }

        // Flush a complete 32bit word if any (does nothing if the word
        // isn't complete).

        void flush() {
            if(ncur==32) {
                *p++ = cur;
                ncur = 0;
            }
        }

        // Check how many bits we can still write in the current word.

        int bits_left_in_word() {
            return ncur<nbits ? ncur : nbits;
        }

        // Put a single bit into the sequence.

        void putbit(word32 bit) {
            ASSERT(nbits>0 && unsigned(bit)<=1);
            cur = (cur<<1) | bit;
            ncur++;
            flush();
            nbits--;
        }

        void put_run(int size,int bit) {
            CHECK_ARG(bit==0 || bit==1);
            while(size>0 && !is_wordaligned()) { putbit(bit); size--; }
            while(size>=32) { 
                putword_aligned(bit?0xffffffff:0);
                size-=32; 
            }
            while(size>0) { putbit(bit); size--; }
        }

        // Put a 32bit word into the sequence, assuming the sequence is
        // aligned.  This is the fast inner loop case.

        void putword_aligned(word32 word) {
            ASSERT(nbits>=32 && ncur==0);
            nbits -= 32;
            *p++ = word;
        }

        // Put a 32bit word into the sequence, even if it's not aligned.
        // This is the second fast inner loop case.

        void putword(word32 word) {
            ASSERT(nbits>=32);
            flush();
            nbits -= 32;
            if(ncur==0) {
                *p++ = word;
                return;
            }
#if 1
            *p++ = (cur << (32-ncur)) | (word >> ncur);
            //cur = word & rightmask(ncur); // no need to clear high bits
            cur = word;
#else
            // slow code for testing
            for(int i=0;i<32;i++) {
                putbit(!!(word&0x80000000));
                word <<= 1;
            }
#endif
        }
    };
}

#endif
