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
// File: imgfilters.h
// Purpose: interface to corresponding .cc file
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#ifndef h_imgfilters__
#define h_imgfilters__

#include "colib/colib.h"

namespace iulib {
    
    // FIXME: Output arguments should come before input arguments.
    void horn_riley_ridges(colib::floatarray &im,colib::floatarray &zero,
                           colib::floatarray &strength,colib::floatarray &angle);
    void plus_laplacian(colib::floatarray &result,colib::floatarray &image);
    void laplacian(colib::floatarray &result,colib::floatarray &image) DEPRECATED;
    void zero_crossings(colib::bytearray &result,colib::floatarray &image);
    void local_minima(colib::bytearray &result,colib::floatarray &image);
    void local_maxima(colib::bytearray &result,colib::floatarray &image);
    void gradient_based_corners(colib::floatarray &image);
    void kitchen_rosenfeld_corners(colib::floatarray &corners,colib::floatarray &image);
    void kitchen_rosenfeld_corners2(colib::floatarray &corners,colib::floatarray &image);
    void median_filter(colib::bytearray &image, int rx, int ry);

}

#endif
