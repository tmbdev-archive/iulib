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
// File: imgbits.h
// Purpose:
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de, www.ocropus.org


// -*- C++ -*-
/* Copyright (c) Thomas M. Breuel */

#ifndef imgbits_h_
#define imgbits_h_

#include "colib/narray.h"

namespace imgbits {
    using namespace colib;

    typedef unsigned int word32;
    enum { DFLTC = 32767 };

    struct BitImage {
        // FIXME consider moving to an narray< narray<word32> > eventually...
        word32 *data;
        int words_per_row;
        int dims[2];

        void init() {
            data = 0;
            words_per_row = 0;
            dims[0] = 0;
            dims[1] = 0;
        }
        void clear() {
            if(data) delete [] data;
            init();
        }
        BitImage() {
            init();
        }
        BitImage(int w,int h) {
            init();
            resize(w,h);
        }
        ~BitImage() {
            clear();
        }
        double megabytes() {
            return (dims[0] * ((dims[1]+31)/32) + 16) * 10e-6;
        }
        void copy(BitImage &other) {
            clear();
            dims[0] = other.dims[0];
            dims[1] = other.dims[1];
            words_per_row = other.words_per_row;
            int total_words = dims[0] * words_per_row;
            data = new word32[total_words];
            memcpy(data,other.data,total_words * sizeof *data);
        }
        int rank() {
            return 2;
        }
        int dim(int i) {
            if(unsigned(i)>=2) throw "rank error";
            return dims[i];
        }
        void resize(int w,int h) {
            clear();
            if(w==dims[0]&&h==dims[1]) return;
            dims[0] = w;
            dims[1] = h;
            words_per_row = int((dims[1] + 31)/32);
            int total_words = dims[0] * words_per_row;
            data = new word32[total_words];
        }
        word32 *get_line(int i) {
#ifndef UNSAFE
            if(unsigned(i)>=unsigned(dims[0])) throw "index error";
#endif
            return data + i * words_per_row;
        }
        bool at(int i,int j) {
            word32 *p = get_line(i);
#ifndef UNSAFE
            if(unsigned(j)>=unsigned(dims[1])) throw "index error";
#endif
            return !!(p[j>>5] & (1<<(31-(j&0x1f))));
        }
        bool operator()(int i,int j) {
            return at(i,j);
        }
        void set_bit(int i,int j) {
            word32 *p = get_line(i);
#ifndef UNSAFE
            if(unsigned(j)>=unsigned(dims[1])) throw "index error";
#endif
            p[j>>5] |= (1<<(31-(j&0x1f)));
        }
        void clear_bit(int i,int j) {
            word32 *p = get_line(i);
#ifndef UNSAFE
            if(unsigned(j)>=unsigned(dims[1])) throw "index error";
#endif
            p[j>>5] &= ~(1<<(31-(j&0x1f)));
        }
        void set(int i,int j,bool value) {
            if(value) set_bit(i,j); else clear_bit(i,j);
        }
        void fill(bool value) {
            int total_words = dims[0] * words_per_row;
            for(int i=0;i<total_words;i++)
                data[i] = value?~0:0;
        }
    };

    enum BlitOp {
        BLIT_SET=1,
        BLIT_SETNOT,
        BLIT_AND,
        BLIT_OR,
        BLIT_XOR,
        BLIT_ANDNOT,
        BLIT_ORNOT
    };

    enum BlitBoundary {
        BLITB_KEEP=100,
        BLITB_CLEAR,
        BLITB_SET
    };

    struct IBlit1D {
        virtual void blit1d(word32 *dest,int enddestbits,
                            word32 *mask,int endmaskbits,
                            int shift,BlitOp op) = 0;
        virtual ~IBlit1D() {}
    };

    struct IBlit2D {
        virtual void blit2d(BitImage &image,BitImage &other,int dx,int dy,
                            BlitOp op,BlitBoundary bop) = 0;
        virtual IBlit1D &blit1d() = 0;
        virtual int getBlitCount() = 0;
        virtual ~IBlit2D() {}
    };
            
    IBlit1D *make_Blit1DBitwise();
    IBlit1D *make_Blit1DWordwise();
    IBlit2D *make_Blit2D(IBlit1D *); // NB: takes ownership

    IBlit1D *make_Blit1DBitwiseC();
    IBlit1D *make_Blit1DWordwiseC();

    void bits_move(BitImage &dest,BitImage &src);
    void bits_convert(BitImage &bimage,bytearray &image);
    void bits_convert(BitImage &bimage,floatarray &image);
    void bits_convert(bytearray &image,BitImage &bimage);
    void bits_convert(floatarray &image,BitImage &bimage);
    int bits_count_rect(BitImage &image,int x0=0,int y0=0,int x1=32000,int y1=32000);
    bool bits_non_empty(BitImage &image);
    void bits_set_rect(BitImage &image,int x0=0,int y0=0,int x1=32000,int y1=32000,bool value=false);
    void bits_resample_normed(bytearray &image,BitImage &bits,int vis_scale,bool norm=true);
    void bits_resample(bytearray &image,BitImage &bits,int vis_scale);
    void bits_reduce2_and(BitImage &out,BitImage &image);
    void bits_transpose(BitImage &out,BitImage &in);
    void bits_transpose(BitImage &image);
    void bits_flip_v(BitImage &image);
    void bits_flip_h(BitImage &image);
    void bits_rotate_rect(BitImage &image,int angle);

    void bits_set(BitImage &image,BitImage &other,int dx=0,int dy=0);
    void bits_setnot(BitImage &image,BitImage &other,int dx=0,int dy=0);
    void bits_and(BitImage &image,BitImage &other,int dx=0,int dy=0);
    void bits_or(BitImage &image,BitImage &other,int dx=0,int dy=0);
    void bits_andnot(BitImage &image,BitImage &other,int dx=0,int dy=0);
    void bits_ornot(BitImage &image,BitImage &other,int dx=0,int dy=0);
    void bits_xor(BitImage &image,BitImage &other,int dx=0,int dy=0);
    void bits_invert(BitImage &image);

    void bits_skew(BitImage &image,float skew,float center=0.0,bool backwards=false);
    void bits_rotate(BitImage &image,float angle);

    void bits_erode_rect(BitImage &image,int rx,int ry);
    void bits_dilate_rect(BitImage &image,int rx,int ry);
    void bits_open_rect(BitImage &image,int rx,int ry);
    void bits_close_rect(BitImage &image,int rx,int ry);

    void bits_erode_mask(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
    void bits_dilate_mask(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
    void bits_open_mask(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
    void bits_close_mask(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
    void bits_mask_hitmiss(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
    void bits_mask_hitmiss(BitImage &image,BitImage &hit,BitImage &miss,int cx=DFLTC,int cy=DFLTC);

    void bits_circ_mask(BitImage &image,int r);
    void bits_erode_circ(BitImage &image,int r);
    void bits_dilate_circ(BitImage &image,int r);
    void bits_open_circ(BitImage &image,int r);
    void bits_close_circ(BitImage &image,int r);

    void bits_erode_rrect(BitImage &image,int w,int h,double angle);
    void bits_dilate_rrect(BitImage &image,int w,int h,double angle);
    void bits_open_rrect(BitImage &image,int w,int h,double angle);
    void bits_close_rrect(BitImage &image,int w,int h,double angle);

    void bits_erode_line(BitImage &image,int r,double angle,int rp);
    void bits_dilate_line(BitImage &image,int r,double angle,int rp);
    void bits_open_line(BitImage &image,int r,double angle,int rp);
    void bits_close_line(BitImage &image,int r,double angle,int rp);

    // mainly for benchmarking

    void bits_erode_rect_bruteforce(BitImage &image,int rx,int ry);
    void bits_dilate_rect_bruteforce(BitImage &image,int rx,int ry);
    void bits_erode_mask_bruteforce(BitImage &image,BitImage &element,int cx,int cy);
    void bits_dilate_mask_bruteforce(BitImage &image,BitImage &element,int cx,int cy);
    void bits_erode_circ_by_dt(BitImage &image,int r);
    void bits_erode_line_by_mask(BitImage &image,int r,double angle);
    void bits_dilate_line_by_mask(BitImage &image,int r,double angle);
    void bits_open_line_by_mask(BitImage &image,int r,double angle);
    void bits_close_line_by_mask(BitImage &image,int r,double angle);
    void bits_erode_line_by_rotation(BitImage &image,int r,double angle,int rp=0);
    void bits_erode_line_by_skew(BitImage &image,int r,double angle,int rp=0);
    void bits_runlength_statistics(floatarray &on,floatarray &off,BitImage &image);
    void bits_read(BitImage &image,const char *file);
    void bits_write(const char *file,BitImage &image);
    void bits_dshow(BitImage &image, const char *spec="");
    void bits_thin(BitImage &image,int neighborhood);
    void bits_parse_mask(BitImage &hit,const char *mask);
    void bits_parse_hitmiss(BitImage &hit,BitImage &miss,const char *mask);

    extern int bits_transpose_slow;
    extern int use_telescope;
    void bits_change_blit(int);
    int get_blit_count();
}

#endif
