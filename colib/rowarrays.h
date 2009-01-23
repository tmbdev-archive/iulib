// -*- C++ -*-

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
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
// File: rowarrays.h
// Purpose:
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file rowarrays.h
/// \brief Row-wise operations on 2D arrays

#ifndef h_rowarrays__
#define h_rowarrays__

#include "colib/checks.h"
#include "colib/narray.h"

namespace colib {

    /// Swap two values.

    namespace {
        template <class T,class S>
        inline void rswap_(T &a,S &b) {
            T temp = b;
            b = a;
            a = temp;
        }
    }


    template <class T>
    int rowargmax(narray<T> &values,int i) {
        if(values.dim(1)<1) return -1;
        int mj = 0;
        T mv = values(i,0);
        for(int j=1;j<values.dim(1);j++) {
            T value = values(i,j);
            if(value<=mv) continue;
            mv = value;
            mj = j;
        }
        return mj;
    }

    template <class T>
    int rowargmin(narray<T> &values,int i) {
        if(values.dim(1)<1) return -1;
        int mj = 0;
        T mv = values(i,0);
        for(int j=1;j<values.dim(1);j++) {
            T value = values(i,j);
            if(value>=mv) continue;
            mv = value;
            mj = j;
        }
        return mj;
    }

    template <class T>
    T rowdist_euclidean_squared(narray<T> &values,int i,narray<T> &v) {
        CHECK_ARG(v.rank()==1 && v.length()==values.dim(1));
        int n = v.length();
        T *row = &values(i,0);
        T *vec = &v(0);
        double total = 0.0;
        for(int j=0;j<n;j++) {
            double d = (*row++) - (*vec++);
            total += d*d;
        }
        return (T)total;
    }

    template <class T>
    T rowdist_euclidean(narray<T> &values,int i,narray<T> &v) {
        return (T)sqrt(rowdist_euclidean_squared(values,i,v));
    }

    template <class T>
    int rowargmindist(narray<T> &a,narray<T> &v) {
        int mi = -1;
        T mv = 1e30;
        for(int i=0;i<a.dim(0);i++) {
            T value = rowdist_euclidean_squared(a,i,v);
            if(value>=mv) continue;
            mv = value;
            mi = i;
        }
        return mi;
    }

    template <class T>
    T &rowmax(narray<T> &values,int i) {
        return values(i,rowargmax(values,i));
    }

    template <class T>
    T &rowmin(narray<T> &values,int i) {
        return values(i,rowargmin(values,i));
    }

    template <class T>
    int rowcompare(narray<T> &values,int i,int j) {
        for(int k=0;k<values.dim(1);k++) {
            if(values(i,k)<values(j,k)) return -1;
            if(values(i,k)>values(j,k)) return 1;
        }
        return 0;
    }

    template <class T>
    void rowswap(narray<T> &values,int i,int j) {
        for(int k=0;k<values.dim(1);k++)
            rswap_(values(i,k),values(j,k));
    }

    template <class T>
    void rowcopy(narray<T> &values,int i,int j) {
        for(int k=0;k<values.dim(1);k++)
            values(i,k) = values(j,k);
    }

    template <class T,class S>
    void rowcopy(narray<T> &a,int i,narray<S> &b) {
        CHECK_ARG(a.dim(1)==b.length());
        for(int k=0;k<a.dim(1);k++)
            a(i,k) = b(k);
    }

    template <class T,class S>
    void rowcopy(narray<T> &a,narray<S> &b,int i) {
        a.resize(b.dim(1));
        for(int k=0;k<b.dim(1);k++)
            a(k) = b(i,k);
    }

    template <class S,class T>
    void rowpush(narray<S> &table,narray<T> &data) {
        if(table.length1d()==0) {
            copy(table,data);
            table.reshape(1,table.length());
            return;
        }
        CHECK_ARG(table.dim(1)==data.length());
        table.reserve(table.length1d()+data.length());
        table.setdims_(table.dim(0)+1,table.dim(1),0,0);
        rowcopy(table,table.dim(0)-1,data);
    }

    template <class T>
    int rowduplicates(narray<T> &data) {
        int count = 0;
        for(int i=1;i<data.dim(0);i++)
            if(rowcompare(data,i-1,i)==0)
                count++;
        return count;
    }

    template <class T>
    int rowduplicates(narray<T> &data,intarray &permutation) {
        CHECK_ARG(data.dim(0)==permutation.length());
        int count = 0;
        for(int i=1;i<data.dim(0);i++)
            if(rowcompare(data,permutation(i-1),permutation(i))==0)
                count++;
        return count;
    }

    template <class T>
    bool rowsorted(narray<T> &data) {
        for(int i=1;i<data.dim(0);i++)
            if(rowcompare(data,i-1,i)>0)
                return 0;
        return 1;
    }

    template <class T>
    bool rowsorted(narray<T> &data,intarray &permutation) {
        CHECK_ARG(data.dim(0)==permutation.length());
        for(int i=1;i<data.dim(0);i++)
            if(rowcompare(data,permutation[i-1],permutation[i])>0)
                return 0;
        return 1;
    }


    template <class T>
    void check_rowsorted(narray<T> &data) {
        CHECK_ARG(rowsorted(data));
    }

    template <class T>
    void check_rowsorted(narray<T> &data,intarray &permutation) {
        CHECK_ARG(rowsorted(data,permutation));
    }

    template <class T>
    void rowsort(narray<int> &index,narray<T> &values,int start,int end) {
        if(start>=end-1) return;

        // pick a pivot
        // NB: it's OK for this to be a reference pointing into values
        // since we aren't actually moving the elements of values[] around

        int pivot = index[(start+end-1)/2];

        // first, split into two parts: less than the pivot
        // and greater-or-equal

        int lo = start;
        int hi = end;
        for(;;) {
            while(lo<hi && rowcompare(values,index[lo],pivot)<0) lo++;
            while(lo<hi && rowcompare(values,index[hi-1],pivot)>=0) hi--;
            if(lo==hi || lo==hi-1) break;
            rswap_(index[lo],index[hi-1]);
            lo++;
            hi--;
        }
        int split1 = lo;

        // now split into two parts: equal to the pivot
        // and strictly greater.

        hi = end;
        for(;;) {
            while(lo<hi && rowcompare(values,index[lo],pivot)==0) lo++;
            while(lo<hi && rowcompare(values,index[hi-1],pivot)>0) hi--;
            if(lo==hi || lo==hi-1) break;
            rswap_(index[lo],index[hi-1]);
            lo++;
            hi--;
        }
        int split2 = lo;

#ifdef TEST
        for(int i=start;i<split1;i++) ASSERT(rowcompare(values,index[i],pivot)<0);
        for(int i=split1;i<split2;i++) ASSERT(rowcompare(values,index[i],pivot)==0);
        for(int i=split2;i<end;i++) ASSERT(rowcompare(values,index[i],pivot)>0);
#endif

        rowsort(index,values,start,split1);
        rowsort(index,values,split2,end);
    }

    /// Quicksort the rows of a 2D array, generating a permutation of the indexes.

    template <class T>
    void rowsort(narray<int> &index,narray<T> &values) {
        index.resize(values.dim(0));
        for(int i=0;i<values.dim(0);i++) index[i] = i;
        rowsort(index,values,0,index.length());
    }

    /// Permute the elements of an array given a permutation.

    template <class T>
    void rowpermute(narray<T> &data,narray<int> &permutation) {
        CHECK_ARG(data.dim(0)==permutation.length());
        narray<bool> finished(data.dim(0));
        fill(finished,false);
        for(int start=0;start<finished.length();start++) {
            if(finished(start)) continue;
            int index = start;
            narray<T> value;
            rowcopy(value,data,index);
            for(;;) {
                int next = permutation(index);
                if(next==start) break;
                rowcopy(data,index,next);
                index = next;
                CHECK_ARG(!finished(index) && "not a permutation");
                finished(index) = true;
                index = next;
            }
            rowcopy(data,index,value);
            finished(index) = true;
        }
    }

    /// Quicksorts the rows of an array in place.
    /// (Uses an intermediate permutation, so elements are moved around minimally.))

    template <class T>
    void rowsort(narray<T> &data) {
        narray<int> permutation;
        rowsort(permutation,data);
        rowpermute(data,permutation);
    }

    /// Select all the rows matching a given prefix

    template <class T>
    int rowprefixselect(intarray &matching,narray<T> &table,narray<T> &prefix) {
        CHECK_ARG(table.dim(1)>=prefix.length());
        check_rowsorted(table);
        matching.clear();
        // FIXME use binary sort here
        for(int i=0;i<table.dim(0);i++) {
            bool match = true;
            for(int j=0;j<prefix.length();j++) {
                if(table(i,j)!=prefix(j)) {
                    match = false;
                    break;
                }
            }
            if(match) matching.push(i);
        }
        return matching.length();
    }

    template <class S,class T>
    inline void rowget(narray<S> &v,narray<T> &data,int row) {
        v.resize(data.dim(1));
        for(int i=0;i<v.length();i++)
            v(i) = data(row,i);
    }

    template <class S,class T>
    inline void rowput(narray<S> &data,int row,narray<T> &v) {
        CHECK(v.length()==data.dim(1));
        for(int i=0;i<v.length();i++)
            data(row,i) = v(i);
    }

    template <class T>
    inline double rowsum(narray<T> &a,int i) {
        double total = 0;
        for(int j=0;j<a.dim(1);j++) total += a(i,j);
        return total;
    }

    template <class T>
    inline double colsum(narray<T> &a,int j) {
        double total = 0;
        for(int i=0;j<a.dim(0);i++) total += a(i,j);
        return total;
    }
}

#endif
