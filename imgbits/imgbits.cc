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
// File: imgbits.cc
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
#include "imglib.h"
#include "imgio.h"
#include "imgbits.h"
#include "imgbitptr.h"
// #include "ocr-utils.h"
// #include "dgraphics.h"
#define dshow(x,y)
#define D if(0)

namespace imgbits {
    using namespace colib;
    using namespace iulib;

    ////////////////////////////////////////////////////////////////
    // conversions
    ////////////////////////////////////////////////////////////////

    // move the contents of src to dest without copying, emptying the source

    void bits_move(BitImage &dest,BitImage &src) {
        dest.clear();
        dest.words_per_row = src.words_per_row;
        dest.dims[0] = src.dims[0];
        dest.dims[1] = src.dims[1];
        dest.data = src.data;
        src.data = 0;
        src.clear();
    }

    // make a bit image from a byte image

    void bits_convert(BitImage &bimage,bytearray &image) {
        int w = image.dim(0), h = image.dim(1);
        bimage.resize(w,h);
        int i,j;
        for(i=0;i<w;i++) {
            BitSnk p(bimage.get_line(i),bimage.dim(1));
            for(j=0;j<h;j++) p.putbit(!!image(i,j));
        }
    }

    // make a bit image from a float image

    void bits_convert(BitImage &bimage,floatarray &image) {
        int w = image.dim(0), h = image.dim(1);
        bimage.resize(w,h);
        int i,j;
        for(i=0;i<w;i++) {
            BitSnk p(bimage.get_line(i),bimage.dim(1));
            for(j=0;j<h;j++) p.putbit(!!image(i,j));
        }
    }

    // convert a bit image to a byte image

    void bits_convert(bytearray &image,BitImage &bimage) {
        int w = bimage.dim(0), h = bimage.dim(1);
        image.resize(w,h);
        int i,j;
        for(i=0;i<w;i++) {
            BitSrc p(bimage.get_line(i),bimage.dim(1));
            for(j=0;j<h;j++) image(i,j) = p.getbit()?255:0;
        }
    }

    // convert a bit image to a float image

    void bits_convert(floatarray &image,BitImage &bimage) {
        int w = bimage.dim(0), h = bimage.dim(1);
        image.resize(w,h);
        int i,j;
        for(i=0;i<w;i++) {
            BitSrc p(bimage.get_line(i),bimage.dim(1));
            for(j=0;j<h;j++) image(i,j) = p.getbit()?1:0;
        }
    }

    ////////////////////////////////////////////////////////////////
    // count the number of bits in a word
    ////////////////////////////////////////////////////////////////

    static int count_bits_row(word32 *row,int from,int to) {
        using namespace bithacks;
        BitSrc bp(row,to,from);
        int total = 0;
        if(bp.bits_left_in_word()>0) {
            word32 t = bp.getbits(bp.bits_left_in_word());
            total += bitcount_table(t);
        }
        while(bp.has_words()) {
            ASSERT(bp.is_wordaligned());
            word32 t = bp.getword_aligned();
            total += bitcount_table(t);
        }
        if(bp.bits_left_in_word()>0) {
            word32 t = bp.getbits(bp.bits_left_in_word());
            total += bitcount_table(t);
        }
        return total;
    }

    int bits_count_rect(BitImage &image,int x0,int y0,int x1,int y1) {
        int total = 0;
        int i;
        if(x0<0) x0 = 0;
        if(x1>=image.dims[0]) x1 = image.dims[0];
        if(y0<0) y0 = 0;
        if(y1>=image.dims[1]) y1 = image.dims[1];
        CHECK_ARG(x1>x0 && y1>y0);
        for(i=x0;i<x1;i++) {
            total += count_bits_row(image.get_line(i),y0,y1);
        }
        return total;
    }

    bool bits_non_empty(BitImage &image) {
        for(int i=0;i<image.dim(0);i++) {
            if(count_bits_row(image.get_line(i),0,image.dim(1))>0) 
                return true;
        }
        return false;
    }

    int bits_count(BitImage &image) {
        return bits_count_rect(image,0,0,image.dim(0),image.dim(1));
    }

    int bits_difference(BitImage &image,BitImage &image2) {
        BitImage temp;
        temp.copy(image);
        bits_xor(image,image2);
        return bits_count_rect(temp,0,0,image.dim(0),image.dim(1));
    }

    ////////////////////////////////////////////////////////////////
    // clearing/setting a rectangle
    ////////////////////////////////////////////////////////////////

    namespace {
        void set_bits_row(word32 *row,int from,int to,word32 value) {
            BitSnk bp(row,to,from);
            while(bp.has_bits() && !bp.is_wordaligned()) bp.putbit(value);
            while(bp.has_words()) bp.putword_aligned(value);
            while(bp.has_bits()) bp.putbit(value);
        }
    }

    void bits_set_rect(BitImage &image,int x0,int y0,int x1,int y1,bool value) {
        int i;
        if(x0<0) x0 = 0;
        if(x1>=image.dims[0]) x1 = image.dims[0];
        if(y0<0) y0 = 0;
        if(y1>=image.dims[1]) y1 = image.dims[1];
        for(i=x0;i<x1;i++)
            set_bits_row(image.get_line(i),y0,y1,value);
    }

    ////////////////////////////////////////////////////////////////
    // runlength statistics
    ////////////////////////////////////////////////////////////////

    void bits_runlength_row(word32 *row,int nbits,floatarray &on,floatarray &off) {
        BitSrc p(row,nbits,0);
        word32 bit = 0;
        int total = 0;
        while(p.has_bits()) {
            word32 b = p.getbit();
            if(b!=bit) {
                if(bit) {
                    if(unsigned(total)<unsigned(on.length()))
                        on[total]++;
                } else {
                    if(unsigned(total)<unsigned(off.length()))
                        off[total]++;
                }
                bit = !bit;
                total = 0;
            }
            total++;
        }
    }

    void bits_runlength_statistics(floatarray &on,floatarray &off,BitImage &image) {
        fill(on,0);
        fill(off,0);
        int i;
        for(i=0;i<image.dim(0);i++)
            bits_runlength_row(image.get_line(i),image.dim(1),on,off);
    }

    ////////////////////////////////////////////////////////////////
    // convenience functions for interfacing with the blit code
    ////////////////////////////////////////////////////////////////

    autodel<IBlit2D> blit2d(make_Blit2D(make_Blit1DWordwise()));

    int get_blit_count() {
        return blit2d->getBlitCount();
    }

    void bits_change_blit(int which) {
        switch(which) {
        case 0:
            blit2d = make_Blit2D(make_Blit1DWordwise());
            break;
        case 1:
            blit2d = make_Blit2D(make_Blit1DWordwiseC());
            break;
        case 2:
            blit2d = make_Blit2D(make_Blit1DBitwise());
            break;
        case 3:
            blit2d = make_Blit2D(make_Blit1DBitwiseC());
            break;
        default:
            throw "no such blit";
        }
    }

    void bits_set(BitImage &image,BitImage &other,int dx,int dy) {
        blit2d->blit2d(image,other,dx,dy,BLIT_SET,BLITB_CLEAR);
    }

    void bits_setnot(BitImage &image,BitImage &other,int dx,int dy) {
        blit2d->blit2d(image,other,dx,dy,BLIT_SETNOT,BLITB_CLEAR);
    }

    void bits_and(BitImage &image,BitImage &other,int dx,int dy) {
        blit2d->blit2d(image,other,dx,dy,BLIT_AND,BLITB_CLEAR);
    }

    void bits_or(BitImage &image,BitImage &other,int dx,int dy) {
        blit2d->blit2d(image,other,dx,dy,BLIT_OR,BLITB_CLEAR);
    }

    void bits_andnot(BitImage &image,BitImage &other,int dx,int dy) {
        blit2d->blit2d(image,other,dx,dy,BLIT_ANDNOT,BLITB_CLEAR);
    }

    void bits_ornot(BitImage &image,BitImage &other,int dx,int dy) {
        blit2d->blit2d(image,other,dx,dy,BLIT_ORNOT,BLITB_CLEAR);
    }

    void bits_xor(BitImage &image,BitImage &other,int dx,int dy) {
        blit2d->blit2d(image,other,dx,dy,BLIT_XOR,BLITB_CLEAR);
    }

    void bits_invert(BitImage &image) {
        int vis_total_words = image.dims[0] * image.words_per_row;
        for(int i=0;i<vis_total_words;i++)
            image.data[i] = ~image.data[i];
    }

    ////////////////////////////////////////////////////////////////
    // transpose and flips
    ////////////////////////////////////////////////////////////////

    void bits_transpose(BitImage &out,BitImage &in) {
        using namespace bithacks;

        out.clear();
        out.resize(in.dim(1),in.dim(0));
        out.fill(0);

        for(int row=0;row<in.dim(0);row+=32) {
            int destword = row/32;
            for(int word=0;word<in.words_per_row;word++) {
                int col = word * 32;
                word32 wout[32],win[32];
                int n,k;
                n = in.dim(0)-row; if(n>32) n = 32;
                for(k=0;k<n;k++)
                    win[k] = in.get_line(row+k)[word];
                transpose_words8(wout,win);
                n = out.dim(0)-col; if(n>32) n = 32;
                for(k=0;k<n;k++)
                    out.get_line(col+k)[destword] = wout[k];
            }
        }
    }

    void bits_transpose(BitImage &image) {
        BitImage temp;
        bits_transpose(temp,image);
        bits_move(image,temp);
    }

    namespace {
        void memswap(word32 *p,word32 *q,int n) {
            while(n-->0) {
                word32 temp = *p;
                *p++ = *q;
                *q++ = temp;
            }
        }
    }

    void bits_flip_v(BitImage &image) {
        int i, n = image.dim(0);
        for(i=0;;i++) {
            int j = n-i-1;
            if(i>=j) break;
            memswap(image.get_line(i),image.get_line(j),image.words_per_row);
        }
    }

    // could do this in place, but probably won't have much
    // impact on real programs (how often can you flip an image
    // anyway?)

    void bits_flip_h(BitImage &image) {
        bits_transpose(image);
        bits_flip_v(image);
        bits_transpose(image);
    }

    void bits_rotate_rect(BitImage &image,int angle) {
        while(angle<0) angle += 360;
        while(angle>=360) angle -= 360;
        if(angle==0) {
            return;
        } else if(angle==90) {
            bits_flip_v(image);
            bits_transpose(image);
        } else if(angle==180) {
            bits_flip_v(image);
            bits_transpose(image);
            bits_flip_v(image);
            bits_transpose(image);
        } else if(angle==270) {
            bits_transpose(image);
            bits_flip_v(image);
        } else {
            throw "angle must be multiple of 90 degree";
        }
    }

    ////////////////////////////////////////////////////////////////
    // skewing and arbitrary rotation
    ////////////////////////////////////////////////////////////////

    void bits_skew(BitImage &image,float skew,float center,bool backwards) {
        int w = image.dim(0), h = image.dim(1);
        BitImage temp(1,h);
        float delta = center * skew;
        for(int i=0;i<w;i++) {
            int offset;
            if(backwards) offset = -int(-skew * i + delta);
            else offset = int(skew * i - delta);
            word32 *iline = image.get_line(i);
            int endilinebits = image.dim(1);
            temp.fill(0);
            word32 *tline = temp.get_line(0);
            int endtlinebits = temp.dim(1);
            blit2d->blit1d().blit1d(tline,endtlinebits,iline,endilinebits,offset,BLIT_SET);
            blit2d->blit1d().blit1d(iline,endilinebits,tline,endtlinebits,0,BLIT_SET);
        }
    }

    void bits_rotate(BitImage &image,float angle) {
        int rect = 0;
        while(angle<=M_PI/4) { rect -= 90; angle += M_PI/2; }
        while(angle>=M_PI/4) { rect += 90; angle -= M_PI/2; }
        if(rect!=0) bits_rotate_rect(image,rect);
        float xshear = -tan(angle/2.0);
        float yshear = sin(angle);
        bits_skew(image,xshear,image.dim(0)/2);
        bits_transpose(image);
        bits_skew(image,yshear,image.dim(0)/2);
        bits_transpose(image);
        bits_skew(image,xshear,image.dim(0)/2);
    }

    ////////////////////////////////////////////////////////////////
    // rectangle morphology (with decomposition)
    ////////////////////////////////////////////////////////////////

    // helper function that lets us invoke the various blit operations using an op parameter
    // and selecting the axis (this avoids having to write separate code for horizontal
    // and vertical decomposition)

    static void bits_op(BitImage &dest,BitImage &src,int r,int axis,int op,int dx=0,int dy=0) {
        if(op==0) {
            if(axis==0) {
                bits_and(dest,src,r+dx,0+dy);
            } else if(axis==1) {
                bits_and(dest,src,0+dx,r+dy);
            } else throw "oops";
        } else if(op==1) {
            if(axis==0) {
                bits_or(dest,src,r+dx,0+dy);
            } else if(axis==1) {
                bits_or(dest,src,0+dx,r+dy);
            } else throw "oops";
        } else throw "oops";
    }

    // Compose a horizontal or vertical morphological operation out of elementary operations.
    // The bottom left of the linear structuring element is at the origin (it's not centered!)
    // Use dx and dy to center it yourself.
    //
    // During each loop, we accumulate a mask of size 2^bit and use this to finish off larger
    // and larger parts of the morphological operation.
    //
    // We decide when to apply the operation by looking at the bit representation of the
    // mask size; this allows us to do the decomposition from small to large, which lets
    // us get by with only a single temporary array.
    // doesn't change bits in the image.

    void bits_rect_op_decomp(BitImage &image,int r,int axis,int op,int dx,int dy) {
        int w = image.dim(0), h = image.dim(1);
        BitImage mask;
        bits_move(mask,image);
        image.resize(w,h);
        image.fill(!op);
        int where = 0;
        int bit = 0;
        while(bit<32) {
            int width = (1<<bit);
            if(r&width) {
                bits_op(image,mask,where,axis,op,dx,dy);
                where += width;
            }
            r &= ~width;
            if(r==0) break;
            bits_op(mask,mask,width,axis,op);
            bit++;
        }
    }

    void bits_rect_op_telescope(BitImage &image,int r,int axis,int op,int dx,int dy) {
        int w = image.dim(0), h = image.dim(1);
        BitImage mask;
        bits_move(mask,image);
        image.resize(w,h);
        image.fill(!op);
        int width = 1;
        while(2*width<r) {
            bits_op(mask,mask,width,axis,op);
            width *= 2;
        }
        bits_op(image,mask,0,axis,op,dx,dy);
        if(r-width>0) bits_op(image,mask,r-width,axis,op,dx,dy);
    }

    void bits_rect_op_shifted(BitImage &image,int r,int axis,int op,int dx,int dy) {
        int width = 1;
        if(dx || dy) bits_set(image,image,dx,dy);
        while(2*width<r) {
            bits_op(image,image,width,axis,op);
            width *= 2;
        }
        if(r>width) bits_op(image,image,r-width-1,axis,op);
    }

    int use_telescope = 1;

    void bits_rect_op_line(BitImage &image,int r,int axis,int op,int dx,int dy) {
        switch(use_telescope) {
        case 1: bits_rect_op_telescope(image,r,axis,op,dx,dy); break;
        case 2: bits_rect_op_decomp(image,r,axis,op,dx,dy); break;
        case 3: bits_rect_op_shifted(image,r,axis,op,dx,dy); break;
        default: throw "unknown telescope";
        }
    }

    void bits_erode_rect(BitImage &image,int rx,int ry) {
        if(rx>1) bits_rect_op_line(image,rx,0,0,-rx/2,0);
        if(ry>1) bits_rect_op_line(image,ry,1,0,0,-ry/2);
    }

    void bits_dilate_rect(BitImage &image,int rx,int ry) {
        if(rx>1) bits_rect_op_line(image,rx,0,1,-(rx-1)/2,0);
        if(ry>1) bits_rect_op_line(image,ry,1,1,0,-(ry-1)/2);
    }

    void bits_open_rect(BitImage &image,int rx,int ry) {
        bits_erode_rect(image,rx,ry);
        bits_dilate_rect(image,rx,ry);
    }

    void bits_close_rect(BitImage &image,int rx,int ry) {
        bits_dilate_rect(image,rx,ry);
        bits_erode_rect(image,rx,ry);
    }

    void bits_erode_rect_bruteforce(BitImage &image,int rx,int ry) {
        BitImage temp;
        int i;
        if(rx>0) temp.copy(image);
        for(i=0;i<rx;i++) {
            bits_op(image,temp,i-rx/2,0,0);
        }
        if(ry>0) temp.copy(image);
        for(i=0;i<ry;i++) {
            bits_op(image,temp,i-ry/2,1,0);
        }
    }

    void bits_dilate_rect_bruteforce(BitImage &image,int rx,int ry) {
        BitImage temp;
        int i;
        if(rx>0) temp.copy(image);
        for(i=0;i<rx;i++) {
            bits_op(image,temp,i-rx/2,0,1);
        }
        if(ry>0) temp.copy(image);
        for(i=0;i<ry;i++) {
            bits_op(image,temp,i-ry/2,1,1);
        }
    }

    ////////////////////////////////////////////////////////////////
    // run-based erosions
    ////////////////////////////////////////////////////////////////

    namespace {
        int compute_element_runs(intarray &xs,intarray &starts,intarray &ends,BitImage &element) {
            int maxwidth = 0;
            xs.clear();
            starts.clear();
            ends.clear();
            for(int i=0;i<element.dim(0);i++) {
                int j=0;
                while(j<element.dim(1)) {
                    while(j<element.dim(1) && !element(i,j)) j++;
                    int start = j;
                    if(j==element.dim(1)) continue;
                    while(j<element.dim(1) && element(i,j)) j++;
                    int end = j;
                    if(start==end) continue;
                    xs.push(i);
                    starts.push(start);
                    ends.push(end);
                    if(end-start>maxwidth) maxwidth = end-start;
                }
            }
            return maxwidth;
        }
    }

    void bits_op_runs(BitImage &image,BitImage &element,int cx,int cy,int op) {
        if(cx==DFLTC) cx = element.dim(0)/2;
        if(cy==DFLTC) cy = element.dim(1)/2;
        intarray xs,start,end;
        int maxwidth = compute_element_runs(xs,start,end,element);
        int w = image.dim(0), h = image.dim(1);
        BitImage mask;
        bits_move(mask,image);
        image.resize(w,h);
        image.fill(!op);
        int width = 1;
        for(;;) {
            for(int i=0;i<start.length();i++) {
                int lwidth = end(i)-start(i);
                if(width<=lwidth && lwidth<2*width) {
                    int x = cx-xs(i);
                    int y = cy-(start(i)+width);
                    bits_op(image,mask,0,1,0,x,y+1);
                    if(end(i)-start(i)!=width) {
                        int y = cy-end(i);
                        bits_op(image,mask,0,1,0,x,y+1);
                    }
                }
            }
            if(maxwidth<2*width) break;
            bits_op(mask,mask,width,1,op);
            width *= 2;
        }
    }

    void bits_erode_mask(BitImage &image,BitImage &element,int cx,int cy) {
        bits_op_runs(image,element,cx,cy,0);
    }

    void bits_dilate_mask(BitImage &image,BitImage &element,int cx,int cy) {
        bits_op_runs(image,element,cx,cy,1);
    }

    void bits_open_mask(BitImage &image,BitImage &element,int cx,int cy) {
        bits_erode_mask(image,element,cx,cy);
        bits_dilate_mask(image,element,cx,cy);
    }

    void bits_close_mask(BitImage &image,BitImage &element,int cx,int cy) {
        bits_dilate_mask(image,element,cx,cy);
        bits_erode_mask(image,element,cx,cy);
    }

    ////////////////////////////////////////////////////////////////
    // circular morphology (relies on mask morphology functions to decompose)
    ////////////////////////////////////////////////////////////////

    void bits_circ_mask(BitImage &mask,int r) {
        mask.resize(2*r+1,2*r+1);
        mask.fill(0);
        int r2 = r*r;
        for(int i=-r;i<=r;i++) for(int j=-r;j<=r;j++) {
            if(i*i+j*j>r2) continue;
            mask.set(i+r,j+r,1);
        }
    }

    void bits_erode_circ(BitImage &image,int r) {
        BitImage mask;
        bits_circ_mask(mask,r);
        bits_erode_mask(image,mask);
    }

    void bits_dilate_circ(BitImage &image,int r) {
        BitImage mask;
        bits_circ_mask(mask,r);
        bits_dilate_mask(image,mask);
    }

    void bits_open_circ(BitImage &image,int r) {
        BitImage mask;
        bits_circ_mask(mask,r);
        bits_erode_mask(image,mask);
        bits_dilate_mask(image,mask);
    }

    void bits_close_circ(BitImage &image,int r) {
        BitImage mask;
        bits_circ_mask(mask,r);
        bits_dilate_mask(image,mask);
        bits_erode_mask(image,mask);
    }

    ////////////////////////////////////////////////////////////////
    // rotated rectangle morphology
    ////////////////////////////////////////////////////////////////

    namespace {
        void normangle0(double &a) {
            while(a<=-M_PI/4) a+=M_PI;
            while(a>=M_PI*3/4) a-=M_PI;
        }
    }

    void bits_erode_rrect(BitImage &image,int r,int rp,double angle) {
        angle -= M_PI/2;
        bits_rotate(image,-angle);
        bits_erode_rect(image,rp,int(2*r));
        bits_rotate(image,angle);
    }

    void bits_dilate_rrect(BitImage &image,int r,int rp,double angle) {
        angle -= M_PI/2;
        bits_rotate(image,-angle);
        bits_dilate_rect(image,rp,int(2*r));
        bits_rotate(image,angle);
    }

    void bits_open_rrect(BitImage &image,int r,int rp,double angle) {
        angle -= M_PI/2;
        bits_rotate(image,-angle);
        bits_open_rect(image,rp,int(2*r));
        bits_rotate(image,angle);
    }

    void bits_close_rrect(BitImage &image,int r,int rp,double angle) {
        angle -= M_PI/2;
        bits_rotate(image,-angle);
        bits_close_rect(image,rp,int(2*r));
        bits_rotate(image,angle);
    }

    ////////////////////////////////////////////////////////////////
    // skewed line morphology (optionally skewed rectangles, 
    // really a faster approximation to rotated rectangles)
    ////////////////////////////////////////////////////////////////

    static void skew_transform(BitImage &image,double angle,bool finish) {
        normangle0(angle);
        CHECK_ARG(angle>=-M_PI/4 && angle<=3*M_PI/4);
        bool transpose_back = false;
        if(angle>M_PI/4) { 
            bits_rotate_rect(image,270); 
            transpose_back =true; 
            angle -= M_PI/2; 
        }
        double skew = tan(angle);
        if(finish==false) {
            bits_skew(image,skew,image.dim(1)/2);
        } else {
            bits_skew(image,-skew,image.dim(1)/2,true);
            if(transpose_back) bits_rotate_rect(image,90);
        }
    }

    void bits_erode_line(BitImage &image,int r,double angle,int rp) {
        skew_transform(image,angle,false);
        bits_erode_rect(image,int(2*r*cos(angle)),rp);
        skew_transform(image,angle,true);
    }

    void bits_dilate_line(BitImage &image,int r,double angle,int rp) {
        skew_transform(image,angle,false);
        bits_dilate_rect(image,int(2*r*cos(angle)),rp);
        skew_transform(image,angle,true);
    }

    void bits_open_line(BitImage &image,int r,double angle,int rp) {
        skew_transform(image,angle,false);
        bits_open_rect(image,int(2*r*cos(angle)),rp);
        skew_transform(image,angle,true);
    }

    void bits_close_line(BitImage &image,int r,double angle,int rp) {
        skew_transform(image,angle,false);
        bits_close_rect(image,int(2*r*cos(angle)),rp);
        skew_transform(image,angle,true);
    }

    ////////////////////////////////////////////////////////////////
    // I/O (eventually do something more efficient than reading
    // via byte images)
    ////////////////////////////////////////////////////////////////

    void bits_read(BitImage &image,const char *file) {
        bytearray temp;
        read_image_gray(temp,file);
        // binarize_by_range(temp);
        bits_convert(image,temp);
    }

    void bits_write(const char *file,BitImage &image) {
        bytearray temp;
        bits_convert(temp,image);
        write_png(stdio(file,"w"),temp);
    }

    void bits_dshow(BitImage &image, const char *spec) {
        bytearray temp;
        bits_convert(temp,image);
        dshow(temp,spec);
    }


    ////////////////////////////////////////////////////////////////
    // simple implementations for verification/debugging
    ////////////////////////////////////////////////////////////////

    void bits_erode_circ_by_dt(BitImage &image,int r) {
        floatarray temp;
        bits_convert(temp,image);
        erode_2(temp,r);
        bits_convert(image,temp);
    }

    void bits_dilate_circ_by_dt(BitImage &image,int r) {
        floatarray temp;
        bits_convert(temp,image);
        dilate_2(temp,r);
        bits_convert(image,temp);
    }

    void bits_erode_mask_bruteforce(BitImage &image,BitImage &element,int cx,int cy) {
        if(cx==DFLTC) cx = element.dim(0)/2;
        if(cy==DFLTC) cy = element.dim(1)/2;
        int i,j,w=element.dim(0),h=element.dim(1);
        BitImage temp;
        temp.copy(image);
        int count = 0;
        for(i=0;i<w;i++) for(j=0;j<h;j++) {
            if(!element(i,j)) continue;
            if(count==0) bits_set(image,temp,-i+cx,-j+cy);
            else bits_and(image,temp,-i+cx,-j+cy);
            count++;
        }
    }

    void bits_dilate_mask_bruteforce(BitImage &image,BitImage &element,int cx,int cy) {
        if(cx==DFLTC) cx = element.dim(0)/2;
        if(cy==DFLTC) cy = element.dim(1)/2;
        int i,j,w=element.dim(0),h=element.dim(1);
        BitImage temp;
        temp.copy(image);
        int count = 0;
        for(i=0;i<w;i++) for(j=0;j<h;j++) {
            if(!element(i,j)) continue;
            if(count==0) bits_set(image,temp,-i+cx,-j+cy);
            else bits_or(image,temp,-i+cx,-j+cy);
            count++;
        }
    }

    void bits_mask_hitmiss(BitImage &image,BitImage &hit,BitImage &miss,int cx,int cy) {
        CHECK_ARG(hit.dim(0)==miss.dim(0) && hit.dim(1)==miss.dim(1));
        int w=hit.dim(0),h=hit.dim(1);
        if(cx==DFLTC) cx = w/2;
        if(cy==DFLTC) cy = h/2;
        BitImage temp;
        temp.copy(image);
        int count = 0;
        for(int i=0;i<w;i++) for(int j=0;j<h;j++) {
            if(hit(i,j)) {
                if(count==0) bits_set(image,temp,-i+cx,-j+cy);
                else bits_and(image,temp,-i+cx,-j+cy);
                count++;
            }
            if(miss(i,j)) {
                if(count==0) bits_setnot(image,temp,-i+cx,-j+cy);
                else bits_andnot(image,temp,-i+cx,-j+cy);
                count++;
            }
        }
    }

    void bits_mask_hitmiss(BitImage &image,BitImage &element,int cx,int cy) {
        int i,j,w=element.dim(0)/2,h=element.dim(1);
        if(cx==DFLTC) cx = w/2;
        if(cy==DFLTC) cy = h/2;
        BitImage temp;
        temp.copy(image);
        int count = 0;
        for(i=0;i<w;i++) for(j=0;j<h;j++) {
            if(element(i,j)) {
                if(count==0) bits_set(image,temp,-i+cx,-j+cy);
                else bits_and(image,temp,-i+cx,-j+cy);
                count++;
            }
            if(element(i+w,j)) {
                if(count==0) bits_setnot(image,temp,-i+cx,-j+cy);
                else bits_andnot(image,temp,-i+cx,-j+cy);
                count++;
            }
        }
    }

    void bits_line_mask(BitImage &mask,int r,double angle) {
        mask.resize(2*r+1,2*r+1);
        mask.fill(0);
        double dx = cos(angle);
        double dy = sin(angle);
        for(float l=-r;l<=r;l+=0.3) {
            int x = int(l*dx)+r;
            int y = int(l*dy)+r;
            if(x<0||x>2*r||y<0||y>2*r) continue;
            mask.set(x,y,1);
        }
    }

    void bits_erode_line_by_mask(BitImage &image,int r,double angle) {
        BitImage mask;
        bits_line_mask(mask,r,angle);
        bits_erode_mask(image,mask,r,r);
    }

    void bits_dilate_line_by_mask(BitImage &image,int r,double angle) {
        BitImage mask;
        bits_line_mask(mask,r,angle);
        bits_dilate_mask(image,mask,r,r);
    }

    void bits_open_line_by_mask(BitImage &image,int r,double angle) {
        BitImage mask;
        bits_line_mask(mask,r,angle);
        bits_erode_mask(image,mask,r,r);
        bits_dilate_mask(image,mask,r,r);
    }

    void bits_close_line_by_mask(BitImage &image,int r,double angle) {
        BitImage mask;
        bits_line_mask(mask,r,angle);
        bits_dilate_mask(image,mask,r,r);
        bits_erode_mask(image,mask,r,r);
    }
}
