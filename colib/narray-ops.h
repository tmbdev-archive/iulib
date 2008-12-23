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
// File: narray-ops.h
// Purpose:
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file narray-ops.h
/// \brief Mathematical operators on arrays

#ifndef h_narray_ops_
#define h_narray_ops_

#include <math.h>
#include <stdlib.h>
#include "colib/narray.h"

namespace narray_ops {

    namespace {
        template <class T>
        inline T abs_(T x) { return x>0?x:-x; }
        template <class T,class S>
        inline T max_(T x,S y) { return x>y?x:y; }
        template <class T,class S>
        inline T min_(T x,S y) { return x<y?x:y; }
    }

    template <class T,class S>
    void max(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = max_(out.at1d(i),in);
    }

    template <class T,class S>
    void min(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = min_(out.at1d(i),in);
    }

    template <class T,class S>
    void add(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) += in;
    }

    template <class T,class S>
    void operator+=(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) += in;
    }

    template <class T,class S>
    void sub(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) -= in;
    }

    template <class T,class S>
    void operator-=(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) -= in;
    }

    template <class T,class S>
    void sub(S in,colib::narray<T> &out) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = in - out.at1d(i);
    }

    template <class T,class S>
    void mul(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) *= in;
    }

    template <class T,class S>
    void operator*=(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) *= in;
    }

    template <class T,class S>
    void div(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) /= in;
    }

    template <class T,class S>
    void div(S value,colib::narray<T> &out) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = value / out.at1d(i);
    }

    template <class T,class S>
    void operator/=(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) /= in;
    }

    template <class T,class S>
    void pow(colib::narray<T> &out,S in) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = ::pow(out.at1d(i),in);
    }

    template <class T,class S>
    void max(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = max_(out.at1d(i),in.at1d(i));
    }

    template <class T,class S>
    void min(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = min_(out.at1d(i),in.at1d(i));
    }

    template <class T,class S>
    void add(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) += in.at1d(i);
    }

    template <class T,class S>
    void operator+=(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) += in.at1d(i);
    }

    template <class T,class S>
    void sub(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) -= in.at1d(i);
    }

    template <class T,class S>
    void operator-=(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) -= in.at1d(i);
    }

    template <class T,class S>
    void mul(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) *= in.at1d(i);
    }

    template <class T,class S>
    void operator*=(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) *= in.at1d(i);
    }

    template <class T,class S>
    void div(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) /= in.at1d(i);
    }

    template <class T,class S>
    void operator/=(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) /= in.at1d(i);
    }

    template <class T,class S>
    void pow(colib::narray<T> &out,colib::narray<S> &in) {
        CHECK_ARG(samedims(out,in));
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = ::pow(out.at1d(i),in.at1d(i));
    }

    template <class T,class S>
    void add(colib::narray<T> &out,colib::narray<S> &in1,colib::narray<S> &in2) {
        CHECK_ARG(samedims(in1,in2));
        makelike(out,in1);
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = in1.at1d(i) + in2.at1d(i);
    }

    template <class T,class S>
    void sub(colib::narray<T> &out,colib::narray<S> &in1,colib::narray<S> &in2) {
        CHECK_ARG(samedims(in1,in2));
        makelike(out,in1);
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = in1.at1d(i) - in2.at1d(i);
    }

    template <class T,class S>
    void mul(colib::narray<T> &out,colib::narray<S> &in1,colib::narray<S> &in2) {
        CHECK_ARG(samedims(in1,in2));
        makelike(out,in1);
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = in1.at1d(i) * in2.at1d(i);
    }

    template <class T,class S>
    void div(colib::narray<T> &out,colib::narray<S> &in1,colib::narray<S> &in2) {
        CHECK_ARG(samedims(in1,in2));
        makelike(out,in1);
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = in1.at1d(i) / in2.at1d(i);
    }

    template <class T,class S>
    void pow(colib::narray<T> &out,colib::narray<S> &in1,colib::narray<S> &in2) {
        CHECK_ARG(samedims(in1,in2));
        makelike(out,in1);
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = ::pow(in1.at1d(i),in2.at1d(i));
    }

    template <class T>
    void neg(colib::narray<T> &out) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = -out.at1d(i);
    }

    template <class T>
    void abs(colib::narray<T> &out) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = abs_(out.at1d(i));
    }

    template <class T>
    void log(colib::narray<T> &out) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = ::log(out.at1d(i));
    }

    template <class T>
    void exp(colib::narray<T> &out) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = ::exp(out.at1d(i));
    }

    template <class T>
    void sin(colib::narray<T> &out) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = ::sin(out.at1d(i));
    }

    template <class T>
    void cos(colib::narray<T> &out) {
        for(int i=0;i<out.length1d();i++)
            out.at1d(i) = ::cos(out.at1d(i));
    }

    template <class T,class S,class R>
    void greater(colib::narray<T> &out,S in,R no,R yes) {
        for(int i=0;i<out.length1d();i++)
            if(out.at1d(i)>in)
                out.at1d(i) = yes;
            else
                out.at1d(i) = no;
    }

    template <class T,class S,class R>
    void less(colib::narray<T> &out,S in,R no,R yes) {
        for(int i=0;i<out.length1d();i++)
            if(out.at1d(i)<in)
                out.at1d(i) = yes;
            else
                out.at1d(i) = no;
    }
}

#endif
