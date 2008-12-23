// -*- C++ -*-

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
// or its licensors, as applicable.
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
// File: imgedges.h
// Purpose: interface to imgedges.cc
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#ifndef h_imgedges__
#define h_imgedges__

#include "colib/colib.h"

namespace iulib {
    namespace {using namespace colib;}

    /// Compute raw edges from image, including non-maximum suppression.
    /// Pixels not corresponding to edges are set to 0, edge pixels
    /// are set to their gradient strength, which is always >0.
    void rawedges(colib::floatarray &edges, colib::floatarray &smoothed);

    /// Compute a fractile of the non-zero pixels in the image.
    float nonzero_fractile(colib::floatarray &edges, float frac, int nbins=1000);

    /// Perform non-maximum suppression
    void nonmaxsup(bytearray &out, floatarray &gradm, floatarray &gradx, floatarray &grady);

    /// Perform hysteresis thresholding of the image, using the given low and high thresholds.
    void hysteresis_thresholding(colib::floatarray &image, float lo, float hi);

    /// Compute gradient magnitude, horizontal, and vertical gradients.
    void gradients(floatarray &gradm,floatarray &gradx,floatarray &grady,floatarray &smoothed);
    /// Perform Canny edge detection.  The resulting array contains non-zero values
    /// at the locations of the Canny edge, with the value indicating the gradient
    /// magnitude.
    void canny(floatarray &gradm,floatarray &image,float sx,float sy,
               float frac=0.3,float tlow=2.0,float thigh=4.0);
}

#endif
