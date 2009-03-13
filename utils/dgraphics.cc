// -*- C++ -*-

// Copyright 2006-2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
//
// You may not use this file except under the terms of the accompanying license.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may
// obtain a copy of the License at http:  www.apache.org/licenses/LICENSE-2.0
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
// Web Sites: www.iupr.org, www.dfki.de

#include <SDL/SDL.h>
#include <SDL/SDL_gfxPrimitives.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_imageFilter.h>
#include <SDL/SDL_rotozoom.h>

#include <stdlib.h>
#include "colib/colib.h"
#include "imglib.h"
#include "imgio.h"
#include "dgraphics.h"

#include "SDL_lines.h"

using namespace colib;
using namespace iulib;

namespace iulib {
#if 0
    static SDL_Rect rect(int x,int y,int w,int h) {
        SDL_Rect r; r.x = x; r.y = y; r.w = w; r.h = h;
        return r;
    }
    static SDL_Color color(int r,int g,int b) {
        SDL_Color c; c.r = r; c.g = g; c.b = b;
        c.unused = 0; // to remove compiler warning
        return c;
    }
#endif
    static SDL_Rect rect(SDL_Surface *s) {
        SDL_Rect r; r.x = 0; r.y = 0; r.w = s->w; r.h = s->h;
        return r;
    }
    static void SDL_UpdateRect(SDL_Surface *screen, SDL_Rect r) {
        SDL_UpdateRect(screen,r.x,r.y,r.w,r.h);
    }
#if 0
    static void Update(SDL_Surface *screen) {
        SDL_UpdateRect(screen,0,0,screen->w,screen->h);
    }
#endif

    static void ArrayBlit(SDL_Surface *dst,SDL_Rect *r,bytearray &data,double angle,double zoom,int smooth) {
        SDL_Surface *input = SDL_CreateRGBSurfaceFrom(&data.at1d(0),data.dim(1),data.dim(0),
                8,data.dim(1),0xff,0xff,0xff,0x00);
        SDL_SetAlpha(input,SDL_SRCALPHA,SDL_ALPHA_OPAQUE);
        SDL_Surface *output = rotozoomSurface(input,angle,zoom,smooth);
        SDL_SetAlpha(output,SDL_SRCALPHA,SDL_ALPHA_OPAQUE);
        SDL_FreeSurface(input);
        SDL_Rect sr; sr.x = 0; sr.y = 0; sr.w = output->w; sr.h = output->h;
        SDL_BlitSurface(output,&sr,dst,r);
        SDL_FreeSurface(output);
    }

    static void ArrayBlit(SDL_Surface *dst,SDL_Rect *r,intarray &data,double angle,double zoom,int smooth) {
        SDL_Surface *input = SDL_CreateRGBSurfaceFrom(&data.at1d(0),data.dim(1),data.dim(0),
                32,data.dim(1)*4,0xff0000,0xff00,0xff,0x00);
        SDL_SetAlpha(input,SDL_SRCALPHA,SDL_ALPHA_OPAQUE);
        SDL_Surface *output = rotozoomSurface(input,angle,zoom,smooth);
        SDL_SetAlpha(output,SDL_SRCALPHA,SDL_ALPHA_OPAQUE);
        SDL_FreeSurface(input);
        SDL_Rect sr; sr.x = 0; sr.y = 0; sr.w = output->w; sr.h = output->h;
        SDL_BlitSurface(output,&sr,dst,r);
        SDL_FreeSurface(output);
    }

    static void ParseSpec(double &x0,double &y0,double &x1,double &y1,const char *spec) {
        while(*spec) {
            switch(*spec) {
                case 'a':
                    x1 = (x0+x1)/2;
                    y1 = (y0+y1)/2;
                    break;
                case 'b':
                    x0 = (x0+x1)/2;
                    y1 = (y0+y1)/2;
                    break;
                case 'c':
                    x1 = (x0+x1)/2;
                    y0 = (y0+y1)/2;
                    break;
                case 'd':
                    x0 = (x0+x1)/2;
                    y0 = (y0+y1)/2;
                    break;
                case 'x':
                    x1 = (x0+x1)/2;
                    break;
                case 'X':
                    x0 = (x0+x1)/2;
                    break;
                case 'y':
                    y1 = (y0+y1)/2;
                    break;
                case 'Y':
                    y0 = (y0+y1)/2;
                    break;
            }
            spec++;
        }
    }

    static const char *dgraphics;
    static bool active = true;
    static SDL_Surface *screen;

    void dinit(int w,int h,bool force) {
        if(getenv("dgeometry")) {
            sscanf(getenv("dgeometry"),"%dx%d",&w,&h);
        }
        dgraphics = getenv("dgraphics");
        if(force || dgraphics) {
            SDL_Init(SDL_INIT_EVERYTHING|SDL_INIT_NOPARACHUTE);
            screen = SDL_SetVideoMode(w,h,24,SDL_SWSURFACE|SDL_HWPALETTE);
            SDL_PixelFormat *fmt = screen->format;
            printf("%d\n",fmt->BitsPerPixel);
        }
        // dgraphics==1 means turn on everything
        // all other valuse are matched against sections
        // and result in things being off by default
        if(dgraphics && !strcmp(dgraphics,"1")) dgraphics = 0;
        if(!force && dgraphics)
            active = false;
        else
            active = true;
    }

    bool dactive() {
        return screen && active;
    }

    static const char *current_section;

    const char *dsection_set(const char *section) {
        const char *last = current_section;
        current_section = section;
        if(!dgraphics) {
            active = true;
        } else if(current_section) {
            active = strstr(current_section,dgraphics);
        }
        return last;
    }

    void dend() {
        dsection("none");
    }

    template <class T>
    void dshow(narray<T> &data,const char *spec,double angle,int smooth,int rgb) {
        int l = -1;
        if(data.rank()==1) {
            l = data.length();
            int s = (int)sqrt(l);
            data.reshape(s,s);
        }
        if(!dactive()) return;
        double x0=0,y0=0,x1=1,y1=1;
        ParseSpec(x0,y0,x1,y1,spec);
        SDL_Rect out = rect(screen);
        out.x = int(out.w * x0);
        out.y = int(out.h * y0);
        out.w = int(out.w * (x1-x0));
        out.h = int(out.h * (y1-y0));
        double xscale = out.w * 1.0 / data.dim(0);
        double yscale = out.h * 1.0 / data.dim(1);
        double scale = xscale<yscale?xscale:yscale;
        rgb = SDL_MapRGB(screen->format,((rgb&0xff0000)>>16),((rgb&0xff00)>>8),(rgb&0xff));
        SDL_FillRect(screen,&out,rgb);
        SDL_UpdateRect(screen,out.x,out.y,out.w,out.h);
        ArrayBlit(screen,&out,data,angle,scale,smooth);
        SDL_UpdateRect(screen,out.x,out.y,out.w,out.h);
        if(l>0) data.reshape(l);
    }

    void dline(int lx0,int ly0,int lx1,int ly1,int rgb,int w,int h,const char *spec) {
        if(!dactive()) return;
        static int count = 0;
        double x0=0,y0=0,x1=1,y1=1;
        ParseSpec(x0,y0,x1,y1,spec);
        SDL_Rect out = rect(screen);
        out.x = int(out.w * x0);
        out.y = int(out.h * y0);
        out.w = int(out.w * (x1-x0));
        out.h = int(out.h * (y1-y0));
        double xscale = out.w / float(w);
        double yscale = out.h / float(h);
        double scale = xscale<yscale?xscale:yscale;
        lx0 = int(lx0*scale)+out.x;
        ly0 = int((h-ly0-1)*scale)+out.y;
        lx1 = int(lx1*scale)+out.x;
        ly1 = int((h-ly1-1)*scale)+out.y;
        ::SDL_extensions::SDL_draw_line(screen,lx0,ly0,lx1,ly1,rgb);
        if(count++%1000==0) dflush();
    }

    void dflush() {
        SDL_Flip(screen);
    }

    template void dshow(narray<unsigned char> &data,const char *spec,double angle,int smooth,int rgb);
    template void dshow(narray<int> &data,const char *spec,double angle,int smooth,int rgb);
    void dshow(floatarray &data,const char *spec,double angle,int smooth,int rgb) {
        bytearray temp;
        copy(temp,data);
        dshow(temp,spec,angle,smooth,rgb);
    }
    void dshow_signed(floatarray &data,const char *spec,double angle,int smooth,int rgb) {
        intarray temp;
        float amin = min(data);
        float amax = max(data);
        float absmax = max(abs(amin),abs(amax));
        temp.resize(data.dim(0),data.dim(1));
        for(int i=0;i<data.length1d();i++) {
            int v = 255 * data.at1d(i)/absmax;
            if(v<0) temp.at1d(i) = (abs(v)<<16);
            else temp.at1d(i) = (abs(v)<<8);
        }
        dshow(temp,spec,angle,smooth,rgb);
    }
    void dshow_grid_signed(floatarray &data,int tw,const char *spec,double angle,int smooth,int rgb) {
        CHECK_ARG(data.rank()==3);
        int w = data.dim(1);
        CHECK_ARG(w>0);
        int h = data.dim(2);
        CHECK_ARG(h>0);
        int n = data.dim(0);
        CHECK_ARG(n>0);
        int ncol = int(tw/w);
        int nrow = (n+ncol-1)/ncol;
        intarray temp(ncol*w,nrow*h);
        temp.fill(0);
        float amin = min(data);
        float amax = max(data);
        float absmax = max(abs(amin),abs(amax));
        int x0 = 0;
        int y0 = 0;
        for(int k=0;k<n;k++) {
            for(int x=0;x<w;x++) {
                for(int y=0;y<h;y++) {
                    int v = 255 * data(k,x,y)/absmax;
                    if(v<0) temp(x0+x,y0+y) = (abs(v)<<16);
                    else temp(x0+x,y0+y) = (abs(v)<<8);
                }
            }
            x0 += w;
            if(x0+w>temp.dim(0)) {
                x0 = 0;
                y0 += h;
            }
        }
        dshow(temp,spec,angle,smooth,rgb);
    }

    template <class T>
    void dshown(narray<T> &data,const char *spec,double angle, int smooth, int rgb) {
        if(!dactive()) return;
        narray<T> temp;
        copy(temp,data);
        expand_range(temp,0,255);
        bytearray btemp;
        copy(btemp,temp);
        dshow(btemp,spec,angle,smooth,rgb);
    }
    template void dshown(narray<unsigned char> &data,const char *spec,double angle,int smooth,int rgb);
    template void dshown(narray<int> &data,const char *spec,double angle,int smooth,int rgb);
    template void dshown(narray<float> &data,const char *spec,double angle,int smooth,int rgb);

    void dshowr(intarray &data,const char *spec,double angle,int smooth,int rgb) {
        if(!dactive()) return;
        intarray temp;
        copy(temp,data);
        replace_values(temp,0xffffff,0);
        simple_recolor(temp);
        dshow(temp,spec,angle,smooth,rgb);
    }

    void dshow1d(floatarray &data,const char *spec) {
        int w = data.dim(0);
        int h = w/2;
        int h2 = h-2;
        bytearray temp(w,h);
        float amin = min(data);
        float amax = max(data);
        temp.fill(0);
        for(int i=0;i<w;i++) {
            temp(i,0) = 64;
            temp(i,h-1) = 64;
            int j1 = h2*(data[i]-amin)/(amax-amin);
            for(int j=1;j<=j1;j++) temp(i,j) = 255;
        }
        dshow(temp,spec);
    }

    void dclear(int rgb) {
        if(!dactive()) return;
        SDL_Rect r = rect(screen);
        rgb = SDL_MapRGB(screen->format,((rgb&0xff0000)>>16),((rgb&0xff00)>>8),(rgb&0xff));
        SDL_FillRect(screen,&r,rgb);
        SDL_UpdateRect(screen,r.x,r.y,r.w,r.h);
    }

    void dwait() {
        if(!dactive()) return;
        SDL_Event event;
        while(SDL_WaitEvent(&event)) {
            if(event.type==SDL_KEYDOWN &&
                event.key.keysym.sym==SDLK_ESCAPE) break;
            if(event.type==SDL_MOUSEBUTTONDOWN) break;
            if(event.type==SDL_QUIT) exit(0);
        }
    }
}
