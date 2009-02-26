// -*- C++ -*-

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
// File: objlist.h
// Purpose: nicer interface for narray< autoref<T> >
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file objlist.h
/// \brief A nicer interface for an array of references.

#ifndef h_objlist_
#define h_objlist_

#include "colib/narray.h"
#include "smartptr.h"

namespace colib {

    /// \brief Wrapper for narray that makes lists of objects less painful.

    template <class T>
    class objlist {
    public:
        typedef autoref<T> Element;
        narray<Element> data;

        objlist() {
        }
        objlist(int n):data(n) {
        }

        int length() {
            return data.length();
        }
        int dim(int d) {
            ASSERT(d==0);
            return data.dim(d);
        }

        T &push() {
            return data.push().ref();
        }

        T &pop() {
            return data.pop().ref();
        }

        T &last() {
            return data.last().ref();
        }

        T &operator()(int i0) {
            return data[i0].ref();
        }

        T &operator[](int i0) {
            return data[i0].ref();
        }

        T &ref(int i0) {
            return data[i0].ref();
        }

        void set(int i0,T *p) {
            data(i0) = p;
        }

        T *ptr(int i0) {
            return data[i0].ptr();
        }

        T *move_ptr(int i0) {
            return data[i0].move();
        }
        void move(objlist<T> &other) {
            data.move(other.data);
        }

        void resize(int n) {
            data.dealloc();
            data.resize(n);
        }

        void clear() {
            for(int i=0;i<data.length();i++)
                data[i].dealloc();
            data.clear();
        }

        void dealloc() {
            data.dealloc();
        }

        void dealloc(int i) {
            data[i].dealloc();
        }
    };

    template <class T>
    void move(narray<T> &out,objlist<T> &in) {
        int n = in.length();
        out.resize(n);
        for(int i=0;i<n;i++)
            move(out[i],in[i]);
    }

    /// \brief Wrapper for narray that makes lists of objects less painful.

    template <class T>
    class ptrlist {
    public:
        typedef autodel<T> Element;
        narray<Element> data;

        int length() {
            return data.length();
        }

        void push(T *value) {
            data.push() = value;
        }
        void push(autodel<T> &value) {
            data.push() = value;
        }

        T &pop() {
            return data.pop().ref();
        }

        T &last() {
            return data.last().ref();
        }

        T &operator()(int i0) {
            return data[i0].ref();
        }

        T &operator[](int i0) {
            return data[i0].ref();
        }

        T &ref(int i0) {
            return data[i0].ref();
        }

        T *ptr(int i0) {
            return data[i0].ptr();
        }

        T *move(int i0) {
            return data[i0].move();
        }

        void resize(int n) {
            data.dealloc();
            data.resize(n);
        }

        void clear() {
            for(int i=0;i<data.length();i++)
                data[i].dealloc();
            data.clear();
        }

        void dealloc() {
            data.dealloc();
        }

        void dealloc(int i) {
            data[i].dealloc();
        }
    };
}

#endif
