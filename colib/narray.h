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
// File: narray.h
// Purpose: single and multidimensional arrays
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file narray.h
/// \brief Single and multidimensional arrays

#ifndef h_narray_
#define h_narray_

// FIXME fix these deprecations once and for all --tmb
#ifdef NARRAY_DEPRECATION
#define NARRAY_DEPRECATED __attribute__ ((deprecated))
#else
#define NARRAY_DEPRECATED
#endif

namespace colib {

    /// \brief Multidimensional array class.
    ///
    /// Arrays are 0-based and support up to four subscripts.
    /// Rank-1 arrays can also be treated as stacks or lists (using the append method).

    template <class T>
    class narray {
    private:
        template <class S>
        static inline void swap_(S &a,S &b) { S t = a; a = b; b = t; }

        narray(const narray<T> &);
        void operator=(const narray<T> &);

        // check that i is in [0,n-1]

        inline void check_range(int i,int n) const {
#ifndef UNSAFE
            if(unsigned(i)>=unsigned(n)) throw "narray: index out of range";
#endif
        }

        // check the given condition to be true and throw message as the
        // exception if it is not

        inline void check(bool condition,const char *message) const {
#ifndef UNSAFE
            if(!condition) throw message;
#endif
        }

        // check that the array is of rank 1

        inline void check_rank1() {
            check(dims[1]==0,"attempt to use narray list operation with rank!=1");
        }

        static inline double growth_factor() { return 1.5; }

        // round up to the next size (for exponential resizing)

        static inline int roundup_(int i) {
            int v = 1;
            while(v <= i) {
                v = (int)(v*growth_factor())+1;
            }
            return v;
        }

    public:

        // a pointer to the actual data being held
        T *data;
        // the total number of elements held by the pointer
        int allocated;
        // the total number of elements that are currently
        // considered accessible / initialized
        int total;
        // the individual dimensions of the array
        int dims[5];

        // compute the total number of elements in an array
        // of the given dimensions

        int total_(int d0,int d1=0,int d2=0,int d3=0) {
            return d0*(d1?d1:1)*(d2?d2:1)*(d3?d3:1);
        }

        // change the elements of the array

        void setdims_(int d0,int d1=0,int d2=0,int d3=0) {
            total = total_(d0,d1,d2,d3);
            dims[0] = d0; dims[1] = d1; dims[2] = d2; dims[3] = d3; dims[4] = 0;
            check(total<=allocated,"bad setdims_ (internal error)");
        }

        // allocate the elements of the array

        void alloc_(int d0,int d1=0,int d2=0,int d3=0) {
            total = total_(d0,d1,d2,d3);
            data = new T[total];
            allocated = total;
            dims[0] = d0; dims[1] = d1; dims[2] = d2; dims[3] = d3; dims[4] = 0;
        }

        /// Creates a rank-0, empty array.

        narray() {
            data = 0;
            for(int i=0;i<5;i++) dims[i] = 0;
            total = 0;
            allocated = 0;
        }

        /// Creates a rank 1 array with dimensions d0.

        narray(int d0) {
            alloc_(d0);
        }

        /// Creates a rank 2 array with dimensions d0 and d1.

        narray(int d0,int d1) {
            alloc_(d0,d1);
        }

        /// Creates a rank 3 array with dimensions d0, d1, and d2.

        narray(int d0,int d1,int d2) {
            alloc_(d0,d1,d2);
        }

        /// Creates a rank 4 array with dimensions d0, d1, d2, and d3.

        narray(int d0,int d1,int d2,int d3) {
            alloc_(d0,d1,d2,d3);
        }

        /// Deallocates all storage associated with this array.

        ~narray() {
            dealloc();
        }

        /// Deallocates all storage associated with this array.

        void dealloc() {
            if(data) {
                delete [] data;
                data = 0;
            }
            dims[0] = 0;
            total = 0;
            allocated = 0;
        }

        /// Truncates the array.

        void truncate(int d0) {
            check(d0<=dims[0] && dims[1]==0,"can only truncate 1D arrays to smaller arrays");
            setdims_(d0);
        }

        /// Resizes the array, possibly destroying any data previously held by it.

        void resize(int d0,int d1=0,int d2=0,int d3=0) {
            int ntotal = total_(d0,d1,d2,d3);
            if(ntotal>total) {
                delete [] data;
                alloc_(d0,d1,d2,d3);
            } else {
                setdims_(d0,d1,d2,d3);
            }
        }

        /// Resizes the array to the given size; this is guaranteed to reallocate
        /// the storage fresh.

        void renew(int d0,int d1=0,int d2=0,int d3=0) {
            dealloc();
            resize(d0,d1,d2,d3);
        }

        /// Reshapes the array; the new shape must have the same number of elements as before.

        void reshape(int d0,int d1=0,int d2=0,int d3=0) {
            int ntotal = total_(d0,d1,d2,d3);
            check(ntotal==total,"narray: bad reshape");
            dims[0] = d0; dims[1] = d1; dims[2] = d2; dims[3] = d3; dims[4] = 0;
        }

        /// Determine the rank of the array.

        int rank() const {
            for(int i=1;i<=4;i++)
                if(dims[i]==0) return i;
            return 0;
        }

        /// Determine the range of valid index for index number i.

        int dim(int i) const {
            check_range(i,4);
            return dims[i];
        }

        /// 1D subscripting.

        T &operator()(int i0) const {
            check(dims[1]==0,"narray: bad rank");
            check_range(i0,dims[0]);
            return data[i0];
        }

        /// 2D subscripting.

        T &operator()(int i0,int i1) const {
            check(dims[2]==0,"narray: bad rank");
            check_range(i0,dims[0]);
            check_range(i1,dims[1]);
            return data[i1+i0*dims[1]];
        }

        /// 3D subscripting.

        T &operator()(int i0,int i1,int i2) const {
            check(dims[3]==0,"narray: bad rank");
            check_range(i0,dims[0]);
            check_range(i1,dims[1]);
            check_range(i2,dims[2]);
            return data[(i1+i0*dims[1])*dims[2]+i2];
        }

        /// 4D subscripting.

        T &operator()(int i0,int i1,int i2,int i3) const {
            check_range(i0,dims[0]);
            check_range(i1,dims[1]);
            check_range(i2,dims[2]);
            check_range(i3,dims[3]);
            return data[((i1+i0*dims[1])*dims[2]+i2)*dims[3]+i3];
        }

        T &at(int i0) { return operator()(i0); }
        T &at(int i0,int i1) { return operator()(i0,i1); }
        T &at(int i0,int i1,int i2) { return operator()(i0,i1,i2); }
        T &at(int i0,int i1,int i2,int i3) { return operator()(i0,i1,i2,i3); }

        /// Unsafe 1D subscripting.

        T &unsafe_at(int i0) const {
            return data[i0];
        }

        /// Unsafe 2D subscripting.

        T &unsafe_at(int i0,int i1) const {
            return data[i1+i0*dims[1]];
        }

        /// Unsafe 3D subscripting.

        T &unsafe_at(int i0,int i1,int i2) const {
            return data[(i1+i0*dims[1])*dims[2]+i2];
        }

        /// Unsafe 4D subscripting.

        T &unsafe_at(int i0,int i1,int i2,int i3) const {
            return data[((i1+i0*dims[1])*dims[2]+i2)*dims[3]+i3];
        }

#ifdef NARRAY_ASSIGNMENT
        /// Initializing/setting the value.

        template <class S>
        void operator=(S value) {
            for(int i=0,n=length1d();i<n;i++)
                unsafe_at1d(i) = value;
        }
#endif

        /// Length of the array, viewed as a 1D array.

        int length1d() const {
            return total;
        }

        /// 1D subscripting (works for arrays of any rank).

        T &at1d(int i) const {
            check_range(i,total);
            return data[i];
        }

        /// Unsafe 1D subscripting (works for arrays of any rank).

        T &unsafe_at1d(int i) const {
            return data[i];
        }


        /// Make sure that the array has allocated room for at least
        /// n more elements.  However, these additional elements may
        /// not be accessible until the dimensions are changed
        /// (e.g., through push).
        ///
        /// This version returns the old data array, if reallocation occured.
        ///
        T *reserve_but_not_delete(int n) {
            int nallocated = total+n;
            if(nallocated<=allocated) return 0;
            nallocated = roundup_(nallocated);
            T *ndata = new T[nallocated];
            for(int i=0;i<total;i++) ndata[i] = data[i];
            T *old_data = data;
            data = ndata;
            allocated = nallocated;
            return old_data;
        }


        /// Make sure that the array has allocated room for at least
        /// n more elements.  However, these additional elements may
        /// not be accessible until the dimensions are changed
        /// (e.g., through push).

        void reserve(int n) {
            T *p = reserve_but_not_delete(n);
            if(p)
                delete [] p;
        }

        /// Make sure that the array is a 1D array capable of holding at
        /// least n elements.  This preserves existing data.

        void grow_to(int n) {
            check_rank1();
            if(n>allocated) reserve(n-total);
            total = dims[0] = n;
        }

        /// Equivalent to dim(0), but checks that the array has rank 1.

        int length() {
            check_rank1();
            return dims[0];
        }

        /// Append an element to a rank-1 array.

        void push(const T &value) {
            check_rank1();
            // we can't delete the old data here because `value' might be
            // pointing inside it. First, we need to copy the value. 
            T *old_data = reserve_but_not_delete(1);
            data[dims[0]++] = value;
            if(old_data)
                delete [] old_data;
            total = dims[0];
        }

        /// Append an element to a rank-1 array.

        void push(T &value) {
            check_rank1();
            // we can't delete the old data here because `value' might be
            // pointing inside it. First, we need to copy the value. 
            T *old_data = reserve_but_not_delete(1);
            data[dims[0]++] = value;
            if(old_data)
                delete [] old_data;
            total = dims[0];
        }

        /// Append an element to a rank-1 array.

        T &push() {
            check_rank1();
            reserve(1);
            T &result = data[dims[0]++];
            total = dims[0];
            return result;
        }

        /// Remove the last element of a rank-1 array (returns a reference).

        T &pop() {
            check_rank1();
            check(dims[0]>0,"pop of empty list");
            T &result = data[--dims[0]];
            total = dims[0];
            return result;
        }

        /// Return a reference to the last element of a rank-1 array.

        T &last() {
            check_rank1();
            check(dims[0]>0,"pop of empty list");
            return data[dims[0]-1];
        }

        /// Same as operator()(int).

        T &operator[](int i0) {
            check_rank1();
            check_range(i0,dims[0]);
            return data[i0];
        }

        /// Make the array empty, but don't deallocate the storage held by it.
        /// The clear() method is more efficient if you expect you will be reusing the
        /// storage in a loop, e.g.,
        ///
        /// narray<int> data;
        /// for(...) {
        ///     data.clear();
        ///     ...
        ///     for(...) data.push(...);
        ///     ...
        /// }
        ///
        /// If you want to deallocate the storage, call dealloc().

        void clear() {
            dims[0] = 0;
            dims[1] = 0;
            total = 0;
        }

        void set(T v0) {
            resize(1);
            at(0) = v0;
        }
        void set(T v0,T v1) {
            resize(2);
            at(0) = v0;
            at(1) = v1;
        }
        void set(T v0,T v1,T v2) {
            resize(3);
            at(0) = v0;
            at(1) = v1;
            at(2) = v2;
        }
        void set(T v0,T v1,T v2,T v3) {
            resize(4);
            at(0) = v0;
            at(1) = v1;
            at(2) = v2;
            at(3) = v3;
        }

        /// Take the data held by the src array and put it into the dest array.
        /// The src array is made empty in the proceess.  This is an O(1) operation.

        void move(narray<T> &src) {
            narray<T> &dest = *this;
            dest.dealloc();
            dest.data = src.data;
            for(int i=0;i<5;i++) dest.dims[i] = src.dims[i];
            dest.total = src.total;
            dest.allocated = src.allocated;
            src.data = 0;
            src.dealloc();
        }

        /// Swap the contents of the two arrays.

        void swap(narray<T> &src) {
            narray<T> &dest = *this;
            swap_(dest.data,src.data);
            for(int i=0;i<5;i++) swap_(dest.dims[i],src.dims[i]);
            swap_(dest.total,src.total);
            swap_(dest.allocated,src.allocated);
        }

        /// Copy the elements of the source array into the destination array,
        /// resizing if necessary.

        template <class S>
        void copy(const narray<S> &src) {
            narray<T> &dest = *this;
            dest.resize(src.dim(0),src.dim(1),src.dim(2),src.dim(3));
            int n = dest.length1d();
            for(int i=0;i<n;i++) dest.unsafe_at1d(i) = (T)src.unsafe_at1d(i);
        }
        /// Check whether two narrays have the same rank and sizes.

        template <class S>
        bool samedims(narray<S> &b) {
            narray<T> &a = *this;
            if(a.rank()!=b.rank()) return 0;
            for(int i=0;i<a.rank();i++) if(a.dim(i)!=b.dim(i)) return 0;
            return 1;
        }

        /// Make the first array have the same dimensions as the second array.

        template <class S>
        void makelike(narray<S> &b) {
            narray<T> &a = *this;
            int r = b.rank();
            switch(r) {
            case 0:
                a.dealloc();
                break;
            case 1:
                a.resize(b.dim(0));
                break;
            case 2:
                a.resize(b.dim(0),b.dim(1));
                break;
            case 3:
                a.resize(b.dim(0),b.dim(1),b.dim(2));
                break;
            case 4:
                a.resize(b.dim(0),b.dim(1),b.dim(2),b.dim(3));
                break;
            default:
                throw "bad rank";
            }
        }

        /// Check whether two narrays are equal (mostly for testing).

        bool equal(narray<T> &b) {
            narray<T> &a = *this;
            if(a.rank()!=b.rank()) return 0;
            for(int i=0;i<a.rank();i++) if(a.dim(i)!=b.dim(i)) return 0;
            int n = a.length1d();
            for(int i=0;i<n;i++) if(a.unsafe_at1d(i) != b.unsafe_at1d(i)) return 0;
            return 1;
        }

        /// Initializing/setting the value.

        template <class S>
        void fill(S value) {
            narray<T> &a = *this;
            for(int i=0,n=a.length1d();i<n;i++)
                a.unsafe_at1d(i) = value;
        }

        /// common combination

        template <class S,class U>
        void makelike(narray<S> &b,U value) {
            makelike(b);
            fill(value);
        }
    };

    typedef unsigned char byte;
    typedef narray<unsigned char> bytearray;
    typedef narray<short> shortarray;
    typedef narray<int> intarray;
    typedef narray<float> floatarray;
    typedef narray<double> doublearray;
    typedef narray<long>  longarray;

    // use the methods instead (fewer name conflicts)
    template <class T> NARRAY_DEPRECATED
    void move(narray<T> &dest,narray<T> &src) {dest.move(src);}
    template <class T> NARRAY_DEPRECATED
    void swap(narray<T> &dest,narray<T> &src) {dest.swap(src);}
    template <class T,class S> NARRAY_DEPRECATED
    void copy(narray<T> &dest,const narray<S> &src) {dest.copy(src);}
    template <class T,class S> NARRAY_DEPRECATED
    bool samedims(narray<T> &a,narray<S> &b) {return a.samedims(b);}
    template <class T,class S> NARRAY_DEPRECATED
    void makelike(narray<T> &a,narray<S> &b) {a.makelike(b);}
    template <class T> NARRAY_DEPRECATED
    bool equal(narray<T> &a,narray<T> &b) {return a.equal(b);}
    template <class T,class S> NARRAY_DEPRECATED
    void fill(narray<T> &a,S value) {a.fill(value);}
}

#endif
