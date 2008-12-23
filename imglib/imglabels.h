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
// File: imglabels.h
// Purpose: interface to corresponding .cc file
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#ifndef h_imglabels__
#define h_imglabels__

#include "colib/colib.h"

namespace iulib {

    /// Propagate labels across the entire image from a set of non-zero seeds.
    void propagate_labels(colib::intarray &image);

    void propagate_labels_to(colib::intarray &target,colib::intarray &seed);
    bool dontcare(int x);
    void remove_dontcares(colib::intarray &image);
    int renumber_labels(colib::intarray &image,int start);

    /// Label the connected components of an image.
    int label_components(colib::intarray &image,bool four_connected=false);

    void simple_recolor(colib::intarray &image);
    void bounding_boxes(colib::narray<colib::rectangle> &result,colib::intarray &image);

    /// bitreverse per channel to get some interesting colors for sequentially numbered regions

    inline int interesting_colors(int x) {
        int r = 0;
        int g = 0;
        int b = 0;
        for(int i=0;i<8;i++) {
            r = (r<<1) | (x&1); x >>= 1;
            g = (g<<1) | (x&1); x >>= 1;
            b = (b<<1) | (x&1); x >>= 1;
        }
        return (r<<16)|(g<<8)|b;
    }

}

#endif
