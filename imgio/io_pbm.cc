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
// File: io_pbm.cc
// Purpose: reading and writing of NetPBM format files
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

extern "C" {
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
}

#include "colib/colib.h"
#include "io_pbm.h"


using namespace iulib;
using namespace colib;

namespace iulib {

    namespace {
        int scanint(FILE *file) {
            int value;
            if(fscanf(file,"%d",&value)!=1) throw "read_pnm: number format error in image";
            return value;
        }

        int getbyte(FILE *file) {
            int value = getc(file);
            if(value==-1) throw "read_pnm: image short";
            return value;
        }

        void read_pnm_header(FILE *stream,char &ptype,int &w,int &h,int &maxval) {
            char c;
            if(fscanf(stream,"%c%c",&c,&ptype)!=2) throw "no image found";
            if(c!='P') throw "not a pnm file";
            while(fscanf(stream," #%*[^\n]%c",&c)==1);
            if(fscanf(stream,"%d",&w)!=1) throw "format error (width)";
            if(ptype=='1' || ptype=='4') {
                maxval = 1;
                while(fscanf(stream," #%*[^\n]%c",&c)==1);
                if(fscanf(stream,"%d%c",&h,&c)!=2) throw "format error (height)";
            } else {
                while(fscanf(stream," #%*[^\n]%c",&c)==1);
                if(fscanf(stream,"%d",&h)!=1) throw "format error (height)";
                while(fscanf(stream," #%*[^\n]%c",&c)==1);
                if(fscanf(stream,"%d%c",&maxval,&c)!=2) throw "format error (maxval)";
            }
            if(c!=' ' && c!='\n' && c!='\r' && c!='#') throw "format error (header end)";
        }

        inline int safe_getc(FILE *stream) {
            int c = getc(stream);
            if(c==EOF) throw "read error";
            return c;
        }

        inline int safe_putc(char c,FILE *stream) {
            int result = putc(c,stream);
            if(result==EOF) throw "write error";
            return result;
        }
    }

    void read_pnm_gray(FILE *stream,bytearray &image) {
        char ptype;
        int w,h,maxval;
        read_pnm_header(stream,ptype,w,h,maxval);
        if(maxval<0||maxval>255) throw "cannot handle 16bpp PNM files yet";
        if(ptype=='1') {
            image.resize(w,h);
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int c;
                for(;;) {
                    c = safe_getc(stream);
                    if(c==' '||c=='\r'||c=='\t'||c=='\n') continue;
                    if(c=='0') {image(i,j) = 0; break; }
                    if(c=='1') {image(i,j) = 1; break; }
                    throw "P1: bad format";
                }
            }
        } else if(ptype=='2') {
            image.resize(w,h);
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int v;
                if(fscanf(stream,"%d",&v)!=1) throw "P2: bad format";
                image(i,j) = v;
            }
        } else if(ptype=='3') {
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int value = 0;
                for(int k=0;k<3;k++) {
                    int v;
                    if(fscanf(stream,"%d",&v)!=1) throw "P3: bad format";
                    value += v;
                }
                image(i,j) = value/3;
            }
        } else if(ptype=='4') {
            image.resize(w,h);
            int bit = 8;
            int c = 0;
            for(int j=h-1;j>=0;j--) {
                for(int i=0;i<w;i++) {
                    if(bit>7) {
                        bit=0;
                        c = safe_getc(stream); 
                    }
                    image(i,j) = (c&0x80)?0:255;
                    c<<=1; bit++;
                }
                if(bit%8!=0 && j) {
                    c = safe_getc(stream);
                    bit = 0;
                }
            }
        } else if(ptype=='5') {
            image.resize(w,h);
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int c = safe_getc(stream);
                image(i,j) = c;
            }
        } else if(ptype=='6') {
            image.resize(w,h);
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int c = safe_getc(stream) + safe_getc(stream) + safe_getc(stream);
                image(i,j) = c/3;
            }
        } else {
            throw "PNM: unknown type";
        }
    }

    void read_ppm(FILE *stream,bytearray &rimg,bytearray &gimg,bytearray &bimg) {
        char ptype;
        int w,h,maxval;
        read_pnm_header(stream,ptype,w,h,maxval);
        if(maxval<0||maxval>255) throw "cannot handle 16bpp PNM files yet";
        rimg.resize(w,h);
        gimg.resize(w,h);
        bimg.resize(w,h);
        if(ptype=='3') {
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int r,g,b;
                if(fscanf(stream,"%d %d %d",&r, &g, &b)!=3) throw "P3: bad format";
                rimg(i,j) = r;
                gimg(i,j) = g;
                bimg(i,j) = b;
            }
        } else if(ptype=='6') {
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int r = safe_getc(stream);
                int g = safe_getc(stream);
                int b = safe_getc(stream);
                rimg(i,j) = r;
                gimg(i,j) = g;
                bimg(i,j) = b;
            }
        } else {
            throw "PPM: unknown type.\n";
        }

    }

    void read_ppm_packed(FILE *stream,intarray &image) {
        char ptype;
        int w,h,maxval;
        read_pnm_header(stream,ptype,w,h,maxval);
        if(maxval<0||maxval>255) throw "cannot handle 16bpp PNM files yet";
        if(ptype=='3') {
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int r,g,b;
                if(fscanf(stream,"%d %d %d",&r, &g, &b)!=3) throw "P3: bad format";
                image(i,j) = (r<<16)|(g<<8)|(b);
            }
        } else if(ptype=='6') {
            image.resize(w,h);
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int r = safe_getc(stream);
                int g = safe_getc(stream);
                int b = safe_getc(stream);
                image(i,j) = (r<<16)|(g<<8)|(b);
            }
        } else {
            throw "PPM: unknown type.\n";
        }

    }

    void read_ppm_rgb(FILE *stream,bytearray &image) {
        char ptype;
        int w,h,maxval;
        read_pnm_header(stream,ptype,w,h,maxval);
        if(maxval<0||maxval>255) throw "cannot handle 16bpp PNM files yet";
        if(ptype=='3') {
            image.resize(w,h,3);
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int r,g,b;
                if(fscanf(stream,"%d %d %d",&r, &g, &b)!=3) throw "P3: bad format";
                image(i,j,0) = r;
                image(i,j,1) = g;
                image(i,j,2) = b;
            }
        } else if(ptype=='6') {
            image.resize(w,h,3);
            for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
                int r = safe_getc(stream);
                int g = safe_getc(stream);
                int b = safe_getc(stream);
                image(i,j,0) = r;
                image(i,j,1) = g;
                image(i,j,2) = b;
            }
        } else {
            throw "PPM: unknown type.\n";
        }

    }

    void write_pbm(FILE *stream,bytearray &image) {
        int w = image.dim(0), h = image.dim(1);
        fprintf(stream,"P4\n%d %d\n",w,h);
        int bit = 7;
        int c = 0;
        for(int j=h-1;j>=0;j--) {
            for(int i=0;i<w;i++) {
                if(image(i,j)) c |= (1<<bit);
                bit--;
                if(bit<0) {
                    safe_putc(c,stream);
                    c = 0;
                    bit = 7;
                }
            }
            if(bit!=7) {
                safe_putc(c,stream);
                c = 0;
                bit = 7;
            }
        }
        if(bit<7) safe_putc(c,stream);
    }


    void write_pgm(FILE *stream,bytearray &image) {
        int w = image.dim(0), h = image.dim(1);
        fprintf(stream,"P5\n%d %d\n%d\n",w,h,255);
        for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
            safe_putc(image(i,j),stream);
        }
    }
    
    void write_ppm(FILE *stream,bytearray &rimg,bytearray &gimg,bytearray &bimg) {
        int w = rimg.dim(0), h = rimg.dim(1);
        ASSERT(bimg.dim(0)==rimg.dim(0));
        ASSERT(bimg.dim(1)==rimg.dim(1));
        ASSERT(gimg.dim(0)==rimg.dim(0));
        ASSERT(gimg.dim(1)==rimg.dim(1));
        fprintf(stream,"P6\n%d %d\n%d\n",w,h,255);
        for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
            safe_putc(rimg(i,j),stream);
            safe_putc(gimg(i,j),stream);
            safe_putc(bimg(i,j),stream);
        }
    }

    void write_ppm_rgb(FILE *stream,bytearray &image) {
        int w = image.dim(0), h = image.dim(1);
        fprintf(stream,"P6\n%d %d\n%d\n",w,h,255);
        for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
            safe_putc(image(i,j,0),stream);
            safe_putc(image(i,j,1),stream);
            safe_putc(image(i,j,2),stream);
        }
    }

    void write_ppm_packed(FILE *stream,intarray &image) {
        int w = image.dim(0), h = image.dim(1);
        fprintf(stream,"P6\n%d %d\n%d\n",w,h,255);
        for(int j=h-1;j>=0;j--) for(int i=0;i<w;i++) {
            safe_putc(image(i,j)>>16,stream);
            safe_putc(image(i,j)>>8,stream);
            safe_putc(image(i,j),stream);
        }
    }

    struct Stdio {
        FILE *stream;
        operator FILE*() { return stream; }
        Stdio() {
            stream = 0;
        }
        Stdio(const char *file,const char *mode) {
            stream = fopen(file,mode);
            if(!stream) throw "open failed";
        }
        ~Stdio() {
            if(stream) {fclose(stream); stream = 0;}
        }
        void open(const char *file,const char *mode) {
            if(stream) {fclose(stream); stream = 0;}
            stream = fopen(file,mode);
            if(!stream) throw "open failed";
        }
        void close() {
            if(stream) {fclose(stream); stream = 0;}
        }
        void popen(const char *file,const char *mode) {
            if(stream) {fclose(stream); stream = 0;}
            stream = ::popen(file,mode);
            if(!stream) throw "open failed";
        }
        void pclose() {
            if(stream) {
                ::pclose(stream);
                stream = 0;
            }
        }
    };

    void read_pnm_gray(const char *file,bytearray &image) {
        read_pnm_gray(Stdio(file,"r"),image);
    }

    void read_ppm(const char *file,bytearray &r,bytearray &g,bytearray &b) {
        read_ppm(Stdio(file,"r"),r,g,b);
    }

    void read_ppm_packed(const char *file,intarray &image) {
        read_ppm_packed(Stdio(file,"r"),image);
    }

    void read_ppm_rgb(const char *file,bytearray &image) {
        read_ppm_rgb(Stdio(file,"r"),image);
    }

    void write_pbm(const char *file,bytearray &image) {
        write_pbm(Stdio(file,"w"),image);
    }

    void write_pgm(const char *file,bytearray &image) {
        write_pgm(Stdio(file,"w"),image);
    }

    void write_ppm(const char *file,bytearray &r,bytearray &g,bytearray &b) {
        write_ppm(Stdio(file,"w"),r,g,b);
    }

    void write_ppm_rgb(const char *file,bytearray &image) {
        write_ppm_rgb(Stdio(file,"w"),image);
    }

    void write_ppm_packed(const char *file,intarray &image) {
        write_ppm_packed(Stdio(file,"w"),image);
    }

    void display(bytearray &image) {
        Stdio stream;
        if(image.rank()==2) {
            stream.popen("display","w");
            write_pgm(stream,image);
            stream.pclose();
        } else {
            stream.popen("display","w");
            write_ppm_rgb(stream,image);
            stream.pclose();
        }
    }

    void display(intarray &image) {
        Stdio stream;
        stream.popen("display","w");
        write_ppm_packed(stream,image);
        stream.pclose();
    }


}
