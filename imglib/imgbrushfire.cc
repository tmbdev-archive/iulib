// -*- C++ -*-

// Copyright 2006-2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
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
// File: imgbrushfire.cc
// Purpose: compute (approximate) distance transform using brushfire algorithm
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

extern "C" {
#include <math.h>
}
#include "colib/colib.h"
#include "imgbrushfire.h"


using namespace colib;

namespace iulib {

    /// LIFO queue data structure

    template <class T>
    struct LIFO_ {
        T *buffer;
        int start,length,total;

        /// Create a LIFO with a maximum size of n.
        
        LIFO_(int n) {
            buffer = new T[n];
            total = n;
            start = 0;
            length = 0;
        }

        ~LIFO_() {
            delete [] buffer;
        }

        /// Add an object to one end of the queue.

        void enqueue(T obj) {
            ASSERT(length<total);
            int index = start+length;
            if(index>=total) index -= total;
            buffer[index] = obj;
            length++;
        }

        /// Remove an object from the other end of the queue.

        T dequeue() {
            ASSERT(length>0);
            T result = buffer[start++];
            if(start==total) start = 0;
            length--;
            return result;
        }
    };

    template <class M>
    struct Brushfire {
        // SGI compiler bug: can't make this a template function with
        // an unused last argument for the template parameter
        static void go(floatarray &distance,narray<point> &source,float maxdist);
    };

    template <class M>
    void Brushfire<M>::go(floatarray &distance,narray<point> &source,float maxdist) {
        const float BIG = 1e38;

        int w = distance.dim(0);
        int h = distance.dim(1);
        distance.resize(w,h);
        source.resize(w,h);

        LIFO_<point> queue(w*h);

        int i, j;
        for(i = 0; i < w; i++) for(j = 0; j < h; j++) {
            if(distance.at(i,j)) {
                queue.enqueue(point(i,j));
                distance.at(i,j) = 0;
                source.at(i,j) = point(i,j);
            } else {
                distance.at(i,j) = BIG;
                source.at(i,j) = point(-1,-1);
            }
        }

        while(queue.length != 0) {
            point q = queue.dequeue();
            float d = M::metric(point(q.x - 1, q.y), source.at(q.x,q.y));
            if(d <= maxdist && q.x > 0 && d < distance.at(q.x - 1,q.y)) {
                queue.enqueue(point(q.x - 1, q.y));
                source.at(q.x - 1,q.y) = source.at(q.x,q.y);
                distance.at(q.x - 1,q.y) = d;
            }
            d = M::metric(point(q.x, q.y - 1), source.at(q.x,q.y));
            if(d <= maxdist && q.y > 0 && d < distance.at(q.x,q.y - 1)) {
                queue.enqueue(point(q.x, q.y - 1));
                source.at(q.x,q.y - 1) = source.at(q.x,q.y);
                distance.at(q.x,q.y - 1) = d;
            }
            d = M::metric(point(q.x + 1, q.y), source.at(q.x,q.y));
            if(d <= maxdist && q.x < w - 1 && d < distance.at(q.x + 1,q.y)) {
                queue.enqueue(point(q.x + 1, q.y));
                source.at(q.x + 1,q.y) = source.at(q.x,q.y);
                distance.at(q.x + 1,q.y) = d;
            }
            d = M::metric(point(q.x, q.y + 1), source.at(q.x,q.y));
            if(d <= maxdist && q.y < h - 1 && d < distance.at(q.x,q.y + 1)) {
                queue.enqueue(point(q.x, q.y + 1));
                source.at(q.x,q.y + 1) = source.at(q.x,q.y);
                distance.at(q.x,q.y + 1) = d;
            }
        }
    }

    inline int abs_(int x) { return (x<0)?-x:x; }
    inline int max_(int x,int y) { return (x<y)?y:x; }
    inline float abs_(float x) { return (x<0)?-x:x; }
    inline float max_(float x,float y) { return (x<y)?y:x; }

    struct Metric1 {
        static inline float metric(point p,point q) {
            int dx = p.x-q.x;
            int dy = p.y-q.y;
            return abs_(dx)+abs_(dy);
        }
    };

    struct Metric2 {
        static inline float metric(point p,point q) {
            int dx = p.x-q.x;
            int dy = p.y-q.y;
            return dx*dx + dy*dy;
        }
    };

    struct MetricInf {
        static inline float metric(point p,point q) {
            int dx = abs_(p.x-q.x);
            int dy = abs_(p.y-q.y);
            return max_(dx,dy);
        }
    };

    void brushfire_1(floatarray &distance,narray<point> &source,float maxdist) {
        Brushfire<Metric1>::go(distance,source,maxdist);
    }

    void brushfire_2(floatarray &distance,narray<point> &source,float maxdist) {
        Brushfire<Metric2>::go(distance,source,maxdist);
        for(int i=0;i<distance.length1d();i++)
            distance.at1d(i) = sqrt(distance.at1d(i));
    }

    void brushfire_inf(floatarray &distance,narray<point> &source,float maxdist) {
        Brushfire<MetricInf>::go(distance,source,maxdist);
    }

    void bool_invert(floatarray &image) {
        for(int i=0;i<image.length1d();i++)
            image.at1d(i) = !image.at1d(i);
    }

    void threshold(floatarray &image,float thresh) {
        for(int i=0;i<image.length1d();i++)
            if(image.at1d(i)>=thresh) image.at1d(i) = 1; else image.at1d(i) = 0;
    }

    void inverse_threshold(floatarray &image,float thresh) {
        for(int i=0;i<image.length1d();i++)
            if(image.at1d(i)<thresh) image.at1d(i) = 1; else image.at1d(i) = 0;
    }

    // simple dilations and erosions using distance transforms

    template <class T>
    struct temparray {
        narray<T> r;
        operator narray<T> &() { return r; }
    };

    struct pointhack {
        intarray &a;
        narray<point> data;
        pointhack(intarray &a):a(a) {}
        ~pointhack() {
            makelike(a,data);
            for(int i=0;i<a.dim(0);i++) for(int j=0;j<a.dim(1);j++) {
                a(i,j,0) = data(i,j).x;
                a(i,j,1) = data(i,j).y;
            }
        }
        operator narray<point> &() { return data; }
    };

    /// Brushfire transformation using 1-norm.
    
    void brushfire_1(floatarray &distance,float maxdist) {
        brushfire_1(distance,temparray<point>(),maxdist);
    }

    /// Brushfire transformation using 2-norm.
    
    void brushfire_2(floatarray &distance,float maxdist) {
        brushfire_2(distance,temparray<point>(),maxdist);
    }

    /// Brushfire transformation using infinity-norm.

    void brushfire_inf(floatarray &distance,float maxdist) {
        brushfire_inf(distance,temparray<point>(),maxdist);
    }

    /// Brushfire transformation using 1-norm.
    
    void brushfire_1(floatarray &distance,intarray &source,float maxdist) {
        brushfire_1(distance,pointhack(source),maxdist);
    }

    /// Brushfire transformation using 2-norm.
    
    void brushfire_2(floatarray &distance,intarray &source,float maxdist) {
        brushfire_2(distance,pointhack(source),maxdist);
    }

    /// Brushfire transformation using infinity-norm.
    
    void brushfire_inf(floatarray &distance,intarray &source,float maxdist) {
        brushfire_inf(distance,pointhack(source),maxdist);
    }

    /// Dilation with a diamond (metric figure of 1-norm).    Uses distance transform.
    
    void dilate_1(floatarray &image,float r) {
        brushfire_1(image,temparray<point>(),r+1);
        inverse_threshold(image,r);
    }

    /// Dilation with a circle (metric figure of 2-norm).    Uses distance transform.
    
    void dilate_2(floatarray &image,float r) {
        brushfire_2(image,temparray<point>(),r+1);
        inverse_threshold(image,r);
    }

    /// Dilation with a square (metric figure of infinity-norm).    Uses distance transform.
    
    void dilate_inf(floatarray &image,float r) {
        brushfire_inf(image,temparray<point>(),r+1);
        inverse_threshold(image,r);
    }

    /// Erosion with a diamond (metric figure of 1-norm).    Uses distance transform.
    
    void erode_1(floatarray &image,float r) {
        bool_invert(image);
        brushfire_1(image,temparray<point>(),r+1);
        threshold(image,r);
    }

    /// Erosion with a circle (metric figure of 1-norm).    Uses distance transform.
    
    void erode_2(floatarray &image,float r) {
        bool_invert(image);
        brushfire_2(image,temparray<point>(),r+1);
        threshold(image,r);
    }

    /// Erosion with a square (metric figure of infinity-norm).   Uses distance transform.
    
    void erode_inf(floatarray &image,float r) {
        bool_invert(image);
        brushfire_inf(image,temparray<point>(),r+1);
        threshold(image,r);
    }

    ////////////////////////////////////////////////////////////////
    // unusual/experimental metrics 
    ////////////////////////////////////////////////////////////////

    struct MetricInfScaled {
        // FIXME these static variables aren't thread safe; make the member functions
        // non-static and the metric parameters member variables
        // FIXME perform distance computations using floating point
        // FIXME the brushfire algorithm really needs to know about this
        // to make it work out properly on discrete geometry; for metrics
        // that aren't too different from roundish, it should be OK.
        static float sx,sy;
        static inline float metric(point p,point q) {
            float dx = sx*abs_(p.x-q.x);
            float dy = sy*abs_(p.y-q.y);
            return max_(dx,dy);
        }
    };

    float MetricInfScaled::sx,MetricInfScaled::sy;

    void brushfire_inf_scaled(floatarray &distance,narray<point> &source,float sx,float sy,float maxdist) {
        MetricInfScaled::sx = sx;
        MetricInfScaled::sy = sy;
        Brushfire<MetricInfScaled>::go(distance,source,maxdist);
    }

    void brushfire_inf_scaled(floatarray &distance,float sx,float sy,float maxdist) {
        brushfire_inf_scaled(distance,temparray<point>(),sx,sy,maxdist);
    }

    struct Metric2Scaled {
        // FIXME these static variables aren't thread safe; make the member functions
        // non-static and the metric parameters member variables
        // FIXME perform distance computations using floating point
        // FIXME the brushfire algorithm really needs to know about this
        // to make it work out properly on discrete geometry; for metrics
        // that aren't too different from roundish, it should be OK.
        static float a,b,c,d;
        static inline float metric(point p,point q) {
            float dx = p.x-q.x;
            float dy = p.y-q.y;
            float du = a*dx + b*dy;
            float dv = c*dx + d*dy;
            return du*du+dv*dv;
        }
    };

    float Metric2Scaled::a,Metric2Scaled::b,Metric2Scaled::c,Metric2Scaled::d;

    void brushfire_2_scaled(floatarray &distance,narray<point> &source,float a,float b,float c,float d,float maxdist) {
        Metric2Scaled::a = a;
        Metric2Scaled::b = b;
        Metric2Scaled::c = c;
        Metric2Scaled::d = d;
        Brushfire<Metric2Scaled>::go(distance,source,maxdist);
    }

    void brushfire_2_scaled(floatarray &distance,float a,float b,float c,float d,float maxdist) {
        brushfire_2_scaled(distance,temparray<point>(),a,b,c,d,maxdist);
    }

}
