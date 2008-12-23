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
// File: imgmisc.cc
// Purpose: misc operations (peaks, valleys, etc.)
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#include "colib/colib.h"
#include "imgmisc.h"
#include "imglib.h"

using namespace colib;

namespace iulib {

    /// Find local minima in a 1D array.

    void valleys(intarray &locations, floatarray &a, int minsize, int maxsize,
            float sigma) {
        locations.clear();
        floatarray v;
        copy(v, a);
        if (sigma>0)
            gauss1d(v, sigma);
        int i0 = max(1, minsize);
        int i1 = min(v.length()-1, maxsize);
        bool negslope = false; // Flag to check if a negative slope has occurred
        for (int i=i0; i<i1; i++) {
            if (v[i]<v[i-1])
                negslope = true;
            if (negslope && v[i]<v[i+1]) {
                locations.push(i);
                negslope = false;
            }
        }
    }

    /// Find local maxima in a 1D array.

    void peaks(intarray &locations, floatarray &a, int minsize, int maxsize,
            float sigma) {
        locations.clear();
        floatarray v;
        copy(v, a);
        if (sigma>0)
            gauss1d(v, sigma);
        int i0 = max(1, minsize);
        int i1 = min(v.length()-1, maxsize);
        bool posslope = false;
        for (int i=i0; i<i1; i++) {
            if (v[i]>v[i-1])
                posslope = true;
            if (posslope && v[i]>v[i+1]) {
                locations.push(i);
                posslope = false;
            }
        }
    }

    /// Compute the histogram of an image.

    void hist(floatarray &hist, bytearray &image) {
        hist.resize(256);
        fill(hist, 0);
        for (int i=0; i<image.length1d(); i++)
            hist(image.at1d(i))++;
    }

    // FIXME comments

    void binarize_by_threshold(bytearray &image) {
        int threshold = (min(image) + max(image)) / 2;
        for(int i = 0; i < image.length1d(); i++) {
            image.at1d(i) = image.at1d(i) < threshold ? 0 : 255;
        }
    }

}
