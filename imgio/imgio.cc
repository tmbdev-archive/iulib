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
// File: imgio.cc
// Purpose: reading image files determining their format automatically
// Responsible: mezhirov
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

#include "imgio.h"


using namespace colib;

namespace iulib {

    bool is_pnm(FILE *in) {
        int magic1 = fgetc(in);
        int magic2 = fgetc(in);
        rewind(in);
        return magic1 == 'P' && magic2 >= '1' && magic2 <= '6';
    }

    bool is_png(FILE *in) {
        char magic[] = {137, 80, 78, 71, 13, 10, 26, 10};
        char buf[sizeof(magic)];
        if(fread(buf, 1, sizeof(buf), in)!=sizeof(buf))
	    buf[0] = 0;
        rewind(in);
        return !memcmp(magic, buf, sizeof(buf));
    }

    bool is_jpeg(FILE *in) {
        int magic1 = fgetc(in);
        int magic2 = fgetc(in);
        rewind(in);
        return magic1 == 0xff && magic2 == 0xd8;
    }

    const char *ext_fmt(const char *filename) {
        int n = strlen(filename);
        if(n>=5) {
            if(!strcasecmp(filename+(n-5),".jpeg")) return "jpg";
            if(!strcasecmp(filename+(n-5),".tiff")) return "tif";
        }
        if(n>=4) {
            if(!strcasecmp(filename+(n-4),".jpg")) return "jpg";
            if(!strcasecmp(filename+(n-4),".png")) return "png";
            if(!strcasecmp(filename+(n-4),".pbm")) return "pnm";
            if(!strcasecmp(filename+(n-4),".pgm")) return "pnm";
            if(!strcasecmp(filename+(n-4),".ppm")) return "pnm";
            if(!strcasecmp(filename+(n-4),".pnm")) return "pnm";
            if(!strcasecmp(filename+(n-4),".tif")) return "tif";
        }
        static char error[1024];
        snprintf(error,1020,"%s: file has an unknown extension",filename);
        throw error;
    }

    const char *content_fmt(FILE *f) {
        if(!f) throw "unknown format (file descriptor not available)";
        if(is_jpeg(f)) return "jpg";
        if(is_png(f)) return "png";
        if(is_pnm(f)) return "pnm";
        throw "unknown format (file contents)";
    }

    const char *spec_fmt(const char *format) {
        if(!strcasecmp(format,"jpg")) return "jpg";
        if(!strcasecmp(format,"jpeg")) return "jpg";
        if(!strcasecmp(format,"png")) return "png";
        if(!strcasecmp(format,"pbm")) return "pnm";
        if(!strcasecmp(format,"pgm")) return "pnm";
        if(!strcasecmp(format,"ppm")) return "pnm";
        if(!strcasecmp(format,"pnm")) return "pnm";
        throw "unknown format (required format)";
    }

    const char *spec_or_content(const char *spec,FILE *f) {
        if(spec) return spec_fmt(spec);
        return content_fmt(f);
    }

    // reading images from file descriptors

    void read_image_packed(intarray &image,FILE *f,const char *format) {
        format = spec_or_content(format,f);
        if(!strcmp(format,"jpg")) read_jpeg_packed(image,f);
        else if(!strcmp(format,"png")) read_png_packed(image,f,false);
        else if(!strcmp(format,"pnm")) read_ppm_packed(f,image);
        else if(!strcmp(format,"tif")) read_tiff_packed(image,f,false);
        else throw "unknown format";
    }

    void read_image_rgb(bytearray &image,FILE *f,const char *format) {
        format = spec_or_content(format,f);
        if(!strcmp(format,"jpg")) read_jpeg_rgb(image,f);
        else if(!strcmp(format,"png")) read_png(image,f,false);
        else if(!strcmp(format,"pnm")) read_ppm_rgb(f,image);
        else if(!strcmp(format,"tif")) read_tiff(image,f,false);
        else throw "unknown format";
    }

    void read_image_gray(bytearray &image,FILE *f,const char *format) {
        format = spec_or_content(format,f);
        if(!strcmp(format,"jpg")) read_jpeg_gray(image,f);
        else if(!strcmp(format,"png")) read_png(image,f,true);
        else if(!strcmp(format,"pnm")) read_pnm_gray(f,image);
        else if(!strcmp(format,"tif")) read_tiff(image,f,true);
        else throw "unknown format";
    }

    void read_image_binary(bytearray &image,FILE *stream,const char *format) {
        read_image_gray(image,stream,format);
        float threshold = (min(image)+max(image))/2.0;
        for(int i=0;i<image.length1d();i++)
            image.at1d(i) = (image.at1d(i)<threshold)?0:255;
    }

    // reading by path

    void read_image_packed(intarray &image,const char *path) {
        const char *format = ext_fmt(path);
        if(!strcmp(format,"jpg")) read_jpeg_packed(image,stdio(path,"rb"));
        else if(!strcmp(format,"png")) read_png_packed(image,stdio(path,"rb"),false);
        else if(!strcmp(format,"pnm")) read_ppm_packed(stdio(path,"rb"),image);
        else if(!strcmp(format,"tif")) read_tiff_packed(image, path);
        else throw "unknown format";
    }

    void read_image_rgb(bytearray &image,const char *path) {
        const char *format = ext_fmt(path);
        if(!strcmp(format,"jpg")) read_jpeg_rgb(image,stdio(path,"rb"));
        else if(!strcmp(format,"png")) read_png(image,stdio(path,"rb"),false);
        else if(!strcmp(format,"pnm")) read_ppm_rgb(stdio(path,"rb"),image);
        else if(!strcmp(format,"tif")) read_tiff(image,path,false);
        else throw "unknown format";
    }

    void read_image_gray(bytearray &image,const char *path) {
        const char *format = ext_fmt(path);
        if(!strcmp(format,"jpg")) read_jpeg_gray(image,stdio(path,"rb"));
        else if(!strcmp(format,"png")) read_png(image,stdio(path,"rb"),true);
        else if(!strcmp(format,"pnm")) read_pnm_gray(stdio(path,"rb"),image);
        else if(!strcmp(format,"tif")) read_tiff(image,path,true);
        else throw "unknown format";
    }

    void read_image_binary(bytearray &image,const char *path) {
        const char *format = ext_fmt(path);
        read_image_gray(image,stdio(path,"rb"),format);
        float threshold = (min(image)+max(image))/2.0;
        for(int i=0;i<image.length1d();i++)
            image.at1d(i) = (image.at1d(i)<threshold)?0:255;
    }

    // writing to file descriptors

    void write_image_packed(FILE *f,intarray &image,const char *format) {
        CHECK_ARG2(f!=0,"null file argument");
        format = spec_fmt(format);
        if(!strcmp(format,"jpg")) throw "jpeg writing unimplemented"; //FIXME
        else if(!strcmp(format,"png")) write_png_packed(f,image);
        else if(!strcmp(format,"pnm")) write_ppm_packed(f,image);
        else throw "unknown format";
    }

    void write_image_rgb(FILE *f,bytearray &image,const char *format) {
        CHECK_ARG2(f!=0,"null file argument");
        format = spec_fmt(format);
        if(!strcmp(format,"jpg")) throw "jpeg writing unimplemented"; //FIXME
        else if(!strcmp(format,"png")) write_png(f,image);
        else if(!strcmp(format,"pnm")) write_ppm_rgb(f,image);
        else throw "unknown format";
    }

    void write_image_gray(FILE *f,bytearray &image,const char *format) {
        CHECK_ARG2(f!=0,"null file argument");
        CHECK_ARG(image.rank()==2);
        format = spec_fmt(format);
        if(!strcmp(format,"jpg")) throw "jpeg writing unimplemented"; //FIXME
        else if(!strcmp(format,"png")) write_png(f,image);
        else if(!strcmp(format,"pnm")) write_pgm(f,image);
        else throw "unknown format";
    }

    void write_image_binary(FILE *stream,bytearray &image,const char *format) {
        CHECK_ARG2(stream!=0,"null file argument");
        CHECK_ARG(image.rank()==2);
        bytearray temp(image.dim(0),image.dim(1));
        float threshold = (min(image)+max(image))/2.0;
        for(int i=0;i<image.length1d();i++)
            temp.at1d(i) = (image.at1d(i)<threshold)?0:255;
        format = spec_fmt(format);
        if(!strcmp(format,"jpg")) throw "jpeg writing unimplemented"; //FIXME
        else if(!strcmp(format,"png")) write_png(stream,image);
        else if(!strcmp(format,"pnm")) write_pbm(stream,image);
        else throw "unknown format";
    }

    // writing by path

    void write_image_packed(const char *path,intarray &image) {
        CHECK_ARG2(path!=0,"null file argument");
        write_image_packed(stdio(path,"wb"),image,ext_fmt(path));
    }

    void write_image_rgb(const char *path,bytearray &image) {
        CHECK_ARG2(path!=0,"null file argument");
        write_image_rgb(stdio(path,"wb"),image,ext_fmt(path));
    }

    void write_image_gray(const char *path,bytearray &image) {
        CHECK_ARG2(path!=0,"null file argument");
        write_image_gray(stdio(path,"wb"),image,ext_fmt(path));
    }

    void write_image_binary(const char *path,bytearray &image) {
        CHECK_ARG2(path!=0,"null file argument");
        write_image_binary(stdio(path,"wb"),image,ext_fmt(path));
    }
}
