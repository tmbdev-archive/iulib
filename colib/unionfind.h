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
