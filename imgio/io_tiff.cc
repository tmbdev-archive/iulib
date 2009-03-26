// -*- C++ -*-

// Copyright 2006-2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
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
// File: io_tiff.cc
// Purpose: read and write TIFF-format files using libtiff
// Responsible: remat
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colib/colib.h"
#include "io_tiff.h"

using namespace colib;

namespace iulib {

    void tiffErrorHandler(const char *module, const char *fmt, va_list ap) {
        char msg[1024];
        strncpy(msg, module, 1023);
        strncat(msg, ": ", 1020-strlen(msg));
        vsnprintf(msg+strlen(msg), 1023-strlen(msg), fmt, ap);
        throw msg;
    }

    Tiff::Tiff(const char* filename, const char* mode) {
        TIFFSetErrorHandler(tiffErrorHandler);
        tif = TIFFOpen(filename, mode);
    }

    Tiff::Tiff(FILE* file, const char* mode) {
        if(ftell(file) != 0) {
            throw "tiff: file offset not zero";
        }
        TIFFSetErrorHandler(tiffErrorHandler);
        tif = TIFFFdOpen(fileno(file), "abcdefgh.tiff", mode);
    }

    Tiff::~Tiff() {
        TIFFClose(tif);
    }

    int Tiff::numPages() {
        int n = 0;
        do{
            TIFFSetDirectory(tif, n);
            n++;
        } while(!TIFFLastDirectory(tif));
        return n;
    }

    void Tiff::getParams(uint32 *w, uint32 *h, short *orientation, short *channelSize, short *nChannels, tstrip_t* nStrips) {
        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, w);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, h);
        TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, nChannels);
        TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, channelSize);
        TIFFGetField(tif, TIFFTAG_ORIENTATION, orientation);
        *nStrips = TIFFNumberOfStrips(tif);
        if(*channelSize != 8) {
            throw "tiff: channel size other than 8 not supported";
        }
    }

    void Tiff::getPageRaw(bytearray &image, int page, bool gray) {
        uint32 w, h;
        short orientation, channelSize, nChannels;
        tstrip_t nStrips;
        TIFFSetDirectory(tif, page);
        getParams(&w, &h, &orientation, &channelSize, &nChannels, &nStrips);
        Buffer raster(w * h * nChannels);
        int s = 0;
        for(int i=0; i<nStrips; i++) {
            s += TIFFReadEncodedStrip(tif, i, raster.buf+s, -1);
        }

        if(gray) {
            image.renew(w, h);
        } else {
            image.renew(w, h, nChannels);
        }
        for(int x=0; x<w; x++) {
            for(int y=0; y<h; y++) {
                uint32 m = 0;
                for(int c=0; c<nChannels; c++) {
                    unsigned char v = raster.buf[(image.dim(1)-1-y)*w*nChannels+x*nChannels+c];
                    if(gray) {
                        m += v;
                    } else {
                        image(x, y, c) = v;
                    }
                }
                if(gray) {
                    image(x, y) = m / nChannels;
                }
            }
        }
    }

    void Tiff::getPage(bytearray &image, int page, bool gray) {
        uint32 w, h;
        short orientation, channelSize, nChannels;
        tstrip_t nStrips;
        TIFFSetDirectory(tif, page);
        getParams(&w, &h, &orientation, &channelSize, &nChannels, &nStrips);
        Buffer raster(w * h * sizeof(uint32));
        TIFFReadRGBAImage(tif, w, h, (uint32*)raster.buf);
        nChannels = 4; // -- read rgba => always four channels

        if(gray) {
            image.renew(w, h);
        } else {
            image.renew(w, h, nChannels);
        }
        for(int x=0; x<w; x++) {
            for(int y=0; y<h; y++) {
                uint32 m = 0;
                for(int c=0; c<nChannels; c++) {
                    unsigned char v = raster.buf[y*w*nChannels+x*nChannels+c];
                    if(gray) {
                        m += v;
                    } else {
                        image(x, y, c) = v;
                    }
                }
                if(gray) {
                    image(x, y) = m / nChannels;
                }
            }
        }
    }

    void Tiff::getPageRaw(intarray &image, int page, bool gray) {
        uint32 w, h;
        short orientation, channelSize, nChannels;
        tstrip_t nStrips;
        TIFFSetDirectory(tif, page);
        getParams(&w, &h, &orientation, &channelSize, &nChannels, &nStrips);
        if(nChannels > 4) {
            throw "tiff: more than 4 channels not supported for packed format";
        }
        Buffer raster(w * h * nChannels);
        int s = 0;
        for(int i=0; i<nStrips; i++) {
            s += TIFFReadEncodedStrip(tif, i, raster.buf+s, -1);
        }

        image.renew(w, h);
        for(int x=0; x<w; x++) {
            for(int y=0; y<h; y++) {
                image(x, y) = 0;
                for(int c=0; c<nChannels; c++) {
                    unsigned char v = raster.buf[(h-1-y)*w*nChannels+x*nChannels+c];
                    if(gray) {
                        image(x, y) += v;
                    } else {
                        image(x, y) |= v << ((2-c)*8);   // 2-c => 012 210
                    }
                }
                if(gray) {
                    image(x, y) /= nChannels;
                }
            }
        }
    }

    void Tiff::getPage(intarray &image, int page, bool gray) {
        uint32 w, h;
        short orientation, channelSize, nChannels;
        tstrip_t nStrips;
        TIFFSetDirectory(tif, page);
        getParams(&w, &h, &orientation, &channelSize, &nChannels, &nStrips);
        Buffer raster(w * h * sizeof(uint32));
        TIFFReadRGBAImage(tif, w, h, (uint32*)raster.buf);
        nChannels = 4; // -- read rgba => always four channels

        image.renew(w, h);
        for(int x=0; x<w; x++) {
            for(int y=0; y<h; y++) {
                image(x, y) = 0;
                for(int c=0; c<nChannels; c++) {
                    unsigned char v = raster.buf[y*w*nChannels+x*nChannels+c];
                    if(gray) {
                        image(x, y) += v;
                    } else {
                        image(x, y) |= v << ((2-c)*8);   // 2-c => 012 210
                    }
                }
                if(gray) {
                    image(x, y) /= nChannels;
                }
            }
        }
    }

    template<class T>
    void Tiff::setParams(narray<T> &image, int nChannels) {
        TIFFSetField(tif, TIFFTAG_IMAGEWIDTH, image.dim(0));
        TIFFSetField(tif, TIFFTAG_IMAGELENGTH, image.dim(1));
        TIFFSetField(tif, TIFFTAG_SAMPLESPERPIXEL, nChannels);
        TIFFSetField(tif, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tif, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
        TIFFSetField(tif, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
        if(nChannels == 1) {
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISBLACK);
        } else {
            TIFFSetField(tif, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);
        }
    }

    void Tiff::setPage(bytearray &image, int page) {
        TIFFSetDirectory(tif, page);
        short nChannels = max(image.dim(2), 1);
        setParams(image, nChannels);
        int w = image.dim(0);
        int h = image.dim(1);
        Buffer raster(w * h * nChannels);
        for(int x=0; x<image.dim(0); x++) {
            for(int y=0; y<image.dim(1); y++) {
                if(image.rank() > 2) {
                    for(int c=0; c<nChannels; c++) {
                        raster.buf[(h-1-y)*w*nChannels+x*nChannels+c] = image(x, y, c);
                    }
                } else {
                    raster.buf[(h-1-y)*w+x] = image(x, y);
                }
            }
        }
        TIFFWriteEncodedStrip(tif, 0, raster.buf, image.dim(0) * image.dim(1) * nChannels);
        TIFFWriteDirectory(tif);
    }

    void Tiff::setPage(intarray &image, int page) {
        TIFFSetDirectory(tif, page);
        short nChannels = 3;
        setParams(image, nChannels);
        int w = image.dim(0);
        int h = image.dim(1);
        Buffer raster(w * h * nChannels);
        for(int x=0; x<image.dim(0); x++) {
            for(int y=0; y<image.dim(1); y++) {
                uint32 v = image(x, y);
                for(int c=0; c<nChannels; c++) {
                    raster.buf[(h-1-y)*w*nChannels+x*nChannels+c] = (v >> (8*(nChannels-c-1))) & 0xFF;
                }
            }
        }
        TIFFWriteEncodedStrip(tif, 0, raster.buf, image.dim(0) * image.dim(1) * nChannels);
        TIFFWriteDirectory(tif);
    }

    void read_tiff(bytearray &image, const char *file, bool gray) {
        Tiff(file, "r").getPage(image, 0, gray);
    }
    void write_tiff(const char *file, bytearray &image) {
        Tiff(file, "w").setPage(image, 0);
    }
    void read_tiff_packed(intarray &image, const char *file, bool gray) {
        Tiff(file, "r").getPage(image, 0, gray);
    }
    void write_tiff_packed(const char *file, intarray &image) {
        Tiff(file, "w").setPage(image, 0);
    }

    void read_tiff(bytearray &image, FILE *file, bool gray) {
        Tiff(file, "r").getPage(image, 0, gray);
    }
    void write_tiff(FILE *file, bytearray &image) {
        Tiff(file, "w").setPage(image, 0);
    }
    void read_tiff_packed(intarray &image, FILE *file, bool gray) {
        Tiff(file, "r").getPage(image, 0, gray);
    }
    void write_tiff_packed(FILE *file, intarray &image) {
        Tiff(file, "w").setPage(image, 0);
    }
}
