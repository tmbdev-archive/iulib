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
// Project:
// File: 
// Purpose: 
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: 


#ifndef h_io_jpeg_
#define h_io_jpeg_

#include "colib/colib.h"

namespace iulib {
    void read_jpeg_rgb(colib::bytearray &a, FILE *infile);
    void read_jpeg_gray(colib::bytearray &a, FILE *f);
    void read_jpeg_packed(colib::intarray &a, FILE *f);

    inline void read_jpeg_gray(colib::bytearray &a, char *f) {
        read_jpeg_gray(a, colib::stdio(f, "r"));
    }
    inline void read_jpeg_packed(colib::intarray &a, char *f) {
        read_jpeg_packed(a, colib::stdio(f, "r"));
    }

    // FIXME add JPEG writing
}

#endif
