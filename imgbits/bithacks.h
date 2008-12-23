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
// File: bithacks.h
// Purpose:
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de, www.ocropus.org

#ifndef h_bithacks_
#define h_bithacks_

////////////////////////////////////////////////////////////////
// These are bit hacks that seem to be coming up a lot; I've collected
// them here.  
//
// Some of them come from the following URL:
// http://graphics.stanford.edu/~seander/bithacks.html (please check there)
//
////////////////////////////////////////////////////////////////

namespace bithacks {
    typedef unsigned int word32;

    inline int bitcount_table(word32 w) {
        static unsigned char counts[256];
        if(counts[1]==0) {
            counts[0] = 0;
            for(int i=1;i<256;i++)
                counts[i] = (i&1) + counts[i/2];
        }
        unsigned char *p = (unsigned char *)&w;
        return counts[p[0]] + counts[p[1]] + counts[p[2]] + counts[p[3]];
    }

    inline int bitcount_shift(word32 v) {
        v = v - ((v >> 1) & 0x55555555);
        v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
        // word32 c = ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24;
        word32 c = ((v + ((v >> 4) & 0xF0F0F0F)) * 0x1010101) >> 24;
        return c;
    }

    inline int bitcount_ones(word32 v) {
        word32 c;
        for(c=0;v;c++) v &= v - 1;
        return c;
    }

    inline word32 bitreverse(word32 v) {
        v = ((v >> 1) & 0x55555555) | ((v & 0x55555555) << 1);
        v = ((v >> 2) & 0x33333333) | ((v & 0x33333333) << 2);
        v = ((v >> 4) & 0x0F0F0F0F) | ((v & 0x0F0F0F0F) << 4);
        v = ((v >> 8) & 0x00FF00FF) | ((v & 0x00FF00FF) << 8);
        v = ( v >> 16             ) | ( v               << 16);
        return v;
    }

    inline void transpose_words32(word32 out[32],word32 in[32]) {
        int i,j;
        word32 bit;
        for(j=0;j<32;j++) out[j] = 0;
        for(i=0,bit=0x80000000;i<32;i++,bit>>=1) {
            word32 w = in[i];
            for(int j=0;j<32;j++) {
                if(w&0x80000000) out[j] |= bit;
                w <<= 1;
            }
        }
    }

    inline void transpose_bytes(word32 out[8],int out_offset,word32 in[8],int in_offset) {
        int i;
        word32 bit;
        for(i=0,bit=(0x80000000>>out_offset);i<8;i++,bit>>=1) {
            word32 w = (in[i]>>(24-in_offset));
            if(w&0x80) out[0] |= bit;
            if(w&0x40) out[1] |= bit;
            if(w&0x20) out[2] |= bit;
            if(w&0x10) out[3] |= bit;
            if(w&0x08) out[4] |= bit;
            if(w&0x04) out[5] |= bit;
            if(w&0x02) out[6] |= bit;
            if(w&0x01) out[7] |= bit;
        }
    }

    inline void transpose_words8(word32 out[32],word32 in[32]) {
        for(int i=0;i<32;i++) out[i] = 0;
        transpose_bytes(out+0,0,in+0,0);
        transpose_bytes(out+0,8,in+8,0);
        transpose_bytes(out+0,16,in+16,0);
        transpose_bytes(out+0,24,in+24,0);
        transpose_bytes(out+8,0,in+0,8);
        transpose_bytes(out+8,8,in+8,8);
        transpose_bytes(out+8,16,in+16,8);
        transpose_bytes(out+8,24,in+24,8);
        transpose_bytes(out+16,0,in+0,16);
        transpose_bytes(out+16,8,in+8,16);
        transpose_bytes(out+16,16,in+16,16);
        transpose_bytes(out+16,24,in+24,16);
        transpose_bytes(out+24,0,in+0,24);
        transpose_bytes(out+24,8,in+8,24);
        transpose_bytes(out+24,16,in+16,24);
        transpose_bytes(out+24,24,in+24,24);
    }
}

#endif
