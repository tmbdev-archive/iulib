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
// File: imgbrushfire.h
// Purpose: interfaces to imgbrushfire.cc
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#ifndef h_imgbrushfire__
#define h_imgbrushfire__

#include "colib/colib.h"

namespace iulib {
    /// Fill `distance' with approximate distances to the cloud of points (Euclidean metric).
    /// @param distance - for input: a rectangular array, nonzero means presense of a point.
    ///                   for output: an array of distances to that cloud of points.
    void brushfire_2(colib::floatarray &distance, float maxdist=1e30);
    /// Same as brushfire_2(), but with sum-abs (1-norm) metric.
    void brushfire_1(colib::floatarray &distance, float maxdist=1e30);
    /// Same as brushfire_2(), but with max-abs (infinity-norm) metric.
    void brushfire_inf(colib::floatarray &distance, float maxdist=1e30);

    /// Perform distance transform, returning not only the distances
    /// but also closest points in the cloud for every pixel in the image.
    void brushfire_2(colib::floatarray &distance, colib::narray<colib::point> &source,
            float maxdist=1e30);
    void brushfire_1(colib::floatarray &distance, colib::narray<colib::point> &source,
            float maxdist=1e30);
    void brushfire_inf(colib::floatarray &distance, colib::narray<colib::point> &source,
            float maxdist=1e30);

    /// Perform distance transform, return the closest point coordinates as a WxHx2 array,
    /// where the closest cloud point for the (x,y) pixel has ((x,y,0), (x,y,1)) coordinates.
    void brushfire_2(colib::floatarray &distance, colib::intarray &source, float maxdist=1e30);
    void brushfire_1(colib::floatarray &distance, colib::intarray &source, float maxdist=1e30);
    void brushfire_inf(colib::floatarray &distance, colib::intarray &source, float maxdist=1e30);

    /// Dilate with a diamond (metric figure of 1-norm). Uses distance transform.
    void dilate_1(colib::floatarray &image, float r);

    /// Dilate with a circle (metric figure of 2-norm).    Uses distance transform.
    void dilate_2(colib::floatarray &image, float r);

    /// Dilate with a square (metric figure of infinity-norm).    Uses distance transform.
    void dilate_inf(colib::floatarray &image, float r);

    /// Erode with a diamond (metric figure of 1-norm).    Uses distance transform.
    void erode_1(colib::floatarray &image, float r);

    /// Erode with a circle (metric figure of 2-norm).    Uses distance transform.
    void erode_2(colib::floatarray &image, float r);

    /// Erode with a square (metric figure of infinity-norm).   Uses distance transform.
    void erode_inf(colib::floatarray &image, float r);

    void brushfire_inf_scaled(colib::floatarray &distance, colib::narray<colib::point> &source,
            float sx, float sy, float maxdist=1e38);
    void brushfire_inf_scaled(colib::floatarray &distance, float sx, float sy,
            float maxdist=1e38);
    void brushfire_2_scaled(colib::floatarray &distance, colib::narray<colib::point> &source,
            float a, float b, float c, float d, float maxdist=1e38);
    void brushfire_2_scaled(colib::floatarray &distance, float a, float b, float c,
            float d, float maxdist=1e38);
}

#endif
