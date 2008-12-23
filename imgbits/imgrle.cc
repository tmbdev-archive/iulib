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
// File: imgrle.cc
// Purpose:
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de, www.ocropus.org


/* Copyright (c) Thomas M. Breuel */

#include <stdlib.h>
#include <map>
#include "colib/narray.h"
#include "colib/narray-util.h"
#include "colib/colib.h"
#include "coords.h"
#include "imgio.h"
#include "io_png.h"
#include "imgbitptr.h"
#include "imgbits.h"
#include "imgrle.h"
#include "imgmisc.h"
//#include "ocrcomponents.h"
//#include "dgraphics.h"
#define dshow(x,y)
#define D if(0)

using namespace std;
using namespace colib;
using namespace iulib;
using namespace imgrle;


namespace {
    
    // simple union-find data structure, used for connected component
    // labeling below

    struct UnionFind {
        narray<int> p,rank;
        UnionFind(int max=10000) {
            p.resize(max);
            fill(p,-1);
            rank.resize(max);
            fill(rank,-1);
        }
        void make_set(int x) {
            if(x<0)
                throw "range error (UnionFind::make_set)";
            p[x] = x;
            rank[x] = 0;
        }
        void make_union(int x,int y) {
            if(x==y) return;
            link(find_set(x),find_set(y));
        }
        void link(int x,int y) {
            if(rank[x]>rank[y]) {
                p[y] = x;
            } else {
                p[x] = y;
                if(rank[x]==rank[y]) rank[y]++;
            }
        }
        int find_set(int x) {
            if(x<0)
                throw "range error (UnionFind::find_set)";
            if(p[x]<0)
                throw "trying to find a set that hasn't been created yet";
            if(x!=p[x]) p[x] = find_set(p[x]);
            return p[x];
        }
    };

    // label renumbering used for connected component labeling

    int renumber_labels(objlist<intarray> &labels,int start=1) {
        map<int,int> translation;
        int n = start;
        for(int i=0;i<labels.length();i++) for(int j=0;j<labels(i).length();j++) {
            int pixel = labels(i)(j);
            if(pixel==0 || pixel==0xffffff) continue;
            map<int,int>::iterator p = translation.find(pixel);
            if(p==translation.end())
                translation.insert(pair<int,int>(pixel,n++));
        }
        n = start;
        // why is this here?
        for(map<int,int>::iterator p = translation.begin();p!=translation.end();p++) {
            p->second = n++;
        }
        for(int i=0;i<labels.length();i++) for(int j=0;j<labels(i).length();j++) {
            int pixel = labels(i)(j);
            if(pixel==0 || pixel==0xffffff) continue;
            labels(i)(j) = translation[pixel]; 
        }
        return n;
    }

    // A data structure that lets us walk through the runs of a line and modify
    // them--it both is a source of runs and a sink for runs.
    // 
    // The sink must not get ahead of the source.
    //
    // The sink will combine overlapping rectangles in the right way;
    // however, you cannot go back before the start of the last run
    // you added.

    struct LineSourceSink {
        // a source of runs from a line
        RLELine &line;
        int offset;
        int index;
        int write_index;
        int last;
        LineSourceSink(RLELine &line,int offset=0):line(line),offset(offset) {
            index = 0;
            write_index = 0;
            last = 0;
            while(index<line.length() && line(index).end+offset<0) index++;
            ASSERT(index<=line.length());
        }
        ~LineSourceSink() {
            ASSERT(write_index==0);
        }
        bool done() {
            return index>=line.length();
        }
        inline RLERun next() {
            ASSERT(index>=write_index);
            ASSERT(index<=line.length());
            return line(index++) + offset;
        }
        inline RLERun tos() {
            ASSERT(index<=line.length());
            return line(index) + offset;
        }
        inline void add(short start,short end) {
            ASSERT(write_index<index);
            if(start<0) start = 0;
            if(start>=end) return;
            if(write_index>0) {
                RLERun &tos = line(write_index-1);
                ASSERT(tos.start<=start);
                if(start<=last) {
                    last = end>last?end:last; // max(end,tos.end);
                    tos.end = last;
                } else {
                    line(write_index++) = RLERun(start,end);
                    last = end;
                }
            } else {
                line(write_index++) = RLERun(start,end);
                last = end;
            }
        }
        inline void finish() {
            ASSERT(write_index<=line.length());
            line.setdims_(write_index);
            write_index = 0;
        }
    };

    // A source of transitions between runs.  This will return successive
    // starting and finishing coordinates of runs, along with whether
    // a black run or a white run starts.

    struct TransitionSource {
        RLELine &line;
        int index;
        int offset;
        TransitionSource(RLELine &line,int offset=0) : line(line),offset(offset) {
            index = 0;
        }
        operator bool() {
            return index/2 < line.length();
        }
        int coord() {
            if(index/2>=line.length()) return 32767;
            if(index%2) return line(index/2).end+offset;
            else return line(index/2).start+offset;
        }
        bool value() {
            return 1-index%2;
        }
        void next() {
            index++;
        }
    };

    // A sink for transitions between runs.  You can add coordinates monotonically,
    // together with an indicator whether a black or a white run starts.  The
    // TransitionSink will assemble these into runs.  Coordinates outside
    // [0,d] get trimmed back.

    struct TransitionSink {
        RLELine &line;
        int d;
        TransitionSink(RLELine &line,int d) : line(line),d(d) {
            ASSERT(d>0);
            line.clear();
        }
        ~TransitionSink() {
            ASSERT(d<0);
            //ASSERT(("forgot to call finish()? ",d<0));
        }
        void append(int x,int bit) {
            if(x<0) x = 0;
            else if(x>=d) x = d;
            if(line.length()>0 && line.last().end==-1) {
                ASSERT(x>=line.last().start);
                if(!bit) {
                    if(line.last().start==x) line.pop();
                    else line.last().end = x;
                }
            } else {
                ASSERT(line.length()==0 || x>=line.last().end);
                if(bit) {
                    if(line.length()==0 || x>line.last().end) {
                        line.push(RLERun(x,-1));
                    } else {
                        line.last().end = -1;
                    }
                }
            }
            ASSERT(line.length()==0 || line.last().end<=d);
        }
        void finish() {
            if(line.length()>0 && line.last().end==-1)
                line.last().end = d;
            ASSERT(line.length()==0 || line.last().end<=d);
            d = -1;
        }
    };
}

namespace imgrle {

    ////////////////////////////////////////////////////////////////
    // putting bits, etc.
    ////////////////////////////////////////////////////////////////

    // put a single bit into the image

    void RLEImage::put(int x,int y,bool bit) {
        CHECK_ARG(y>=0 && y<dim(1));
        RLELine &line = this->line(x);
        if(line.length()==0) {
            if(bit) line.push(RLERun(y,y+1));
            return;
        }
        for(int j=0;j<line.length();j++) {
            RLERun &r = line(j);
            if(y<r.start-1) {
                ASSERT(j<1 || y>line(j-1).end);
                if(bit) {
                    insert_at(line,j);
                    line(j) = RLERun(y,y+1);
                }
                return;
            } else if(y==r.start-1) {
                ASSERT(j<1 || y>line(j-1).end);
                if(bit) r.start--;
                return;
            } else if(y<r.end) {
                if(!bit) {
                    // inside a segment, split it
                    insert_at(line,j);
                    line(j).end = y;
                    line(j+1).start = y+1;
                }
                return;
            } else if(y==r.end) {
                if(j<line.length()-1) {
                    if(y<line(j+1).start-1) {
                        if(bit) r.end++;
                        return;
                    } else {
                        // between two segments, merge them
                        line(j).end = line(j+1).end;
                        delete_at(line,j+1);
                        return;
                    } 
                } else {
                    if(bit) r.end++;
                    return;
                }
            }
        }
        ASSERT(line.length()==0 || y>line(line.length()-1).end);
        if(bit) line.push(RLERun(y,y+1));
    }

    ////////////////////////////////////////////////////////////////
    // flips, transpositions, and rotations
    ////////////////////////////////////////////////////////////////

    // Flip vertically.

    void rle_flip_v(RLEImage &image) {
        int i, n = image.dim(0);
        for(i=0;;i++) {
            int j = n-i-1;
            if(i>=j) break;
            swap(image.line(i),image.line(j));
        }
    }

    // Transposing using a per-pixel table.

    void rle_transpose_table(RLEImage &out,RLEImage &in) {
        in.verify();
        intarray indexes(in.dim(1));
        intarray starts(in.dim(1));
        fill(starts,-1);
        out.resize(in.dim(1),in.dim(0));
        for(int i=0;i<in.dim(0);i++) {
            RLELine &line = in.line(i);
            int k=0;
            for(int j=0;j<line.length();j++) {
                // finish off runs that don't continue
                RLERun &run = line(j);
                int start = run.start;
                int end = run.end;
                for(;k<start;k++) {
                    if(starts(k)>=0)
                        out.line(k).push(RLERun(starts(k),i));
                    starts(k) = -1;
                }
                // start any new runs that have appeared
                end = min(end,starts.length());
                for(;k<end;k++) {
                    if(starts(k)<0) starts(k) = i;
                }
            }
            for(;k<starts.length();k++) {
                if(starts(k)>=0)
                    out.line(k).push(RLERun(starts(k),i));
                starts(k) = -1;
            }
        }
        // finish off remaining runs
        int width = in.dim(0);
        for(int k=0;k<starts.length();k++) {
            if(starts(k)>=0)
                out.line(k).push(RLERun(starts(k),width));
        }
        out.verify();
    }

    void rle_transpose(RLEImage &image,RLEImage &input) {
        rle_transpose_runs(image,input);
    }

    void rle_transpose(RLEImage &image) {
        RLEImage temp;
        rle_transpose(temp,image);
        image.take(temp);
    }

    // Rotate by multiples of 90 degrees.

    void rle_rotate_rect(RLEImage &image,int angle) {
        while(angle<0) angle += 360;
        while(angle>=360) angle -= 360;
        if(angle==0) {
            return;
        } else if(angle==90) {
            rle_flip_v(image);
            rle_transpose(image);
        } else if(angle==180) {
            rle_flip_v(image);
            rle_transpose(image);
            rle_flip_v(image);
            rle_transpose(image);
        } else if(angle==270) {
            rle_transpose(image);
            rle_flip_v(image);
        } else {
            throw "angle must be multiple of 90 degrees";
        }
    }

    ////////////////////////////////////////////////////////////////
    // skewing and rotation
    ////////////////////////////////////////////////////////////////

    // Skew by an arbitrary amount.

    void rle_skew(RLEImage &image,float skew,float center) {
        int w = image.dim(0);
        //int h = image.dim(1);
        float delta = center * skew;
        for(int i=0;i<w;i++) {
            LineSourceSink p(image.line(i));
            int offset = int(skew * i - delta);
            while(!p.done()) {
                RLERun r = p.next();
                p.add(r.start+offset,r.end+offset);
            }
        }
    }

    // Rotate by an arbitrary angle.
    // 
    // TODO make this pixel-accurate some day, like the bitmap rotation.

    void rle_rotate(RLEImage &image,float angle) {
        // angle = -angle;
        if(angle<=-M_PI/4 || angle>=M_PI/4)
            throw "rotation angle out of range";
        float xshear = -tan(angle/2.0);
        float yshear = sin(angle);
        rle_skew(image,xshear,image.dim(0)/2);
        rle_transpose(image);
        rle_skew(image,yshear,image.dim(0)/2);
        rle_transpose(image);
        rle_skew(image,xshear,image.dim(0)/2);
    }

    ////////////////////////////////////////////////////////////////
    // conversions
    ////////////////////////////////////////////////////////////////

    // Convert from bytearray to runlength.

    void rle_convert(RLEImage &out,bytearray &in) {
        int w = in.dim(0), h = in.dim(1);
        out.resize(w,h);
        for(int i=0;i<w;i++) {
            RLELine &line = out.line(i);
            line.clear();
            int j = 0;
            while(j<h) {
                while(j<h && !in(i,j)) j++;
                int start = j;
                while(j<h && in(i,j)) j++;
                if(start<j) line.push(RLERun(start,j));
            }
            verify_line(line);
        }
    }

    // Convert from runlength to bytearray.

    void rle_convert(bytearray &out,RLEImage &in) {
        in.verify();
        int w = in.dim(0), h = in.dim(1);
        out.resize(w,h);
        for(int i=0;i<w;i++) {
            for(int j=0;j<h;j++) out(i,j) = 0;
            RLELine &line = in.line(i);
            for(int k=0;k<line.length();k++) {
                RLERun r = line(k);
                for(int j=r.start;j<r.end;j++)
                    out(i,j) = 255;
            }
        }
    }

    // packed bits to/from RLE conversions

    void rle_convert(RLEImage &out,BitImage &in) {
        out.resize(in.dim(0),in.dim(1));
        for(int i=0;i<in.dim(0);i++) {
            BitSrc s(in.get_line(i),in.dim(1));
            RLELine &line = out.line(i);
            line.clear();
            int offset = 0;
            int start,end;
            while(s.get_run(start,end)) {
                ASSERT(line.length()<in.dim(1)); // prevent runaway
                ASSERT(end>=start);
                ASSERT(end>0);
                line.push(RLERun(start+offset,end+offset));
                offset += end;
            }
        }
    }

    void rle_convert(BitImage &out,RLEImage &in) {
        out.resize(in.dim(0),in.dim(1));
        for(int i=0;i<in.dim(0);i++) {
            RLELine &line = in.line(i);
            BitSnk s(out.get_line(i),out.dim(1));
            int last = 0;
            for(int j=0;j<line.length();j++) {
                RLERun run = line(j);
                if(run.end>in.dim(1)) run.end = in.dim(1);
                s.put_run(run.start-last,0);
                s.put_run(run.end-run.start,1);
                last = run.end;
            }
            s.put_run(in.dim(1)-last,0);
        }
    }

    ////////////////////////////////////////////////////////////////
    // counting
    ////////////////////////////////////////////////////////////////

    // Count the number of bits in an image.

    int rle_count_bits(RLEImage &image) {
        image.verify();
        int w = image.dim(0), h = image.dim(1);
        int total = 0;
        for(int i=0;i<w;i++) {
            RLELine &line = image.line(i);
            for(int j=0;j<line.length();j++) {
                RLERun r = line(j);
                if(r.start<0) r.start = 0;
                if(r.end>h) r.end = h;
                int delta = r.end - r.start;
                ASSERT(delta>=0);
                total += delta;
            }
        }
        return total;
    }

    // Count the number of bits inside a rectangle.

    int rle_count_bits(RLEImage &image,int x0,int x1,int y0,int y1) {
        CHECK_ARG(x0>=0 && y0>=0 && x1>x0 && y1>y0 && x1<=image.dim(0) && y1<=image.dim(1));
        image.verify();
        int total = 0;
        for(int i=x0;i<x1;i++) {
            RLELine &line = image.line(i);
            for(int j=0;j<line.length();j++) {
                RLERun r = line(j);
                if(r.start<y0) r.start=y0;
                if(r.end>y1) r.end = y1;
                total += r.end - r.start;
            }
        }
        return total;
        image.verify();
    }

    ////////////////////////////////////////////////////////////////
    // Helper functions for "run length blitting".
    ////////////////////////////////////////////////////////////////

    namespace {
        void line_invert(RLELine &out,int n) {
            int last = 0;
            for(int i=0;i<out.length();i++) {
                RLERun &r = out[i];
                int temp = r.end;
                r.end = r.start;
                r.start = last;
                last = temp;
            }
            if(last<n) out.push(RLERun(last,n));
            if(out.length()>0 && out(0).end==0) delete_at(out,0);
            verify_line(out);
        }

        void line_and(RLELine &out,RLELine &l1,RLELine &l2,int offset2,int total) {
            TransitionSink sink(out,total);
            TransitionSource src1(l1,0);
            TransitionSource src2(l2,offset2);
            int where = -30000;
            bool b1 = false;
            bool b2 = false;
            while(src1 || src2) {
                if(src1.coord()<src2.coord()) {
                    b1 = src1.value();
                    where = src1.coord();
                    src1.next();
                } else {
                    b2 = src2.value();
                    where = src2.coord();
                    src2.next();
                }
                sink.append(where,b1&&b2);
            }
            sink.finish();
        }

        void line_or(RLELine &out,RLELine &l1,RLELine &l2,int offset2,int total) {
            TransitionSink sink(out,total);
            TransitionSource src1(l1,0),src2(l2,offset2);
            int where = -999;
            bool b1 = false;
            bool b2 = false;
            while(src1 || src2) {
                if(src1.coord()<src2.coord()) {
                    b1 = src1.value();
                    where = src1.coord();
                    src1.next();
                } else {
                    b2 = src2.value();
                    where = src2.coord();
                    src2.next();
                }
                sink.append(where,b1||b2);
            }
            sink.finish();
        }
    }

    ////////////////////////////////////////////////////////////////
    // image blits
    ////////////////////////////////////////////////////////////////

    // invert the given image
    
    void rle_invert(RLEImage &image) {
        for(int i=0;i<image.dim(0);i++) {
            line_invert(image.line(i),image.dim(1));
        }
    }

    // AND the two images together, offsetting the second by (d0,d1)

    void rle_and(RLEImage &image,RLEImage &mask,int d0,int d1) {
        image.verify();
        mask.verify();
        int start,inc,end;
        if(d0>0) {
            start = image.dim(0)-1; end = -1; inc = -1;
        } else {
            start = 0; end = image.dim(0); inc = 1;
        }
        RLELine temp;
        for(int i=start;i!=end;i+=inc) {
            int mi = i-d0;
            if(unsigned(mi)>=unsigned(mask.dim(0))) {
                RLELine empty;
                swap(image.line(i),empty);
            } else {
                verify_line(image.line(i));
                line_and(temp,image.line(i),mask.line(mi),d1,image.dim(1));
                swap(temp,image.line(i));
                verify_line(image.line(i));
            }
        }
        image.verify();
    }

    // OR the two images together, offsetting the second by (d0,d1)

    void rle_or(RLEImage &image,RLEImage &mask,int d0,int d1) {
        image.verify();
        mask.verify();
        int start,end,inc;
        if(d0>0) {
            start = image.dim(0)-1; end = -1; inc = -1;
        } else {
            start = 0; end = image.dim(0); inc = 1;
        }
        RLELine temp;
        for(int i=start;i!=end;i+=inc) {
            int mi = i-d0;
            if(unsigned(mi)>=unsigned(mask.dim(0))) {
                // nothing
            } else {
                verify_line(image.line(i),image.dim(1));
                line_or(temp,image.line(i),mask.line(mi),d1,image.dim(1));
                swap(temp,image.line(i));
                verify_line(image.line(i),image.dim(1));
            }
        }
        image.verify();
    }

    ////////////////////////////////////////////////////////////////
    // shifting and padding
    ////////////////////////////////////////////////////////////////

    namespace {
        void rle_shift_d0(RLEImage &image,int d) {
            if(d==0) return;
            if(d<0) {
                d=-d;
                for(int i=0;i<image.dim(0);i++)
                    if(i+d>=image.dim(0)) {
                        image.line(i).clear();
                    } else {
                        move(image.line(i),image.line(i+d));
                    }
            } else {
                for(int i=image.dim(0)-1;i>=0;i--) {
                    if(i-d<0) {
                        image.line(i).clear();
                    } else {
                        move(image.line(i),image.line(i-d));
                    }
                }
            }
            image.verify();
        }

        void rle_shift_d1(RLEImage &image,int d) {
            if(d==0) return;
            int w = image.dim(0), h = image.dim(1);
            for(int i=0;i<w;i++) {
                RLELine &line = image.line(i);
                int k = 0;
                for(int j=0;j<line.length();j++) {
                    ASSERT(k<=j);
                    RLERun r = line(j);
                    r += d;
                    r.start = max(0,r.start);
                    r.end = min(h,r.end);
                    if(r.end<0 || r.start>=h) continue;
                    line(k++) = r;
                }
            }
        }
    }

    void rle_shift(RLEImage &image,int d0,int d1) {
        rle_shift_d0(image,d0);
        rle_shift_d1(image,d1);
    }

    void rle_pad_x(RLEImage &image,int x0,int x1) {
        RLEImage temp;
        temp.resize(image.dim(0)+x0+x1,image.dim(1));
        for(int i=0;i<image.dim(0);i++) {
            int j = i+x0;
            if(j<0||j>=temp.dim(0)) continue;
            move(temp.line(j),image.line(i));
        }
        image.take(temp);
    }

    ////////////////////////////////////////////////////////////////
    // run-length morphology (using runs and transpose)
    ////////////////////////////////////////////////////////////////

    namespace {
        void erode_runs(RLELine &iline,int r,int d,int offset=0) {
            verify_line(iline,d);
            r--;
            LineSourceSink line(iline);
            while(!line.done()) {
                RLERun run = line.next();
                run.start += r/2;
                run.end -= r-r/2;
                if(offset) {
                    run.start += offset;
                    run.end += offset;
                }
                line.add(run.start,run.end);
            }
            line.finish();
            verify_line(iline,d);
        }

        void dilate_runs(RLELine &iline,int r,int d,int offset=0) {
            verify_line(iline,d);
            r--;
            LineSourceSink line(iline);
            while(!line.done()) {
                RLERun run = line.next();
                run.start -= (r-r/2);
                run.end += r/2;
                if(offset) {
                    run.start += offset;
                    run.end += offset;
                }
                line.add(run.start,run.end);
            }
            line.finish();
            trim_line(iline,d);
            verify_line(iline,d);
        }
    }

    void rle_erode_runs(RLEImage &image,int r) {
        int w = image.dim(0);
        for(int i=0;i<w;i++) {
            erode_runs(image.line(i),r,image.dim(1));
        }
    }

    void rle_dilate_runs(RLEImage &image,int r) {
        int w = image.dim(0);
        for(int i=0;i<w;i++) {
            dilate_runs(image.line(i),r,image.dim(1));
        }
    }

    void rle_erode_rect_runlength(RLEImage &image,int r0,int r1) {
        if(r1>0) {
            rle_erode_runs(image,r1);
        }
        if(r0>0) {
            rle_transpose(image);
            rle_erode_runs(image,r0);
            rle_transpose(image);
        }
        image.verify();
    }

    void rle_dilate_rect_runlength(RLEImage &image,int r0,int r1) {
        if(r1>0) {
            rle_shift(image,0,1-r1%2); // make open/close work correctly for even r1
            rle_dilate_runs(image,r1);
        }
        if(r0>0) {
            rle_shift(image,1-r0%2,0); // make open/close work correctly for even r0
            rle_transpose(image);
            rle_dilate_runs(image,r0);
            rle_transpose(image);
        }
        image.verify();
    }

    ////////////////////////////////////////////////////////////////
    // rectangular morphology (using blit and decomposition)
    ////////////////////////////////////////////////////////////////

    void rle_dilate_rect_decomp(RLEImage &image,int r0,int r1) {
        if(r1>1) {
            throw "not implemented";
        }
        if(r0>1) {
            rle_shift(image,-(r0-1)/2,0);
            int width = 1;
            while(2*width<r0) {
                rle_or(image,image,width,0);
                width *= 2;
            }
            if(width<r0) rle_or(image,image,r0-width,0);
        }
        image.verify();
    }

    void rle_erode_rect_decomp(RLEImage &image,int r0,int r1) {
        if(r1>1) {
            throw "not implemented";
        }
        if(r0>1) {
            rle_pad_x(image,100,100);
            rle_shift(image,-r0/2,0);
            int width = 1;
            while(2*width<r0) {
                rle_and(image,image,width,0);
                width *= 2;
            }
            if(width<r0) rle_and(image,image,r0-width,0);
            rle_pad_x(image,-100,-100);
        }
        image.verify();
    }

    ////////////////////////////////////////////////////////////////
    // rectangular morphology
    //
    // First, perform runlength morphology in one direction, then
    // bit blit morphology in the other.  This approach avoids the
    // transpose and offers overall good scaling behavior.
    ////////////////////////////////////////////////////////////////

    void rle_dilate_rect(RLEImage &image,int r0,int r1) {
        rle_dilate_rect_runlength(image,0,r1);
        rle_dilate_rect_decomp(image,r0,0);
    }

    void rle_erode_rect(RLEImage &image,int r0,int r1) {
        rle_erode_rect_runlength(image,0,r1);
        rle_erode_rect_decomp(image,r0,0);
    }

    void rle_open_rect(RLEImage &image,int r0,int r1) {
        rle_erode_rect(image,r0,r1);
        rle_dilate_rect(image,r0,r1);
    }

    void rle_close_rect(RLEImage &image,int r0,int r1) {
        rle_dilate_rect(image,r0,r1);
        rle_erode_rect(image,r0,r1);
    }

    ////////////////////////////////////////////////////////////////
    // arbitrary masks
    ////////////////////////////////////////////////////////////////

    static inline int square(int i) { return i*i; }

    void rle_circular_mask(RLEImage &image,int r) {
        bytearray temp(r,r);
        fill(temp,0);
        for(int i=0;i<r;i++) for(int j=0;j<r;j++) {
            if(square(i-r/2)+square(j-r/2)<=square(r/2)) 
                temp(i,j) = 255;
        }
        rle_convert(image,temp);
    }

    void rle_erode_mask(RLEImage &image,RLEImage &mask,int dx,int dy) {
        RLEImage result;
        result.resize(image.dim(0),image.dim(1));
        result.fill(1);
        for(int mi=0;mi<mask.nlines();mi++) {
            RLELine &mline = mask.line(mi);
            if(mline.length()==0) continue;
            RLERun run = mask.line(mi)(0);
            RLELine temp;
            RLELine out;
            for(int i=mi;i<image.dim(0);i++) {
                copy(temp,image.line(i));
                for(int j=0;j<mline.length();j++)
                    erode_runs(temp,run.end-run.start,image.dim(1),run.start);
                line_and(out,result.line(i-mi),temp,0,image.dim(1));
                swap(out,result.line(i-mi));
            }
        }
        image.take(result);
    }

    ////////////////////////////////////////////////////////////////
    // connected component labeling
    ////////////////////////////////////////////////////////////////

    int label_components(objlist<intarray> &labels,RLEImage &image) {
        int maxcomponents = image.number_of_runs() + 1;
        labels.dealloc();
        labels.resize(image.nlines());
        UnionFind uf(maxcomponents);
        int count = 1;
        uf.make_set(0);
        for(int j=0;j<image.line(0).length();j++) {
            uf.make_set(count);
            labels(0).push(count++);
        }
        for(int i=1;i<image.nlines();i++) {
            RLELine &above = image.line(i-1);
            RLELine &line = image.line(i);
            intarray &above_labels = labels(i-1);
            intarray &line_labels = labels(i);
            ASSERT(line_labels.length()==0);
            int start = 0;
            for(int k=0;k<line.length();k++) {
                uf.make_set(count);
                line_labels.push(count);
#ifdef TESTING
                for(int j=0;j<above.length();j++) {
                    if(above(j).overlaps(line(k)))
                        uf.make_union(above_labels(j),line_labels(k));
                }
#else
                bool found = 0;
                for(int j=start;j<above.length();j++) {
                    if(above(j).before(line(k))) {
                        start = j;
                    } else if(above(j).overlaps(line(k))) {
                        found = 1;
                        uf.make_union(above_labels(j),line_labels(k));
                    } else if(above(j).after(line(k))) {
                        break;
                    }
                }
#endif
                count++;
            }
        }
        for(int i=0;i<labels.length();i++) {
            intarray &line_labels = labels(i);
            for(int j=0;j<line_labels.length();j++)
                line_labels(j) = uf.find_set(line_labels(j));
        }
        return renumber_labels(labels,1);
    }

    int rle_bounding_boxes(narray<rectangle> &boxes,RLEImage &image) {
        objlist<intarray> labels;
        int n = label_components(labels,image);
        boxes.dealloc();
        boxes.resize(n);
        for(int i=0;i<image.nlines();i++) {
            RLELine &line = image.line(i);
            for(int j=0;j<line.length();j++) {
                int index = labels(i)(j);
                boxes(index).include(i,line(j).start);
                boxes(index).include(i,line(j).end);
            }
        }
        boxes[0] = rectangle(0,0,image.dim(0),image.dim(1));
        return n;
    }
    void rle_runlength_statistics(floatarray &h0,floatarray &h1,RLEImage &image) {
        CHECK_ARG(h1.length()>1);
        CHECK_ARG(h0.length()>1);
        fill(h1,0);
        fill(h0,0);
        for(int i=0;i<image.nlines();i++) {
            RLELine &line = image.line(i);
            for(int j=0;j<line.length();j++) {
                int br = 0;
                int wr = 0;
                if(line.length()>0) {
                    if(j==0) br = line(0).start;
                    else br = line(j).start - line(j-1).end;
                    if(j==line.length()-1) br = image.dim(1)-line(j).end;
                    else br = line(j+1).start - line(j).end;
                    wr = line(j).end - line(j).start;
                } else {
                    br = image.dim(1);
                    wr = 0;
                }
                if(br>0 && br<h0.length()) h0(br)++;
                if(wr>0 && wr<h1.length()) h1(wr)++;
            }
        }
    }
    void rle_peak_estimation(intarray &h0,intarray &h1,intarray &v0,intarray &v1,RLEImage &image,float sh,float sv) {
        floatarray histh0(1000),histh1(1000),histv0(1000),histv1(1000);
        rle_runlength_statistics(histh0,histh1,image);
        peaks(h0,histh0,0,200,sh);
        peaks(h1,histh1,0,200,sh);
        RLEImage temp;
        rle_transpose(temp,image);
        rle_runlength_statistics(histv0,histv1,temp);
        peaks(v0,histv0,0,200,sv);
        peaks(v1,histv1,0,200,sv);
    }

    // FIXME do something more efficient 

    void rle_read(RLEImage &image,const char *file) {
        bytearray temp;
        read_image_binary(temp,file);
        rle_convert(image,temp);
    }

    // FIXME do something more efficient 

    void rle_write(const char *file,RLEImage &image) {
        bytearray temp;
        rle_convert(temp,image);
        write_png(stdio(file,"w"),temp);
    }

    void rle_dshow(RLEImage &image, const char *spec) {
        bytearray temp;
        rle_convert(temp,image);
        dshow(temp,spec);
    }

    void rle_debug(RLEImage &image) {
        for(int i=0;i<image.dim(0);i++) {
            printf("%3d:",i);
            RLELine &line = image.line(i);
            for(int j=0;j<line.length();j++)
                printf(" [%d %d]",line(j).start,line(j).end);
            printf("\n");
        }
        printf("\n");
        image.verify();
    }
}

////////////////////////////////////////////////////////////////
// below are implementations that are mainly for testing
////////////////////////////////////////////////////////////////

namespace imgrle {
    void rle_transpose_bruteforce(RLEImage &out,RLEImage &in) {
        bytearray temp;
        rle_convert(temp,in);
        transpose(temp);
        rle_convert(out,temp);
    }

    void rle_erode_rect_bruteforce(RLEImage &image,int r0,int r1) {
        image.verify();
        RLEImage temp;
        if(r0>0) {
            temp.copy(image);
            image.fill(1);
            for(int i=0;i<r0;i++)
                rle_and(image,temp,i-r0/2,0);
            image.verify();
        }
        if(r1>0) {
            temp.copy(image);
            image.fill(1);
            for(int j=0;j<r1;j++)
                rle_and(image,temp,0,j-r1/2);
            image.verify();
        }
    }

    void rle_dilate_rect_bruteforce(RLEImage &image,int r0,int r1) {
        image.verify();
        RLEImage temp;
        temp.copy(image);
        if(r0>0) {
            image.fill(0);
            for(int i=0;i<r0;i++) 
                rle_or(image,temp,i-(r0-1)/2,0);
        }
        if(r1>0) {
            temp.copy(image);
            image.fill(0);
            for(int j=0;j<r1;j++)
                rle_or(image,temp,0,j-(r1-1)/2);
        }
        image.verify();
    }

    // TODO this function is just incredibly ugly; rewrite it in terms of
    // TransitionSink eventually

    void rle_transpose_runs(RLEImage &out,RLEImage &in) {
        in.verify();
        // transpose using merging of interval lists; this code is rather
        // messy because of boundary conditions, the many different cases that
        // can occur, and the lack of lexically scoped local functions in C++
        CHECK_ARG(out.doesNotAlias(in));
        intarray indexes(in.dim(1));
        intarray starts(in.dim(1));
        fill(starts,-1);
        out.resize(in.dim(1),in.dim(0));
        RLELine open;
        intarray open_from;
        RLELine next;
        intarray next_from;
        RLELine empty;
        for(int here=0;here<=in.dim(0);here++) {
            next.clear();
            next_from.clear();
            RLELine &line = here==in.dim(0)?empty:in.line(here);
            RLERun O(0,0);
            RLERun L(0,0);
            int last = -1;
            int jo = 0;
            int jl = 0;
            for(;;) {
#define finish(FROM,TO) do{int TO_=min(TO,out.nlines()); for(int K=FROM;K<TO_;K++) out.line(K).push(RLERun(last,here));}while(0)
#define more(FROM,TO) do{if(FROM<TO) {next.push(RLERun(FROM,TO)); next_from.push(last);}}while(0)
#define commence(FROM,TO) do{if(FROM<TO) {next.push(RLERun(FROM,TO)); next_from.push(here);}}while(0)
                if(O.start==O.end) {
                    if(jo==open.length()) {
                        commence(L.start,L.end);
                        while(jl<line.length()) {
                            L = line(jl);
                            commence(L.start,L.end);
                            jl++;
                        }
                        break;
                    } else {
                        O = open(jo);
                        last = open_from(jo);
                        jo++;
                    }
                }
                if(L.start==L.end) {
                    if(jl==line.length()) {
                        finish(O.start,O.end);
                        while(jo<open.length()) {
                            O = open(jo);
                            last = open_from(jo);
                            finish(O.start,O.end);
                            jo++;
                        }
                        break;
                    } else {
                        L = line(jl);
                        jl++;
                    }
                }
                if(O.start==L.start && O.end==L.end) { // O = L, common special case
                    more(O.start,O.end);
                    O.start = O.end;
                    L.start = L.end;
                } else if(O.end<=L.start) { // O L
                    finish(O.start,O.end);
                    O.start = O.end;
                } else if(O.start>=L.start && O.end<=L.end) { // L LO L
                    commence(L.start,O.start);
                    more(O.start,O.end);
                    O.start = O.end;
                    L.start = O.end;
                } else if(L.end<=O.start) { // L O
                    commence(L.start,L.end);
                    L.start = L.end;
                } else if(L.start>=O.start && L.end<=O.end) { // O LO O
                    finish(O.start,L.start);
                    more(L.start,L.end);
                    L.start = L.end;
                    O.start = L.end;
                } else if(O.start<=L.start && O.end>=L.start && O.end<=L.end) { // O LO L
                    finish(O.start,L.start);
                    more(L.start,O.end);
                    O.start = O.end;
                    L.start = O.end;
                } else if(L.start<=O.start && L.end>=O.start && L.end<=O.end) { // L LO O
                    commence(L.start,O.start);
                    more(O.start,L.end);
                    O.start = L.end;
                    L.start = L.end;
                } else {
                    abort();
                }
#undef commence
#undef more
#undef finish
            } /* for(;;) */
            swap(next,open);
            swap(next_from,open_from);
        } /* for(here=...;;) */
        out.verify();
    }
}
