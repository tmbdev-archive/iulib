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
// File: imgrle.h
// Purpose:
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de, www.ocropus.org


// -*- C++ -*-

#ifndef imgrle_h_
#define imgrle_h_

#include "colib/checks.h"
#include "colib/narray.h"
#include "imgbits.h"
#include "colib/narray-util.h"

namespace imgrle {
    using namespace imgbits;

    extern int run_density_threshold;

    enum Relation {
        NONE, 
        PRECEDES, 
        PRECEDES_I, 
        MEETS, 
        MEETS_I,
        OVERLAPS, 
        OVERLAPS_I, 
        STARTS, 
        STARTS_I,
        DURING, 
        DURING_I,
        FINISHES, 
        FINISHES_I,
        EQUALS
    };

    struct RLERun {
        short start,end;
        RLERun():start(-1),end(-1) {
        }
        RLERun(short start,short end):start(start),end(end) {
        }
        RLERun operator+(int offset) {
            return RLERun(start+offset,end+offset);
        }
        void operator+=(int offset) {
            start+=offset;
            end+=offset;
        }
        bool contains(const RLERun other) {
            return start<=other.start && end>=other.end;
        }
        bool overlaps(const RLERun &other) {
            return min(end,other.end)>=max(start,other.start);
        }
        bool before(const RLERun &other) {
            return end<other.start;
        }
        bool after(const RLERun &other) {
            return start>other.end;
        }
    };

    typedef narray<RLERun> RLELine;

    // Verify that an RLE line is in canonical format.
    //
    // If a right limitis given, also verify that no run extends
    // beyond that.
    
    inline void verify_line(RLELine &line,int l=(1<<30)) {
#ifndef UNSAFE
        if(line.length()==0) return;
        CHECK_CONDITION(line(0).start>=0);
        for(int j=1;j<line.length();j++) {
            CHECK_CONDITION(line(j).start<line(j).end);
            CHECK_CONDITION(line(j).start>line(j-1).end);
        }
        CHECK_CONDITION(line(line.length()-1).end<=l);
#endif
    }

    // Trim an RLE line back to the range [0,l); this only examines
    // and modifies the first and last run.
    
    inline void trim_line(RLELine &line,int l) {
        int n = line.length();
        if(n==0) return;
        n--;
        if(line(0).start<0) {
            line(0).start = 0;
            ASSERT(n==0||line(1).start>0);
        }
        if(line(n).end>l) {
            line(n).end = l;
            ASSERT(n==0||line(n-1).end<l);
        }
    }

    // A run-length-encoded image.

    struct RLEImage {
    private:
        narray<RLELine> lines;
        int dims[2];
    public:
        bool doesNotAlias(RLEImage &other) {
            return &lines != &other.lines;
        }
        int dim(int d) {
            return dims[d];
        }
        RLELine &line(int i) {
            return lines(i);
        }
        int nlines() {
            return lines.length();
        }

        void take(RLEImage &in) {
            RLEImage &out = *this;
            move(out.lines,in.lines);
            out.dims[0] = in.dims[0];
            out.dims[1] = in.dims[1];
            in.dims[0] = 0;
        }

        bool equals(RLEImage &b) {
            RLEImage &a = *this;
            if(a.dim(0)!=b.dim(0)) return 0;
            if(a.dim(1)!=b.dim(1)) return 0;
            for(int i=0;i<a.dim(0);i++) {
                if(a.lines(i).length()!=b.lines(i).length()) return 0;
                for(int j=0;j<a.lines(i).length();j++) {
                    if(a.lines(i)(j).start != b.lines(i)(j).start) return 0;
                    if(a.lines(i)(j).end != b.lines(i)(j).end) return 0;
                }
            }
            return 1;
        }
    
        int number_of_runs() {
            int total = 0;
            for(int i=0;i<lines.length();i++) total += lines(i).length();
            return total;
        }
        double megabytes() {
            return (number_of_runs() * 4 + lines.length() * 4 + 16) * 10e-6;
        }
        void resize(int d0,int d1,int prealloc=0) {
            dims[0] = d0;
            dims[1] = d1;
            lines.resize(d0);
            for(int i=0;i<d0;i++) {
                lines(i).clear();
                if(prealloc>0) lines(i).reserve(prealloc);
            }
        }
        void fill(bool value) {
            resize(dim(0),dim(1));
            if(value) {
                for(int i=0;i<dim(0);i++)
                    lines(i).push(RLERun(0,dim(1)));
            }
        }
        void copy(RLEImage &other) {
            dims[0] = other.dims[0];
            dims[1] = other.dims[1];
            lines.resize(other.lines.dim(0));
            for(int i=0;i<lines.length();i++) {
                lines(i).clear();
                RLELine &line = lines(i);
                RLELine &oline = other.lines(i);
                for(int j=0;j<oline.length();j++)
                    line.push(oline(j));
            }
        }
        int at(int x,int y) {
            RLELine &line = lines(x);
            // mainly for debugging, but might want to use binary search eventually
            for(int j=0;j<line.length();j++) {
                RLERun r = line(j); 
                if(y>=r.start && y<r.end) return 1;
            }
            return 0;
        }
        void put(int x,int y,bool p);
        void verify() {
#ifndef UNSAFE
            for(int i=0;i<lines.length();i++) {
                verify_line(lines(i),dim(1));
            }
#endif
        }
    };

    // various image processing functions

    void rle_move(RLEImage &out,RLEImage &in);
    void rle_convert(RLEImage &out,bytearray &in);
    void rle_convert(bytearray &out,RLEImage &in);
    void rle_convert(RLEImage &out,BitImage &in);
    void rle_convert(BitImage &out,RLEImage &in);

    int rle_count_bits(RLEImage &image);
    int rle_count_bits(RLEImage &image,int x0,int y0,int x1,int y1);
    void rle_runlength_statistics(floatarray &on,floatarray &off,RLEImage &image);
    void rle_peak_estimation(intarray &h0,intarray &h1,intarray &v0,intarray &v1,RLEImage &image,float sh=3.0,float sv=3.0);

    void rle_shift(RLEImage &image,int d0,int d1);
    void rle_transpose(RLEImage &out,RLEImage &in);
    void rle_transpose(RLEImage &image);
    void rle_rotate_rect(RLEImage &image,int angle);
    void rle_skew(RLEImage &image,float skew,float center);
    void rle_rotate(RLEImage &image,float angle);
    void rle_flip_v(RLEImage &image);


    void rle_invert(RLEImage &image);
    void rle_and(RLEImage &image,RLEImage &mask,int d0,int d1);
    void rle_or(RLEImage &image,RLEImage &mask,int d0,int d1);

    void rle_dilate_rect(RLEImage &image,int r0,int r1);
    void rle_erode_rect(RLEImage &image,int r0,int r1);
    void rle_open_rect(RLEImage &image,int r0,int r1);
    void rle_close_rect(RLEImage &image,int r0,int r1);

    void rle_circular_mask(RLEImage &image,int r);
    void rle_erode_mask(RLEImage &image,RLEImage &mask,int r0,int r1);

    int rle_bounding_boxes(narray<rectangle> &boxes,RLEImage &image);
    // int rle_render(RLEImage &image,narray<rectangle> &boxes);
    // int rle_select(RLEImage &image,RLEImage &markers);

    void rle_read(RLEImage &,const char *);
    void rle_write(const char *,RLEImage &);
    void rle_dshow(RLEImage &image,const char *spec);
    void rle_debug(RLEImage &image);

    // The following functions are for testing/benchmarking; don't use them.

    void rle_dilate_rect_runlength(RLEImage &image,int r0,int r1);
    void rle_erode_rect_runlength(RLEImage &image,int r0,int r1);
    void rle_dilate_rect_decomp(RLEImage &image,int r0,int r1);
    void rle_erode_rect_decomp(RLEImage &image,int r0,int r1);
    void rle_erode_rect_bruteforce(RLEImage &image,int r0,int r1);
    void rle_dilate_rect_bruteforce(RLEImage &image,int r0,int r1);
    void rle_transpose_bruteforce(RLEImage &out,RLEImage &in);
    void rle_transpose_table(RLEImage &out,RLEImage &in);
    void rle_transpose_runs(RLEImage &out,RLEImage &in);
    void rle_dilate_runs(RLEImage &image,int r);
    void rle_erode_runs(RLEImage &image,int r);

    void rle_peak_estimation(intarray &h0,intarray &h1,intarray &v0,intarray &v1,RLEImage &image,float sh,float sv);
}

#endif
