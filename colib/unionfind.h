// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
// Copyright 1995-2005 by Thomas M. Breuel
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
// File: checks.h
// Purpose: contains macros for assertions, checks, and unit testing
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

#ifndef unionfind_h_
#define unionfind_h_

namespace colib {
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
}
#endif
