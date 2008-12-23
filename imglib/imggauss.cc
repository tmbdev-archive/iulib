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
// File: imggauss.cc
// Purpose: gaussian convolution
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

extern "C" {
#include <math.h>
}

#include "colib/colib.h"
#include "imglib.h"


using namespace colib;

namespace iulib {

    /// Perform 1D Gaussian convolutions using a FIR filter.
    ///
    /// The mask is computed to 3 sigma.

    template<class T>
    void gauss1d(narray<T> &out, narray<T> &in, float sigma) {
        out.resize(in.dim(0));
        // make a normalized mask
        int range = 1+int(3.0*sigma);
        floatarray mask(2*range+1);
        for (int i=0; i<=range; i++) {
            double y = exp(-i*i/2.0/sigma/sigma);
            mask(range+i) = mask(range-i) = y;
        }
        float total = 0.0;
        for (int i=0; i<mask.dim(0); i++)
            total += mask(i);
        for (int i=0; i<mask.dim(0); i++)
            mask(i) /= total;

        // apply it
        int n = in.length();
        for (int i=0; i<n; i++) {
            double total = 0.0;
            for (int j=0; j<mask.dim(0); j++) {
                int index = i+j-range;
                if (index<0)
                    index = 0;
                if (index>=n)
                    index = n-1;
                total += in(index) * mask(j); // it's symmetric
            }
            out(i) = T(total);
        }
    }

template     void gauss1d(bytearray &out, bytearray &in, float sigma);
template     void gauss1d(floatarray &out, floatarray &in, float sigma);

    /// Perform 1D Gaussian convolutions using a FIR filter.
    ///
    /// The mask is computed to 3 sigma.

    template<class T>
    void gauss1d(narray<T> &v, float sigma) {
        narray<T> temp;
        gauss1d(temp, v, sigma);
        move(v, temp);
    }

template         void gauss1d(bytearray &v, float sigma);
template         void gauss1d(floatarray &v, float sigma);

    /// Perform 2D Gaussian convolutions using a FIR filter.
    ///
    /// The mask is computed to 3 sigma.

    template<class T>
    void gauss2d(narray<T> &a, float sx, float sy) {
        floatarray r, s;
        for (int i=0; i<a.dim(0); i++) {
            getd0(a, r, i);
            gauss1d(s, r, sy);
            putd0(a, s, i);
        }
        for (int j=0; j<a.dim(1); j++) {
            getd1(a, r, j);
            gauss1d(s, r, sx);
            putd1(a, s, j);
        }
    }

template         void gauss2d(bytearray &image, float sx, float sy);
template         void gauss2d(floatarray &image, float sx, float sy);

}
