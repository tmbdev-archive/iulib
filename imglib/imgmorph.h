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
// File: imgmorph.h
// Purpose: interface to corresponding .cc file
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de
#ifndef h_imgmorph__
#define h_imgmorph__

#include "colib/colib.h"

namespace iulib {

    void make_binary(colib::bytearray &image);
    void check_binary(colib::bytearray &image);
    void binary_invert(colib::bytearray &image);
    void binary_autoinvert(colib::bytearray &image);
    void complement(colib::bytearray &image);
    void difference(colib::bytearray &image, colib::bytearray &image2, int dx, int dy);
    int maxdifference(colib::bytearray &image, colib::bytearray &image2, int cx=0, int cy=0);
    void binary_and(colib::bytearray &image, colib::bytearray &image2, int dx=0, int dy=0);
    void binary_or(colib::bytearray &image, colib::bytearray &image2, int dx=0, int dy=0);
    void binary_erode_circle(colib::bytearray &image, int r);
    void binary_dilate_circle(colib::bytearray &image, int r);
    void binary_open_circle(colib::bytearray &image, int r);
    void binary_close_circle(colib::bytearray &image, int r);
    void binary_erode_rect(colib::bytearray &image, int rw, int rh);
    void binary_dilate_rect(colib::bytearray &image, int rw, int rh);
    void binary_open_rect(colib::bytearray &image, int rw, int rh);
    void binary_close_rect(colib::bytearray &image, int rw, int rh);

}

#endif
