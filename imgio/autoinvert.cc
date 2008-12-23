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
// File: autoinvert.cc
// Purpose: automatically invert document images to be white=0
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#include "colib/colib.h"

using namespace colib;

namespace iulib {
    void make_page_black(bytearray &image) {
        float mean = sum(image)*1.0/image.length1d();
        if(mean>128) {
            for(int i=0;i<image.length1d();i++)
                image.at1d(i) = 255-image.at1d(i);
        }
    }
    void make_page_normalized_and_black(bytearray &image) {
        float mn = min(image);
        float mx = max(image);
        float denom = mx - mn;
        if(denom==0.0f) denom = 1.0f;
        for(int i=0;i<image.length1d();i++)
            image.at1d(i) = int((image.at1d(i)-mn) * 255.0 / denom + 0.5);
        
        float mean = sum(image)*1.0/image.length1d();
        if(mean>128) {
            for(int i=0;i<image.length1d();i++)
                image.at1d(i) = 255-image.at1d(i);
        }
    }
    void make_page_binary_and_black(bytearray &image) {
        int nonzero = 0;
        float v0 = min(image);
        float v1 = max(image);
        float threshold = (v1+v0)/2;
        for(int i=0;i<image.length1d();i++)
            if(image.at1d(i)) nonzero++;
        if(nonzero>image.length1d()/2) {
            for(int i=0;i<image.length1d();i++)
                image.at1d(i) = (image.at1d(i) > threshold) ? 0: 255;
        } else {
            for(int i=0;i<image.length1d();i++)
                image.at1d(i) = (image.at1d(i) < threshold) ? 0: 255;
        }
    }
    void make_page_binary_and_black(intarray &image) {
        int nonzero = 0;
        for(int i=0;i<image.length1d();i++)
            if(image.at1d(i)) nonzero++;
        if(nonzero>image.length1d()/2) {
            for(int i=0;i<image.length1d();i++)
                image.at1d(i) = 0xffffff * !image.at1d(i);
        } else {
            for(int i=0;i<image.length1d();i++)
                image.at1d(i) = 0xffffff * !!image.at1d(i);
        }
    }
}
