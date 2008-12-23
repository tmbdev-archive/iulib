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
// File: imglabels.cc
// Purpose: connected component labeling
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

#include "colib/colib.h"
#include "imglib.h"

#include <map> /* CODE-OK--tmb */

using namespace std;

using namespace colib;

namespace iulib {

    /// Propagate labels across the entire image from a set of non-zero seeds.

    void propagate_labels(intarray &image) {
        floatarray dist;
        narray<point> source;
        copy(dist,image);
        brushfire_2(dist,source,1000000);
        for(int i=0;i<dist.length1d();i++) {
            point p = source.at1d(i);
            if(!image.at1d(i)) image.at1d(i) = image(p.x,p.y);
        }
    }

    /// Propagate labels across the non-zero pixels of the target image from the seed image.

    void propagate_labels_to(intarray &target,intarray &seed) {
        floatarray dist;
        narray<point> source;
        copy(dist,seed);
        brushfire_2(dist,source,1000000);
        for(int i=0;i<dist.length1d();i++) {
            point p = source.at1d(i);
            if(target.at1d(i)) target.at1d(i) = seed(p.x,p.y);
        }
    }

    inline bool dontcare(int x) {
        return (x&0xffffff)==0xffffff;
    }

    void remove_dontcares(intarray &image) {
        floatarray dist;
        narray<point> source;
        dist.resize(image.dim(0),image.dim(1));
        for(int i=0;i<dist.length1d();i++)
            if(!dontcare(image.at1d(i))) dist.at1d(i) = !!image.at1d(i);
        brushfire_2(dist,source,1000000);
        for(int i=0;i<dist.length1d();i++) {
            point p = source.at1d(i);
            if(dontcare(image.at1d(i))) image.at1d(i) = image(p.x,p.y);
        }
    }

    struct Enumerator {
        int n;
        map<int,int> translation;
        Enumerator(int start=0) {
            n = start;
        }
        int operator[](int i) {
            map<int,int>::iterator p;
            p = translation.find(i);
            if(p!=translation.end()) {
                return p->second;
            } else {
                translation.insert(pair<int,int>(i,n));
                return n++;
            }
        }
        int total() {
            return n;
        }
    };


    /// Renumber the non-zero pixels in an image to start with pixel value start.
    /// The numerical order of pixels is preserved.

    int renumber_labels(intarray &image,int start=1) {
        map<int,int> translation;
        int n = start;
        for(int i=0;i<image.length1d();i++) {
            int pixel = image.at1d(i);
            if(pixel==0 || pixel==0xffffff) continue;
            map<int,int>::iterator p = translation.find(pixel);
            if(p==translation.end())
                translation.insert(pair<int,int>(pixel,n++));
        }
        n = start;
        for(map<int,int>::iterator p = translation.begin();p!=translation.end();p++) {
            p->second = n++;
        }
        for(int i=0;i<image.length1d();i++) {
            int pixel = image.at1d(i);
            if(pixel==0 || pixel==0xffffff) continue;
            image.at1d(i) = translation[pixel];
        }
        return n;
    }

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

    /// Label the connected components of an image.

    int label_components(intarray &image,bool four_connected) {
        int w = image.dim(0), h = image.dim(1);
        // We slice the image into columns and call make_set()
        // for every continuous segment within each column.
        // Maximal number of segments per column is (h + 1) / 2.
        // We do it `w' times, so it's w * (h + 1) / 2.
        // We also need to add 1 because index 0 is not used, but counted.
        UnionFind uf(w * (h + 1) / 2 + 1);
        uf.make_set(0);
        int top = 1;
        for(int i=0;i<image.length1d();i++) image.at1d(i) = !!image.at1d(i);
        //for(int i=0;i<w;i++) {image(i,0) = 0; image(i,h-1) = 0;}
        //for(int j=0;j<h;j++) {image(0,j) = 0; image(w-1,j) = 0;}
        for(int i=0;i<w;i++) {
            int current_label = 0;
            for(int j=0;j<h;j++) {
                int pixel = image(i,j);
                int range = four_connected?0:1;
                for(int delta=-range;delta<=range;delta++) {
                    int adj_label = bat(image,i-1,j+delta,0);
                    if(!pixel) {
                        current_label = 0;
                        continue;
                    }
                    if(!current_label) {
                        current_label = top;
                        uf.make_set(top);
                        top++;
                    }
                    if(adj_label) {
                        current_label = uf.find_set(current_label);
                        adj_label = uf.find_set(adj_label);
                        if(current_label != adj_label) {
                            uf.make_union(current_label,adj_label);
                            current_label = uf.find_set(current_label);
                            adj_label = uf.find_set(adj_label);
                        }
                    }
                    image(i,j) = current_label;
                }
            }
        }
        for(int i=0;i<image.length1d();i++) {
            if(!image.at1d(i)) continue;
            image.at1d(i) = uf.find_set(image.at1d(i));
        }
        return renumber_labels(image,1);
    }

    int colors[] = {
        0xff00ff,
        0x009f4f,
        0xff0000,
        0x0000ff,
        0xff0000,
        0xffff00,
        0x6f007f,
        0x00ff00,
        0x004f9f,
        0x7f9f00,
        0xaf006f,
        0x00ffff,
        0xff5f00,
    };

#define ncolors (sizeof colors/sizeof colors[0])

    void simple_recolor(intarray &image) {
        Enumerator enumerator(1);
        for(int i=0;i<image.length1d();i++) {
            if(!image.at1d(i)) continue;
            image.at1d(i) = enumerator[image.at1d(i)];
        }
        for(int i=0;i<image.length1d();i++) {
            int value = image.at1d(i);
            if(value==0||value==0xffffff) continue;
            image.at1d(i) = interesting_colors(1+value%19);
            //CK: tests for dshowr
            //if(!image.at1d(i)) image.at1d(i) = 0xffffff;
            //else image.at1d(i) = colors[image.at1d(i)%ncolors];
        }
    }

    /// Compute the bounding boxes for the pixels in the image.

    void bounding_boxes(narray<rectangle> &result,intarray &image) {
        result.dealloc();
        int n = max(image);
        if(n<1) return;
        result.resize(n+1);
        for(int i=0;i<image.dim(0);i++) for(int j=0;j<image.dim(1);j++) {
            int value = image(i,j);
            result(value).include(i,j);
        }
    }
}
