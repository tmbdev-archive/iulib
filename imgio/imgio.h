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
// File: imgio.cc
// Purpose: reading image files determining their format automatically
// Responsible: mezhirov
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

#ifndef h_imgio_
#define h_imgio_

#include "io_png.h"
#include "io_pbm.h"
#include "io_jpeg.h"
#include "io_tiff.h"
#include "autoinvert.h"

namespace iulib {
    namespace {using namespace colib;}

    // Read images from streams.  The format can either be given, or
    // it is inferred from the stream.

    void read_image_packed(intarray &image, FILE *f,const char *fmt=0);
    void read_image_rgb(bytearray &image, FILE *f,const char *fmt=0);
    void read_image_gray(bytearray &image, FILE *f,const char *fmt=0);
    void read_image_binary(bytearray &image, FILE *f, const char *fmt=0);

    // Read images from files.  The format is inferred from the extension.

    void read_image_packed(intarray &,const char *path);
    void read_image_rgb(bytearray &, const char *path);
    void read_image_gray(bytearray &, const char *path);
    void read_image_binary(bytearray &, const char *path);

    // Write images to streams.  The desired output format must be specified.

    void write_image_packed(FILE *f,intarray &image, const char *fmt);
    void write_image_rgb(FILE *f,bytearray &image, const char *fmt);
    void write_image_gray(FILE *f,bytearray &image, const char *fmt);
    void write_image_binary(FILE *f,bytearray &image,const char *fmt);

    // Write images to files.  The format is inferred from the extension.

    void write_image_packed(const char *path,intarray &);
    void write_image_rgb(const char *path, bytearray &);
    void write_image_gray(const char *path,bytearray &);
    void write_image_binary(const char *path,bytearray &);
}

#endif
