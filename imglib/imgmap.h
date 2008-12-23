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
// File: imgmap.h
// Purpose: interface to corresponding .cc file
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de
#ifndef h_imgmap__
#define h_imgmap__

#include "colib/colib.h"

namespace iulib {

    template<class T> void rotate_direct_sample(colib::narray<T> &out, colib::narray<T> &in,
            float angle, float cx, float cy);
    template<class T> void rotate_direct_interpolate(colib::narray<T> &out,
            colib::narray<T> &in, float angle, float cx, float cy);
    template<class T> void scale_sample(colib::narray<T> &out, colib::narray<T> &in,
            float sx, float sy);
    template<class T> void scale_sample(colib::narray<T> &out, colib::narray<T> &in, int nx,
            int ny);
    template<class T> void scale_interpolate(colib::narray<T> &out, colib::narray<T> &in,
            float sx, float sy);
    template<class T> void scale_interpolate(colib::narray<T> &out, colib::narray<T> &in,
            int nx, int ny);

}

#endif
