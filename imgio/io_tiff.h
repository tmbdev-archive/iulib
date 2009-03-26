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
// File: io_tiff.h
// Purpose: interface to io_tiff.cc
// Responsible: remat
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

#ifndef h_tiff__
#define h_tiff__

#include "colib/colib.h"
#include "iulib.h"
#include "tiff.h"
#include "tiffio.h"

namespace iulib {
    using namespace colib;

    /**
     * @brief reading and writing TIFF images
     */
    class Tiff {
    public:
        Tiff(const char* filename, const char* mode);
        Tiff(FILE* file, const char* mode);
        ~Tiff();
        ///@return number of pages inside the tiff file
        int numPages();
        /// reads page in original color format
        void getPageRaw(bytearray &image, int page, bool gray=true);
        /// reads page in original color format
        void getPageRaw(intarray &image, int page, bool gray=false);
        /// reads page in RGBA color format
        void getPage(bytearray &image, int page, bool gray=true);
        /// reads page in RGBA color format
        void getPage(intarray &image, int page, bool gray=false);
        /// writes page in RGBA color format
        void setPage(bytearray &image, int page);
        /// writes page in RGBA color format
        void setPage(intarray &image, int page);
    private:
        void getParams(uint32 *w, uint32 *h, short *orientation, short *channelSize, short *nChannelss, tstrip_t* nStrips);
        template<class T>
        void setParams(narray<T> &image, int nChannels);
        TIFF* tif;
        /**
         * @brief simple autofree buffer using libtiff's malloc and free
         */
        class Buffer {
        public:
            Buffer(int size) : buf(NULL) {
                buf = (unsigned char*) _TIFFmalloc(size);
            }
            ~Buffer() {
                if(buf) _TIFFfree(buf);
            }
            unsigned char *buf;
        };
    };

    void read_tiff(colib::bytearray &image, const char *file, bool gray=true);
    void write_tiff(const char *file, colib::bytearray &image);
    void read_tiff_packed(colib::intarray &image, const char *file, bool gray=false);
    void write_tiff_packed(const char *file, colib::intarray &image);

    void read_tiff(colib::bytearray &image, FILE *file, bool gray=true);
    void write_tiff(FILE *file, colib::bytearray &image);
    void read_tiff_packed(colib::intarray &image, FILE *file, bool gray=false);
    void write_tiff_packed(FILE *file, colib::intarray &image);

}

#endif
