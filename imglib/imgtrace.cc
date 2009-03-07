// -*- C++ -*-

// Copyright 2006-2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
// or its licensors,as applicable.
// Copyright 1995-2005 Thomas M. Breuel
// 
// You may not use this file except under the terms of the accompanying license.
// 
// Licensed under the Apache License,Version 2.0 (the "License"); you
// may not use this file except in compliance with the License. You may
// obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// 
// Project: iulib -- image understanding library
// File: imgtrace.cc
// Purpose: chain tracing and approximation
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org,www.dfki.de

extern "C" {
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
}

#include "colib/colib.h"
#include "imglib.h"


using namespace colib;

namespace colib {
    typedef narray<vec2> vec2array;
    template void reverse(vec2array &);
}

namespace iulib {


    inline float point_line_dist(vec2 p,vec2 a,vec2 b) {
        vec2 delta = b-a;
        float mag = delta.magnitude();
        // if the distance is small,just return the point distance;
        // that's the right thing for approx_chain
        if(mag<1e-4)
            return a.distance(p);
        vec2 normal = delta.normal()/mag;
        float offset = normal * a;
        return fabs(normal * p - offset);
    }

    static void approx_chain(intarray &breaks,vec2array &chain,int i0,
            int i1,float maxdist) {
        float md = 0.0;
        int mi = -1;
        vec2 a = chain[i0];
        vec2 b = chain[i1];
        for(int i=i0; i<=i1; i++) {
            float d = point_line_dist(chain[i],a,b);
            //printf("%d %g %g %g\n",i,d,a.distance(chain[i]),b.distance(chain[i]));
            if(d<=md)
                continue;
            md = d;
            mi = i;
        }
        //printf("i0 %d i1 %d mi %d md %g maxdist %g\n",i0,i1,mi,md,maxdist);
        if(md<maxdist)
            return;
        ASSERT(mi!=i0 && mi!=i1);
        approx_chain(breaks,chain,i0,mi,maxdist);
        breaks.push(mi);
        approx_chain(breaks,chain,mi,i1,maxdist);
    }

    struct ChainTracer : IChainTracer {
        enum {OFF=0,ON=1,DONE=2};

        bytearray bi;
        int sx,sy;
        int x,y;
        int w,h;

        int count_neighbors() {
            int nn=0;
            if(bi(x+1,y)) nn++;
            if(bi(x+1,y+1)) nn++;
            if(bi(x,y+1)) nn++;
            if(bi(x-1,y+1)) nn++;
            if(bi(x-1,y)) nn++;
            if(bi(x-1,y-1)) nn++;
            if(bi(x,y-1)) nn++;
            if(bi(x+1,y-1)) nn++;
            return nn;
        }

        bool nextpixel() {
            if(bi(x+1,y)==ON) { x=x+1; y=y; return true; }
            if(bi(x+1,y+1)==ON) { x=x+1; y=y+1; return true; }
            if(bi(x,y+1)==ON) { x=x; y=y+1; return true; }
            if(bi(x-1,y+1)==ON) { x=x-1; y=y+1; return true; }
            if(bi(x-1,y)==ON) { x=x-1; y=y; return true; }
            if(bi(x-1,y-1)==ON) { x=x-1; y=y-1; return true; }
            if(bi(x,y-1)==ON) { x=x; y=y-1; return true; }
            if(bi(x+1,y-1)==ON) { x=x+1; y=y-1; return true; }
            return false;
        }

        bool nextstart() {
            for(; sx<w; sx++)
                for(sy=0; sy<h; sy++) {
                    if(!bi(sx,sy))
                        continue;
                    if(bi(sx,sy)==DONE)
                        continue;
                    x = sx;
                    y = sy;
                    return true;
                }
            return false;
        }

        void set_image(bytearray &image) {
            w = image.dim(0);
            h = image.dim(1);
            x = 0;
            y = 0;
            sx = 0;
            sy = 0;
            bi.resize(w,h);
            for(int i=0,n=image.length1d(); i<n; i++)
                bi.at1d(i) = image.at1d(i) ? ON : OFF;
            for(int i=0; i<w; i++)
                bi(i,0)=bi(i,h-1)=OFF;
            for(int j=0; j<h; j++)
                bi(0,j)=bi(w-1,j)=OFF;
            w--;
            h--;
        }

        bool get_chain(vec2array &points,bool close=false) {
            points.clear();
            if(!nextstart())
                return false;
            do {
                points.push() = vec2(x,y);
                bi(x,y) = DONE;
            } while(nextpixel());
            x = sx;
            y = sy;
            if(nextpixel()) {
                // sweep up the other direction (if any)
                colib::reverse(points);
                do {
                    points.push() = vec2(x,y);
                    bi(x,y) = DONE;
                } while(nextpixel());
            }
            if(close && points[0].distance(points[points.length()-1])<2.0) {
                // close circular chains
                points.push(points[0]);
            }
            return true;
        }

        intarray breaks;
        vec2array chain;

        bool get_poly(vec2array &poly,float maxdist=1.0,bool close=false) {
            poly.clear();
            breaks.clear();
            if(!get_chain(chain,close))
                return false;
            if(chain.length()<3) {
                for(int i=0; i<chain.length(); i++)
                    poly.push(chain[i]);
            } else {
                breaks.push(0);
                approx_chain(breaks,chain,0,chain.length()-1,maxdist);
                breaks.push(chain.length()-1);
                for(int i=0; i<breaks.length(); i++)
                    poly.push(chain[breaks[i]]);
            }
            return true;
        }

        bool get_chain(floatarray &points,bool close=false,int sample=1) {
            vec2array temp;
            bool result = get_chain(temp,close);
            if(!result)
                return false;
            int total = temp.length();
            int n = int(total/sample);
            int offset = int((total-n*sample)/2);
            int reserve = int((total+sample-1)/sample);
            points.resize(reserve,2);
            fill(points,-1);
            for(int i=offset,j=0; i<total; i+=sample,j++) {
                points(j,0) = temp[i][0];
                points(j,1) = temp[i][1];
            }
            return true;
        }

        bool get_poly(floatarray &points,float maxdist=1.0,bool close=false) {
            vec2array temp;
            bool result = get_poly(temp,maxdist,close);
            if(!result)
                return false;
            points.resize(temp.length(),2);
            for(int i=0; i<temp.length(); i++) {
                points(i,0) = temp[i][0];
                points(i,1) = temp[i][1];
            }
            return true;
        }

        bool started() {
            return bi.dim(0)>0;
        }

        void clear() {
            bi.clear();
            breaks.clear();
            chain.clear();
        }

        ~ChainTracer() {
        }
    };

    IChainTracer::~IChainTracer() {
    }

    IChainTracer *chaintracer(bytearray &image) {
        IChainTracer *result = new ChainTracer();
        result->set_image(image);
        return result;
    }

}
