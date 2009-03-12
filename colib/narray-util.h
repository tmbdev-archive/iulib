// -*- C++ -*-

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
// Copyright 1995-2005 Thomas M. Breuel.
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
// File: narray-util.h
// Purpose: miscellaneous array utility functions
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file narray-util.h
/// \brief Miscellaneous array utility functions


#ifndef h_narray_util__
#define h_narray_util__

#include <math.h>
#include <stdlib.h>
#include "colib/checks.h"
#include "misc.h"
#ifdef WIN32
#include "compat.h"
#endif

///
/// \brief Miscellaneous array utility functions


namespace colib {
    /// Make sure the array doesn't contain any NaN values.

    template <class T>
    void check_nan(narray<T> &v) {
        for(int i=0;i<v.length1d();i++)
            ASSERT(!isnan(v.unsafe_at1d(i)));
    }

    /// Compute the global max of the absolute value of the array.

    template <class T>
    inline T absmax(narray<T> &a) {
        T value = a.at1d(0);
        for(int i=1;i<a.length1d();i++) {
            T nvalue = a.at1d(i);
            if(nvalue<0) nvalue = -nvalue;
            if(nvalue<=value) continue;
            value = nvalue;
        }
        return value;
    }

    /// Compute the global max of the array.

    template <class T>
    inline T max(narray<T> &a) {
        T value = a.at1d(0);
        for(int i=1;i<a.length1d();i++) {
            T nvalue = a.at1d(i);
            if(nvalue<=value) continue;
            value = nvalue;
        }
        return value;
    }

    /// Compute the global min of the array.

    template <class T>
    inline T min(narray<T> &a) {
        T value = a.at1d(0);
        for(int i=1;i<a.length1d();i++) {
            T nvalue = a.at1d(i);
            if(nvalue>=value) continue;
            value = nvalue;

        }
        return value;
    }

    /// Sum the values of the array.

    template <class T>
    inline double sum(narray<T> &data) {
        double result = 0.0;
        int n = data.length1d();
        for(int i=0;i<n;i++)
            result += data.at1d(i);
        return result;
    }

    /// The product of the values of the array.

    template <class T>
    inline double product(narray<T> &data) {
        double result = 1.0;
        int n = data.length1d();
        for(int i=0;i<n;i++)
            result *= data.at1d(i);
        return result;
    }

    /// Compute the argmax of the rank-1 array.

    template <class T>
    inline int argmax(narray<T> &a) {
        CHECK_ARG(a.rank()==1 && a.dim(0)>0);
        T value = a(0);
        int index = 0;
        for(int i=1;i<a.dim(0);i++) {
            T nvalue = a(i);
            if(nvalue<=value) continue;
            value = nvalue;
            index = i;
        }
        return index;
    }

    /// Compute the argmin of the rank-1 array.

    template <class T>
    inline int argmin(narray<T> &a) {
        CHECK_ARG(a.rank()==1 && a.dim(0)>0);
        T value = a(0);
        int index = 0;
        for(int i=1;i<a.dim(0);i++) {
            T nvalue = a(i);
            if(nvalue>=value) continue;
            value = nvalue;
            index = i;
        }
        return index;
    }

    /// Make a unit vector of length n with a 1 in position i.

    inline void make_unit_vector(floatarray &a,int n,int i) {
        a.resize(n);
        fill(a,0);
        a(i) = 1;
    }


    /// Randomly permute the elements of a vector.

    template <class T>
    inline void randomly_permute(narray<T> &v) {
        int n = v.length();
        for(int i=0;i<n-1;i++) {
            int target = rand()%(n-i)+i;
            T temp = v[target];
            v[target] = v[i];
            v[i] = temp;
        }
    }

    /// Euclidean distance squared.

    inline double dist2squared(floatarray &a,floatarray &b) {
        CHECK_ARG(samedims(a,b));
        double total = 0.0;
        for(int i=0;i<a.length1d();i++)
            total += sqr(a.at1d(i)-b.at1d(i));
        CHECK_ARG(!isnan(total));
        return total;
    }

    /// Euclidean distance.

    inline double dist2(floatarray &a,floatarray &b) {
        return sqrt(dist2squared(a,b));
    }

    /// Euclidean norm squared.

    inline double norm2squared(floatarray &a) {
        double total = 0.0;
        for(int i=0;i<a.length1d();i++)
            total += sqr(a.at1d(i));
        return total;
    }

    /// Euclidean norm.

    inline double norm2(floatarray &a) {
        return sqrt(norm2squared(a));
    }

    /// Normalize the Euclidean norm of the array.

    inline void normalize2(floatarray &a) {
        double scale = 1.0/norm2(a);
        for(int i=0;i<a.length1d();i++)
            a.at1d(i) *= scale;
    }

    /// 1-norm.

    inline double norm1(floatarray &a) {
        double total = 0.0;
        int n = a.length1d();
        for(int i=0;i<n;i++) total += fabs(a.unsafe_at1d(i));
        return total;
    }

    /// Normalize the 1-norm of the array.

    inline void normalize1(floatarray &a) {
        double scale = 1.0/norm1(a);
        for(int i=0;i<a.length1d();i++)
            a.unsafe_at1d(i) *= scale;
    }

    /// Make a random vector of length n with uniformly random values
    /// between -scale and scale.

    inline void make_random(floatarray &v,int n,float scale) {
        v.resize(n);
        for(int i=0;i<n;i++)
            v(i) = (double(rand())/RAND_MAX*2-1)*scale;
    }

    /// Fill an array with random values.

    inline void fill_uniform(floatarray &v,float lo,float hi) {
        int n = v.length1d();
        for(int i=0;i<n;i++)
            v.at1d(i) = drand48()*(hi-lo)+lo;
    }

    /// Perturb the array with uniformly random values between -scale and scale.

    inline void perturb(floatarray &v,float scale) {
        int n = v.length1d();
        for(int i=0;i<n;i++)
            v.at1d(i) += (double(rand())/RAND_MAX*2-1)*scale;
    }

    /// Make a vector of length n with entries from 0 to n-1.

    template <class T>
    inline void iota(narray<T> &v,int n) {
        v.resize(n);
        for(int i=0;i<n;i++)
            v[i] = i;
    }

    /// reverse an array

    template <class T>
    inline void reverse(narray<T> &out,narray<T> &in) {
        out.clear();
        for(int i=in.length()-1;i>=0;i--)
            out.push(in(i));
    }

    /// reverse an array

    template <class T>
    inline void remove_left(narray<T> &a,int offset) {
        CHECK_ARG(offset>=0);
        if(offset==0) return;
        narray<T> temp;
        for(int i=offset;i<a.length();i++)
            temp.push(a(i));
        move(a,temp);
    }

    /// reverse an array in place

    template <class T>
    inline void reverse(narray<T> &out) {
        if(out.length()<1) return;
        int n = out.length();
        int m = int(n/2);
        for(int i=0;i<m;i++) {
            T temp = out(i);
            out(i) = out(n-i-1);
            out(n-i-1) = temp;
        }
    }

    /// Add the data to the result.

    template <class T>
    inline void add(narray<T> &result,narray<T> &data) {
        CHECK_ARG(samedims(result,data));
        int n = result.length1d();
        for(int i=0;i<n;i++)
            result.at1d(i) += data.at1d(i);
    }

    /// Multiply the data with scale and add to result.

    template <class T>
    inline void addscaled(narray<T> &result,narray<T> &data,double scale) {
        CHECK_ARG(samedims(result,data));
        int n = result.length();
        for(int i=0;i<n;i++)  result[i] += data[i] * scale;
    }

    /// Array subscripting with extending boundary conditions.

    template <class T>
    inline T &ext(narray<T> &a,int i) {
        i = max(0,min(i,a.dim(0)-1));
        return a.unsafe_at(i);
    }

    /// Array subscripting with extending boundary conditions.

    template <class T>
    inline T &ext(narray<T> &a,int i,int j) {
        i = max(0,min(i,a.dim(0)-1));
        j = max(0,min(j,a.dim(1)-1));
        return a.unsafe_at(i,j);
    }

    /// Array subscripting with fixed boundary conditions.

    template <class T,class U>
    inline T bat1(narray<T> &a,int i,U value) {
        if(unsigned(i)>=a.dim(0)) return value;
        return a.unsafe_at(i);
    }

    /// Array subscripting with fixed boundary conditions.

    template <class T,class U>
    inline T bat(narray<T> &a,int i,int j,U value) {
        if(unsigned(i)>=unsigned(a.dim(0))) return value;
        if(unsigned(j)>=unsigned(a.dim(1))) return value;
        return a.unsafe_at(i,j);
    }

    template <class T>
    void remove_element(narray<T> &a,int index) {
        narray<T> result;
        for(int i=0;i<a.length();i++)
            if(i!=index)
                result.push(a[i]);
        move(a,result);
    }

    template <class T,class S>
    void remove_value(narray<T> &a,S v) {
        intarray result;
        for(int i=0;i<a.length();i++)
            if(a[i]!=v) result.push(a[i]);
        move(a,result);
    }

    template <class T,class S>
    int first_index_of(narray<T> &a,S target) {
        for(int i=0;i<a.length();i++)
            if(a(i)==target) return i;
        return -1;
    }

    template <class T>
    static inline void insert_at(narray<T> &a,int i) {
        a.push();
        int n = a.length();
        for(int j=n-1;j>i;j--) a[j] = a[j-1];
    }

    template <class T>
    static inline void delete_at(narray<T> &a,int i) {
        int n = a.length();
        for(int j=i+1;j<n;j++) a[j-1] = a[j];
        a.pop();
    }

    template <class T>
    static inline void insert_at(narray<T> &a,int i,T value) {
        a.push();
        int n = a.length();
        for(int j=n-1;j>i;j--) a[j] = a[j-1];
        a[i] = value;
    }

    /// Temporary arrays

    template <class T>
    class check_narray_nomod {
        narray<T> &arg;
#ifndef UNSAFE
        narray<T> temp;
#endif
    public:
        check_narray_nomod(narray<T> &arg):arg(arg) {
#ifndef UNSAFE
            copy(temp,arg);
#endif
        }
        ~check_narray_nomod() {
#ifndef UNSAFE
            ASSERT(equal(temp,arg));
#endif
        }
        operator narray<T> &() {
            return arg;
        }
    };

    template <class T>
    inline T clamp(T x,T lo,T hi) {
        if(x<lo) return lo;
        if(x>=hi) return hi-1;
        return x;
    }

    template <class T,class U>
    inline void clampscale(narray<T> &out,narray<U> &in,T lo,T hi) {
        out.makelike(in);
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = clamp(256*(in.at1d()-lo)/(hi-lo),0,256);
    }

    template<class T,class U>
    bool contains_only(narray<T> &a, U value) {
        for(int i = 0; i < a.length1d(); i++) {
            if(a.at1d(i) != value)
                return false;
        }
        return true;
    }

    template<class T,class U,class V>
    bool contains_only(narray<T> &a, U value1, V value2) {
        for(int i = 0; i < a.length1d(); i++) {
            if(a.at1d(i) != value1 && a.at1d(i) != value2)
                return false;
        }
        return true;
    }

    template<class T>
    void get_dims(intarray &dims, narray<T> &a) {
        dims.resize(a.rank());
        for(int i = 0; i < dims.length(); i++)
            dims[i] = a.dim(i);
    }

    template<class T>
    void set_dims(narray<T> &a, intarray &dims) {
        switch(dims.length()) {
        case 0:
            a.dealloc();
            break;
        case 1:
            a.resize(dims[0]);
            break;
        case 2:
            a.resize(dims[0],dims[1]);
            break;
        case 3:
            a.resize(dims[0],dims[1],dims[2]);
            break;
        case 4:
            a.resize(dims[0],dims[1],dims[2],dims[3]);
            break;
        default:
            throw "bad rank";
        }
    }

}
#endif
