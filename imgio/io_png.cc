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
// File: io_png.cc
// Purpose: read and write PNG-format files using libpng
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

// Copyright (C) 2001 Leptonica. All rights reserved.
// This software is distributed in the hope that it will be
// useful, but with NO WARRANTY OF ANY KIND.
// No author or distributor accepts responsibility to anyone for the
// consequences of using this software, or for whether it serves any
// particular purpose or works at all, unless he or she says so in
// writing. Everyone is granted permission to copy, modify and
// redistribute this source code, for commercial or non-commercial
// purposes, with the following restrictions: (1) the origin of this
// source code must not be misrepresented; (2) modified versions must
// be plainly marked as such; and (3) this notice may not be removed
// or altered from any source or modified source distribution.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <png.h>
#include "colib/colib.h"


using namespace colib;

#ifndef NO_CONSOLE_IO
#define DEBUG 0
#endif /* ~NO_CONSOLE_IO */

#define ERROR(s) while(1) throw s;

namespace iulib {
    void read_png(bytearray &image,FILE *fp,bool gray=false) {
        int d, spp;
        int png_transforms;
        int num_palette;
        png_byte bit_depth, color_type, channels;
        int w, h, rowbytes;
        png_bytep rowptr;
        png_bytep *row_pointers;
        png_structp png_ptr;
        png_infop info_ptr, end_info;
        png_colorp palette;

        if(!fp)
            ERROR("fp not defined");

        // Allocate the 3 data structures
        if((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             (png_voidp)NULL, NULL, NULL)) == NULL)
            ERROR("png_ptr not made");

        if((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
            png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
            ERROR("info_ptr not made");
        }

        if((end_info = png_create_info_struct(png_ptr)) == NULL) {
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            ERROR("end_info not made");
        }

        // Set up png setjmp error handling

        if(setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            ERROR("internal png error");
        }

        png_init_io(png_ptr, fp);

        // Set the transforms flags. Whatever you do here,
        // DO NOT invert binary using PNG_TRANSFORM_INVERT_MONO!!
        // To remove alpha channel, use PNG_TRANSFORM_STRIP_ALPHA
        // To strip 16 --> 8 bit depth, use PNG_TRANSFORM_STRIP_16 */
        //#if 0 /* this does both */
        //        png_transforms = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA;
        //#else /* this just strips alpha */
        //        png_transforms = PNG_TRANSFORM_STRIP_ALPHA;
        //#endif
        png_transforms = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA
            | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND;

        // Do it!
        png_read_png(png_ptr, info_ptr, png_transforms, NULL);

        row_pointers = png_get_rows(png_ptr, info_ptr);
        w = png_get_image_width(png_ptr, info_ptr);
        h = png_get_image_height(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        channels = png_get_channels(png_ptr, info_ptr);

        spp = channels;

        if(spp == 1)
            d = bit_depth;
        else if(spp == 2) {
            d = 2 * bit_depth;
            ERROR("there shouldn't be 2 spp!");
        }
        else if(spp == 3)
            d = 4 * bit_depth;
        else { /* spp == 4 */
            d = 4 * bit_depth;
            ERROR("there shouldn't be 4 spp!");
        }

        /* Remove if/when this is implemented for all bit_depths */
        if(spp == 3 && bit_depth != 8) {
            fprintf(stderr, "Help: spp = 3 and depth = %d != 8\n!!", bit_depth);
            ERROR("not implemented for this depth");
        }

        narray<int> color_map;

        if(color_type == PNG_COLOR_TYPE_PALETTE ||
           color_type == PNG_COLOR_MASK_PALETTE) { /* generate a colormap */
            png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
            color_map.resize(3,num_palette);
            for(int cindex = 0; cindex < num_palette; cindex++) {
                color_map(0,cindex) = palette[cindex].red;
                color_map(1,cindex) = palette[cindex].green;
                color_map(2,cindex) = palette[cindex].blue;
            }
        }

        if(gray) image.resize(w,h);
        else image.resize(w,h,3);

        if(spp == 1) {
            CHECK_CONDITION(color_type!=PNG_COLOR_TYPE_PALETTE && color_type!=PNG_COLOR_MASK_PALETTE);
            CHECK_CONDITION(bit_depth==1 || bit_depth==8);
            for(int i = 0; i < h; i++) {
                rowptr = row_pointers[i];
                for(int j = 0; j < w; j++) {
                    int x = j;
                    int y = h-i-1;
                    int value;
                    if(bit_depth==1) {
                        value = (rowptr[j/8] & (128>>(j%8))) ? 255 : 0;
                    } else {
                        value = rowptr[j];
                    }
                    if(gray) {
                        image(x,y) = value;
                    } else {
                        image(x,y,0) = value;
                        image(x,y,1) = value;
                        image(x,y,2) = value;
                    }
                }
            }
        }
        else {
            CHECK_CONDITION(color_type!=PNG_COLOR_TYPE_PALETTE && color_type!=PNG_COLOR_MASK_PALETTE);
            CHECK_CONDITION(bit_depth == 8);
            for(int i = 0; i < h; i++) {
                rowptr = row_pointers[i];
                int k = 0;
                for(int j = 0; j < w; j++) {
                    int x = j;
                    int y = h-i-1;
                    if(gray) {
                        int value = rowptr[k++];
                        value += rowptr[k++];
                        value += rowptr[k++];
                        image(x,y) = value/3;
                    } else {
                        image(x,y,0) = rowptr[k++];
                        image(x,y,1) = rowptr[k++];
                        image(x,y,2) = rowptr[k++];
                    }
                }
            }
        }

        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    }


    void write_png(FILE *fp,bytearray &image) {
        int d;
        png_byte bit_depth, color_type;
        int w, h;
        png_structp png_ptr;
        png_infop info_ptr;
        unsigned int default_xres = 300;
        unsigned int default_yres = 300;

        int rank = image.rank();
        CHECK_ARG(image.rank()==2||(image.rank()==3 && image.dim(2)==3));

        if(!fp)
            ERROR("stream not open");

        /* Allocate the 2 data structures */
        if((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                              (png_voidp)NULL, NULL, NULL)) == NULL)
            ERROR("png_ptr not made");

        if((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
            png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
            ERROR("info_ptr not made");
        }

        /* Set up png setjmp error handling */
        if(setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_write_struct(&png_ptr, &info_ptr);
            ERROR("internal png error");
        }

        png_init_io(png_ptr, fp);

        w = image.dim(0);
        h = image.dim(1);
        d = image.dim(2);
        bit_depth = 8;
        color_type = PNG_COLOR_TYPE_RGB;

        png_set_IHDR(png_ptr, info_ptr, w, h, bit_depth, color_type,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                     PNG_FILTER_TYPE_BASE);
        png_set_pHYs(png_ptr, info_ptr, default_xres, default_yres,
                     PNG_RESOLUTION_METER);
        png_write_info(png_ptr, info_ptr);

        bytearray rowbuffer;
        rowbuffer.resize(3*w);
        for(int i = 0; i < h; i++) {
            int k = 0;
            for(int j = 0; j < w; j++) {
                int x = j;
                int y = h - i - 1;
                if(rank==2) {
                    int value = image(x,y);
                    rowbuffer(k++) = value;
                    rowbuffer(k++) = value;
                    rowbuffer(k++) = value;
                } else {
                    rowbuffer(k++) = image(x,y,0);
                    rowbuffer(k++) = image(x,y,1);
                    rowbuffer(k++) = image(x,y,2);
                }
            }

            png_byte *p = &rowbuffer(0);
            png_write_rows(png_ptr, &p, 1);
        }

        png_write_end(png_ptr, info_ptr);

        png_destroy_write_struct(&png_ptr, &info_ptr);
    }

    void read_png_packed(intarray &image,FILE *fp,bool gray=false) {
        int d, spp;
        int png_transforms;
        int num_palette;
        png_byte bit_depth, color_type, channels;
        int w, h, rowbytes;
        png_bytep rowptr;
        png_bytep *row_pointers;
        png_structp png_ptr;
        png_infop info_ptr, end_info;
        png_colorp palette;

        if(!fp)
            ERROR("fp not defined");

        /* Allocate the 3 data structures */
        if((png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,
                                             (png_voidp)NULL, NULL, NULL)) == NULL)
            ERROR("png_ptr not made");

        if((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
            png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
            ERROR("info_ptr not made");
        }

        if((end_info = png_create_info_struct(png_ptr)) == NULL) {
            png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
            ERROR("end_info not made");
        }

        /* Set up png setjmp error handling */
        if(setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
            ERROR("internal png error");
        }

        png_init_io(png_ptr, fp);

        /* Set the transforms flags. Whatever you do here,
         * DO NOT invert binary using PNG_TRANSFORM_INVERT_MONO!!
         * To remove alpha channel, use PNG_TRANSFORM_STRIP_ALPHA
         * To strip 16 --> 8 bit depth, use PNG_TRANSFORM_STRIP_16 */
        // this would does both
        // png_transforms = PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_STRIP_ALPHA;

        png_transforms = PNG_TRANSFORM_STRIP_ALPHA;

        /* Do it! */
        png_read_png(png_ptr, info_ptr, png_transforms, NULL);

        row_pointers = png_get_rows(png_ptr, info_ptr);
        w = png_get_image_width(png_ptr, info_ptr);
        h = png_get_image_height(png_ptr, info_ptr);
        bit_depth = png_get_bit_depth(png_ptr, info_ptr);
        rowbytes = png_get_rowbytes(png_ptr, info_ptr);
        color_type = png_get_color_type(png_ptr, info_ptr);
        channels = png_get_channels(png_ptr, info_ptr);

        spp = channels;

        if(spp == 1)
            d = bit_depth;
        else if(spp == 2) {
            d = 2 * bit_depth;
            ERROR("there shouldn't be 2 spp!");
        }
        else if(spp == 3)
            d = 4 * bit_depth;
        else { /* spp == 4 */
            d = 4 * bit_depth;
            ERROR("there shouldn't be 4 spp!");
        }

        /* Remove if/when this is implemented for all bit_depths */
        if(spp == 3 && bit_depth != 8) {
            fprintf(stderr, "Help: spp = 3 and depth = %d != 8\n!!", bit_depth);
            ERROR("not implemented for this depth");
        }

        narray<int> color_map;

        if(color_type == PNG_COLOR_TYPE_PALETTE ||
           color_type == PNG_COLOR_MASK_PALETTE) { /* generate a colormap */
            png_get_PLTE(png_ptr, info_ptr, &palette, &num_palette);
            color_map.resize(3,num_palette);
            for(int cindex = 0; cindex < num_palette; cindex++) {
                color_map(0,cindex) = palette[cindex].red;
                color_map(1,cindex) = palette[cindex].green;
                color_map(2,cindex) = palette[cindex].blue;
            }
        }

        image.resize(w,h);

        if(spp == 1) {
            CHECK_CONDITION(color_type!=PNG_COLOR_TYPE_PALETTE && color_type!=PNG_COLOR_MASK_PALETTE);
            CHECK_CONDITION(bit_depth==1 || bit_depth==8);
            for(int i = 0; i < h; i++) {
                rowptr = row_pointers[i];
                for(int j = 0; j < w; j++) {
                    int x = j;
                    int y = h-i-1;
                    int value;
                    if(bit_depth==1) {
                        value = 255 * !!(rowptr[j/8] & (128>>(j%8)));
                    } else {
                        value = rowptr[j];
                    }
                    image(x,y) = ((value<<16)|(value<<8)|value);
                }
            }
        }
        else {
            CHECK_CONDITION(color_type!=PNG_COLOR_TYPE_PALETTE && color_type!=PNG_COLOR_MASK_PALETTE);
            CHECK_CONDITION(bit_depth == 8);
            for(int i = 0; i < h; i++) {
                rowptr = row_pointers[i];
                int k = 0;
                for(int j = 0; j < w; j++) {
                    int x = j;
                    int y = h-i-1;
                    int r = rowptr[k++];
                    int g = rowptr[k++];
                    int b = rowptr[k++];
                    image(x,y) = ((r<<16)|(g<<8)|b);
                }
            }
        }

        /* use this to get resolution information
        xres = png_get_x_pixels_per_meter(png_ptr, info_ptr);
        yres = png_get_y_pixels_per_meter(png_ptr, info_ptr);
        pixSetXRes(pix, (int32)((float32)xres / 39.37 + 0.5)); // to ppi
        pixSetYRes(pix, (int32)((float32)yres / 39.37 + 0.5)); // to ppi
        */

        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
    }


    void write_png_packed(FILE *fp,intarray &image) {
        png_byte bit_depth, color_type;
        int w, h;
        png_structp png_ptr;
        png_infop info_ptr;
        unsigned int default_xres = 300;
        unsigned int default_yres = 300;

        CHECK_ARG(image.rank()==2||(image.rank()==3 && image.dim(2)==3));

        if(!fp)
            ERROR("stream not open");

        /* Allocate the 2 data structures */
        if((png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING,
                                              (png_voidp)NULL, NULL, NULL)) == NULL)
            ERROR("png_ptr not made");

        if((info_ptr = png_create_info_struct(png_ptr)) == NULL) {
            png_destroy_write_struct(&png_ptr, (png_infopp)NULL);
            ERROR("info_ptr not made");
        }

        /* Set up png setjmp error handling */
        if(setjmp(png_jmpbuf(png_ptr))) {
            png_destroy_write_struct(&png_ptr, &info_ptr);
            ERROR("internal png error");
        }

        png_init_io(png_ptr, fp);

        w = image.dim(0);
        h = image.dim(1);
        bit_depth = 8;
        color_type = PNG_COLOR_TYPE_RGB;

        png_set_IHDR(png_ptr, info_ptr, w, h, bit_depth, color_type,
                     PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE,
                     PNG_FILTER_TYPE_BASE);
        png_set_pHYs(png_ptr, info_ptr, default_xres, default_yres,
                     PNG_RESOLUTION_METER);
        png_write_info(png_ptr, info_ptr);

        bytearray rowbuffer;
        rowbuffer.resize(3*w);
        for(int i = 0; i < h; i++) {
            int k = 0;
            for(int j = 0; j < w; j++) {
                int x = j;
                int y = h - i - 1;
                int value = image(x,y);
                rowbuffer(k++) = (value>>16);
                rowbuffer(k++) = (value>>8);
                rowbuffer(k++) = value;
            }

            png_byte *p = &rowbuffer(0);
            png_write_rows(png_ptr, &p, 1);
        }

        png_write_end(png_ptr, info_ptr);

        png_destroy_write_struct(&png_ptr, &info_ptr);
    }
}
