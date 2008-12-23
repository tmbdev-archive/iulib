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
// File: io_pbm.h
// Purpose: interface to io_pbm.cc
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#ifndef h_pbm__
#define h_pbm__

#include "colib/colib.h"

namespace iulib {
    /// Read any pbm/pgm/ppm file as a grayscale image (image is left with rank=2).

    void read_pnm_gray(FILE *,colib::bytearray &image);

    /// Read any pbm/pgm/ppm file as a three separate color images.
        
    void read_ppm(FILE *,colib::bytearray &r,colib::bytearray &g,colib::bytearray &b);

    /// Read any pbm/pgm/ppm file as a single color image with RGB packed into 24 bits.
        
    void read_ppm_packed(FILE *,colib::intarray &image);

    /// Read any pbm/pgm/ppm file into a rank-3 image, with the last array dimension containing
    /// the RGB values.

    void read_ppm_rgb(FILE *,colib::bytearray &image);
    
    /// Write an image in pbm format (non-zero pixels are written as bit=1).

    void write_pbm(FILE *,colib::bytearray &image);

    /// Write any image in pgm format.
        
    void write_pgm(FILE *,colib::bytearray &image);

    /// Write three images as a color image in PPM format.
        
    void write_ppm(FILE *,colib::bytearray &r,colib::bytearray &g,colib::bytearray &b);

    /// Write a rank-3 image with RGB corresponding to the last dimension in PPM format.
        
    void write_ppm_rgb(FILE *,colib::bytearray &image);

    /// Write a color image with 24 bits per pixel packed into integers.
        
    void write_ppm_packed(FILE *,colib::intarray &image);

    void read_pnm_gray(const char *file,colib::bytearray &image);
    void read_ppm(const char *file,colib::bytearray &r,colib::bytearray &g,colib::bytearray &b);
    void read_ppm_packed(const char *file,colib::intarray &image);
    void read_ppm_rgb(const char *file,colib::bytearray &image);
    void write_pbm(const char *file,colib::bytearray &image);
    void write_pgm(const char *file,colib::bytearray &image);
    void write_ppm(const char *file,colib::bytearray &r,colib::bytearray &g,colib::bytearray &b);
    void write_ppm_rgb(const char *file,colib::bytearray &image);
    void write_ppm_packed(const char *file,colib::intarray &image);

    void display(colib::bytearray &image);
    void display(colib::intarray &image);
}

#endif
