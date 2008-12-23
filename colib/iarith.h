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
// Project: ocr-layout-rast - Extract textlines from a document image using RAST
// File: iarith.h
// Purpose: 
// Responsible: Faisal Shafait (faisal.shafait@dfki.de)
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

/// \file iarith.h
/// \brief Interval arithmetic.

#ifndef h_iarith_
#define h_iarith_

#include <math.h>
#include <float.h>

namespace colib {

#ifdef IARITH_ERRS_NONFATAL
#define returnoops(S,V) return (V)
#else
#define returnoops(S,V) throw S
#endif

    inline double min(double x,double y) { return x<y?x:y; }
    inline double max(double x,double y) { return x>y?x:y; }
    inline double hs(double x) { return x<=0?0:x; }
    inline double hsq(double x) { return x>=0?0:x; }
    inline double dabs(double x) { return x<=0?-x:x; }
    inline double sqr(double x) { return x*x; }

#if !defined(IARITH_FAST) && !defined(IARITH_EXACT)
#define IARITH_FAST
#endif

#ifdef IARITH_FAST
    inline double nextfp(double x) {
        return x;
    }
    inline double prevfp(double x) {
        return x;
    }

    inline double nan() { return NAN; }
    inline double neginfinity() { return -HUGE_VAL; }
    inline double posinfinity() { return HUGE_VAL; }
    inline double negzero() {return (-1.0) * (0.0);}
#define IARITH_PRIMS
#endif

#ifdef IARITH_EXACT
// C99 primitives require gcc-3.0 or above
    inline double nextfp(double x) {
        return nextafter(x,HUGE_VAL);
    }
    inline double prevfp(double x) {
        return nextafter(x,-HUGE_VAL);
    }

    inline double nan() { return NAN; }
    inline double neginfinity() { return -HUGE_VAL; }
    inline double posinfinity() { return HUGE_VAL; }
    inline double negzero() {return (-1.0) * (0.0);}
#endif

    inline double add_lo(double x,double y) { return prevfp(x+y); }
    inline double add_hi(double x,double y) { return nextfp(x+y); }
    inline double sub_lo(double x,double y) { return prevfp(x-y); }
    inline double sub_hi(double x,double y) { return nextfp(x-y); }
    inline double mul_lo(double x,double y) { if(x==0||y==0) return 0; return prevfp(x*y); }
    inline double mul_hi(double x,double y) { if(x==0||y==0) return 0; return nextfp(x*y); }
    inline double div_lo(double x,double y) { if(x==0) return 0; return prevfp(x/y); }
    inline double div_hi(double x,double y) { if(x==0) return 0; return nextfp(x/y); }
    inline double sqrt_lo(double x) { if(x<0) return nan(); return hs(prevfp(sqrt(x))); }
    inline double sqrt_hi(double x) { if(x<0) return nan(); return nextfp(sqrt(x)); }
    inline double exp_lo(double x) {
        if(isinf(x)) {if(x<0) return 0; else return x;}
        return max(0.0,prevfp(exp(x)));
    }
    inline double exp_hi(double x) {
        if(isinf(x)) {if(x<0) return 0; else return x;}
        return nextfp(exp(x));
    }
    inline double log_lo(double x) {
        if(x<0) return nan();
        if(isinf(x)) return x;
        return prevfp(log(x));
    }
    inline double log_hi(double x) {
        if(x<0) return nan();
        if(isinf(x)) return x;
        return nextfp(log(x));
    }
    inline double cos_lo(double x) {return prevfp(cos(x));}
    inline double cos_hi(double x) {return nextfp(cos(x));}
    inline double sin_lo(double x) {return prevfp(sin(x));}
    inline double sin_hi(double x) {return nextfp(sin(x));}

    struct interval {
        double lo,hi;
#ifdef UNSAFE
        interval() {}
#else
        interval():lo(nan()),hi(nan()) {}
#endif
        interval(double x):lo(x),hi(x) {}
        interval(double lo,double hi):lo(lo),hi(hi) {}
        interval &operator=(double x) { lo=x; hi=x; return *this; }
        bool contains(double x) const {
            return x>=lo && x<=hi;
        }
        double width() const {return hi-lo;}
        double center() const { return (hi+lo)/2; }
        void extend(double d) { hi+=d; }
        void widen(double d) { lo-=d; hi+=d; }
        bool icontains(interval other) {
            return lo<=other.lo && other.hi<=hi;
        }
        interval lefthalf() const {
            return interval(lo,nextfp((hi+lo)/2));
        }
        interval righthalf() const {
            return interval(prevfp((hi+lo)/2),hi);
        }
        bool overlaps(interval other) {
            return lo<=other.hi && hi>=other.lo;
        }
        void intersect(interval other) {
            lo = max(lo,other.lo);
            hi = min(hi,other.hi);
        }
        bool empty() {
            return lo>hi;
        }
        interval operator+(interval b) const;
        interval operator+(double b) const;
        interval operator-(interval b) const;
        interval operator-(double b) const;
        interval operator*(interval b) const;
        interval operator*(double b) const;
        interval operator/(interval b) const;
    };

    inline interval naninterval() {
        return interval(nan(),nan());
    }

    inline interval cinterval(double x,double eps) {
        return interval(prevfp(x-eps),nextfp(x+eps));
    }

    inline bool operator==(interval a,double b)   {return a.lo==b    && a.hi==b;}
    inline bool operator!=(interval a,double b)   {return a.lo!=b    || a.hi!=b;}
    inline bool operator==(double a,interval b)   {return a==b.lo    && a==b.hi;}
    inline bool operator!=(double a,interval b)   {return a!=b.lo    || a!=b.hi;}
    inline bool operator==(interval a,interval b) {return a.lo==b.lo && a.hi==b.hi;}
    inline bool operator!=(interval a,interval b) {return a.lo!=b.lo || a.hi!=b.hi;}

    inline bool operator>(interval a,double b) {return a.lo>b;}
    inline bool operator>=(interval a,double b) {return a.lo>=b;}
    inline bool operator<(interval a,double b) {return a.hi<b;}
    inline bool operator<=(interval a,double b) {return a.hi<=b;}

    inline bool operator>(interval a,interval b) {return a.lo>b.hi;}
    inline bool operator>=(interval a,interval b) {return a.lo>=b.hi;}
    inline bool operator<(interval a,interval b) {return a.hi<b.lo;}
    inline bool operator<=(interval a,interval b) {return a.hi<=b.lo;}

    inline bool maybe_less(interval a,double b) {return a.lo<b;}
    inline bool strictly_less(interval a,double b) {return a.hi<b;}

    inline bool maybe_less(double a,interval b) {return a<b.hi;}
    inline bool strictly_less(double a,interval b) {return a<b.lo;}

    inline bool maybe_less(interval a,interval b) {return a.lo<b.hi;}
    inline bool strictly_less(interval a,interval b) {return a.hi<b.lo;}

    inline bool maybe_leq(interval a,double b) {return a.lo<=b;}
    inline bool strictly_leq(interval a,double b) {return a.hi<=b;}

    inline bool maybe_leq(double a,interval b) {return a<=b.hi;}
    inline bool strictly_leq(double a,interval b) {return a<=b.lo;}

    inline bool maybe_leq(interval a,interval b) {return a.lo<=b.hi;}
    inline bool strictly_leq(interval a,interval b) {return a.hi<=b.lo;}

    inline interval rinfluence(interval a,double t) {
        //a = dabs(a);
        if(a.lo<0) returnoops("rinfluence: assert(a.lo>=0)",naninterval());
        return interval(a.hi<t,a.lo<t);
    }
    inline interval sqinfluence(interval a,double t) {
        //a = dabs(a);
        if(a.lo<0) returnoops("sqinfluence: assert(a.lo>=0)",naninterval());
        return interval(max(0.0,1-a.hi*a.hi/t/t),max(0.0,1-a.lo*a.lo/t/t));
    }

    inline bool nonempty(interval x) { return x.hi>=x.lo; }
    inline interval intersect(interval a,interval b) {
        return interval(max(a.lo,b.lo),min(a.hi,b.hi));
    }
    inline interval iunion(interval a,interval b) {
        return interval(min(a.lo,b.lo),max(a.hi,b.hi));
    }

    inline interval iintersect(interval a,interval b) {
        interval result = interval(max(a.lo,b.lo),min(a.hi,b.hi));
        if(result.lo>result.hi) returnoops("empty intersection",naninterval());
        return result;
    }

    inline interval operator-(interval a) {
        return interval(prevfp(-a.hi),nextfp(-a.lo));
    }
    inline interval abs(interval x) {
        if(x.lo>=0.0) return x;
        if(x.hi<=0.0) return -x;
        return interval(0.0,max(dabs(x.lo),dabs(x.hi)));
    }
    inline interval oneover(interval a) {
        if(a.contains(0.0)) returnoops("division by interval containing zero",naninterval());
        return interval(prevfp(1.0/a.hi),nextfp(1.0/a.lo));
    }

    inline interval interval::operator+(interval b) const {
        const interval &a = *this;
        return interval(add_lo(a.lo,b.lo),add_hi(a.hi,b.hi));
    }

    inline interval interval::operator+(double b) const {
        const interval &a = *this;
        return interval(add_lo(a.lo,b),add_hi(a.hi,b));
    }

    inline interval interval::operator-(interval b) const {
        const interval &a = *this;
        return interval(sub_lo(a.lo,b.hi),sub_hi(a.hi,b.lo));
    }

    inline interval interval::operator-(double b) const {
        const interval &a = *this;
        return interval(sub_lo(a.lo,b),sub_hi(a.hi,b));
    }

    inline interval interval::operator*(interval b) const {
        const interval &a = *this;
        if(a==0.0||b==0.0) return interval(0.0,0.0);
        if(a>=0.0) {
            if(b>=0.0) return interval(hs(mul_lo(a.lo,b.lo)),mul_hi(a.hi,b.hi));
            if(b<=0.0) return interval(mul_lo(a.hi,b.lo),hsq(mul_hi(a.lo,b.hi)));
            return interval(mul_lo(a.hi,b.lo),mul_hi(a.hi,b.hi));
        } else if(a<=0.0) {
            if(b>=0.0) return interval(mul_lo(a.lo,b.hi),hsq(mul_hi(a.hi,b.lo)));
            if(b<=0.0) return interval(hs(mul_lo(a.hi,b.hi)),mul_hi(a.lo,b.lo));
            return interval(mul_lo(a.lo,b.hi),mul_hi(a.lo,b.lo));
        } else {
            if(b>=0.0) return interval(mul_lo(a.lo,b.hi),mul_hi(a.hi,b.hi));
            if(b<=0.0) return interval(mul_lo(a.hi,b.lo),mul_hi(a.lo,b.lo));
            return interval(min(mul_lo(a.hi,b.lo),mul_lo(a.lo,b.hi)),
                            max(mul_hi(a.lo,b.lo),mul_hi(a.hi,b.hi)));
        }
    }

    inline interval interval::operator*(double b) const {
        const interval &a = *this;
        if(a==0.0||b==0.0) return interval(0.0,0.0);
        if(a>=0.0) {
            if(b>=0.0) return interval(hs(mul_lo(a.lo,b)),mul_hi(a.hi,b));
            else return interval(mul_lo(a.hi,b),hsq(mul_hi(a.lo,b)));
        } else if(a<=0.0) {
            if(b>=0.0) return interval(mul_lo(a.lo,b),hsq(mul_hi(a.hi,b)));
            else return interval(hs(mul_lo(a.hi,b)),mul_hi(a.lo,b));
        } else {
            if(b>=0.0) return interval(mul_lo(a.lo,b),mul_hi(a.hi,b));
            else return interval(mul_lo(a.hi,b),mul_hi(a.lo,b));
        }
    }

    inline interval interval::operator/(interval b) const {
        const interval &a = *this;
        return a * oneover(b);
    }

    inline interval operator+(double a,interval b) {
        return b+a;
    }
    inline interval operator-(double a,interval b) {
        return -(b-a);
    }
    inline interval operator*(double a,interval b) {
        return b*a;
    }
    inline interval operator/(double a,interval b) {
        return a * oneover(b);
    }

    inline void operator+=(interval &a,double b) { a.lo = prevfp(a.lo+b); a.hi = nextfp(a.hi+b); }
    inline void operator-=(interval &a,double b) { a.lo = prevfp(a.lo-b); a.hi = nextfp(a.hi-b); }

    inline void operator*=(interval &a,interval b) { a = a * b; }
    inline void operator*=(interval &a,double b) { a = a * b; }

    inline interval sin(interval x) {
        interval result;
        if(x.lo<-M_PI/2||x.lo>=2*M_PI+0.01) returnoops("sin argument out of range",naninterval());
        if(x.width()>2*M_PI) return interval(-1,1);
        if(x.contains(M_PI/2)||x.contains(5*M_PI/2)) result.hi = 1.0;
        else result.hi = max(sin_hi(x.lo),sin_hi(x.hi));
        if(x.contains(3*M_PI/2)||x.contains(7*M_PI/2)) result.lo = -1.0;
        else result.lo = min(sin_lo(x.lo),sin_lo(x.hi));
        return result;
    }
    inline interval cos(interval x) {
        interval result;
        if(x.lo<-M_PI/2||x.lo>=2*M_PI+0.01) returnoops("cos argument out of range",naninterval());
        if(x.width()>2*M_PI) return interval(-1,1);
        if(x.contains(0)||x.contains(2*M_PI)) result.hi = 1.0;
        else result.hi = max(cos_hi(x.lo),cos_hi(x.hi));
        if(x.contains(M_PI)||x.contains(3*M_PI)) result.lo = -1.0;
        else result.lo = min(cos_lo(x.lo),cos_lo(x.hi));
        return result;
    }
    inline interval sqr(interval x) {
        x = abs(x);
        return interval(x.lo*x.lo,x.hi*x.hi);
    }
    inline interval sqrt(interval x) {
        return interval(sqrt_lo(x.lo),sqrt_hi(x.hi));
    }
    inline interval exp(interval x) {
        return interval(exp_lo(x.lo),exp_hi(x.hi));
    }
    inline interval log(interval x) {
        if(x<=0.0) returnoops("log of negative interval",naninterval());
        return interval(log_lo(hs(x.lo)),log_hi(x.hi));
    }
    inline interval min(double a,interval b) {
        return interval(min(a,b.lo),min(a,b.hi));
    }
    inline interval max(double a,interval b) {
        return interval(max(a,b.lo),max(a,b.hi));
    }
    inline interval min(interval a,double b) {
        return interval(min(a.lo,b),min(a.hi,b));
    }
    inline interval max(interval a,double b) {
        return interval(max(a.lo,b),max(a.hi,b));
    }
    inline interval min(interval a,interval b) {
        return interval(min(a.lo,b.lo),min(a.hi,b.hi));
    }
//inline interval max(interval a,interval b) {
    inline interval max(interval a,interval b) {
        return interval(max(a.lo,b.lo),max(a.hi,b.hi));
    }

    inline interval angle_canonicalize(interval a) {
        while(a.hi<a.lo) {a.hi = nextfp(a.hi+2*M_PI);}
        while(a.lo<0) {a.lo = prevfp(a.lo+2*M_PI); a.hi = nextfp(a.hi+2*M_PI); }
        while(a.lo>=2*M_PI) { a.lo = prevfp(a.lo-2*M_PI); a.hi = nextfp(a.hi-2*M_PI); }
        return a;
    }

    inline interval atan2(interval y,interval x) {
        using ::atan2;
        int kind = 10;
        if(x.lo>0) {
            if(y.lo>0) kind = 1;
            else if(y.hi>0) kind = 0;
            else kind = 7;
        } else if(x.hi>0) {
            if(y.lo>0) kind = 2;
            else if(y.hi>0) kind = 8;
            else kind = 6;
        } else {
            if(y.lo>0) kind = 3;
            else if(y.hi>0) kind = 4;
            else kind = 5;
        }
        switch(kind) {
        case 0: return angle_canonicalize(interval(atan2(y.lo,x.lo),atan2(y.hi,x.lo)));
        case 1: return angle_canonicalize(interval(atan2(y.lo,x.hi),atan2(y.hi,x.lo)));
        case 2: return angle_canonicalize(interval(atan2(y.lo,x.hi),atan2(y.lo,x.lo)));
        case 3: return angle_canonicalize(interval(atan2(y.hi,x.hi),atan2(y.lo,x.lo)));
        case 4: return angle_canonicalize(interval(atan2(y.hi,x.hi),atan2(y.lo,x.hi)));
        case 5: return angle_canonicalize(interval(atan2(y.hi,x.lo),atan2(y.lo,x.hi)));
        case 6: return angle_canonicalize(interval(atan2(y.hi,x.lo),atan2(y.hi,x.hi)));
        case 7: return angle_canonicalize(interval(atan2(y.lo,x.lo),atan2(y.hi,x.hi)));
        case 8: return angle_canonicalize(interval(0.0,prevfp(2*M_PI)));
        default: returnoops("oops",naninterval());
        }
    }

    inline interval angle_difference(interval a,double b) {
        while(a.lo>=2*M_PI) a -= 2*M_PI;
        while(a.lo<0) a += 2*M_PI;
        while(b>=2*M_PI) b -= 2*M_PI;
        while(b<0) b += 2*M_PI;
        interval range = min(abs(a-b),min(abs(a-(b-2*M_PI)),abs(a-(b+2*M_PI))));
        return range;
    }

    inline interval orientation_difference(interval a,double b) {
        while(a.lo>=M_PI) a -= M_PI;
        while(a.lo<0) a += M_PI;
        while(b>=M_PI) b -= M_PI;
        while(b<0) b += M_PI;
        interval range = min(abs(a-b),min(abs(a-(b-M_PI)),abs(a-(b+M_PI))));
        return range;
    }

#if 0
    struct vec2 {
        double x,y;
        vec2(){}
        vec2(double x,double y):x(x),y(y) {}
        vec2 operator+(const vec2 &other) const {
            return vec2(x+other.x,y+other.y);
        }
        vec2 operator-(const vec2 &other) const {
            return vec2(x-other.x,y-other.y);
        }
        vec2 operator-() const {
            return vec2(-x,-y);
        }
        interval operator*(const vec2 &other) const {
            return x*other.x+y*other.y;
        }
        vec2 operator*(double other) const {
            return vec2(x*other,y*other);
        }
        interval distance_squared(const vec2 &other) {
            return sqr(x-other.x)+sqr(y-other.y);
        }
        interval distance(const vec2 &other) {
            return sqrt(max(0.0,distance_squared(other)));
        }
    };
#endif

    struct vec2i {
        interval x,y;
        vec2i(){}
        vec2i(interval x,interval y):x(x),y(y) {}
        vec2i(vec2 v):x(v.data[0]),y(v.data[1]) {}
        float minwidth() {
            return min(x.width(),y.width());
        }
        float maxwidth() {
            return max(x.width(),y.width());
        }
        vec2i operator+(const vec2i &other) const {
            return vec2i(x+other.x,y+other.y);
        }
        vec2i operator-(const vec2i &other) const {
            return vec2i(x-other.x,y-other.y);
        }
        vec2i operator-() const {
            return vec2i(-x,-y);
        }
        interval operator*(const vec2i &other) const {
            return x*other.x+y*other.y;
        }
        vec2i operator*(interval other) const {
            return vec2i(x*other,y*other);
        }
        vec2i operator*(double other) const {
            return vec2i(x*other,y*other);
        }
        interval distance_squared(const vec2i &other) const {
            return sqr(x-other.x)+sqr(y-other.y);
        }
        interval distance(const vec2i &other) const {
            return sqrt(max(0.0,distance_squared(other)));
        }
        inline interval operator*(vec2 v) {
            vec2i &u = *this;
            return u.x*v.data[0]+u.y*v.data[1];
        }
    };

    inline interval norm(vec2i v) {
        return sqrt(max(0.0,sqr(v.x)+sqr(v.y)));
    }

    inline double norm(vec2 v) {
        return ::sqrt(max(0.0,sqr(v.data[0])+sqr(v.data[1])));
    }

    inline vec2i perpendicular(vec2i v) {
        return vec2i(-v.y,v.x);
    }

    inline vec2i cmul(const vec2i &a,const vec2i &b) {
        return vec2i(a.x*b.x-a.y*b.y,a.x*b.y+a.y*b.x);
    }

    inline vec2i cdiv(const vec2i &a,const vec2i &b) {
        interval n = sqr(b.x)+sqr(b.y);
        return vec2i((a.x*b.x+a.y*b.y)/n,(a.y*b.x-a.x*b.y)/n);
    }

    template <int N>
    struct vecni {
        enum { length = N };
        interval p[N];
        interval &operator[](int i) {
            if(unsigned(i)>=unsigned(N)) returnoops("index error",p[0]);
            return p[i];
        }
        void set(int i,interval v) {
            (*this)[i] = v;
        }
        const interval &operator[](int i) const {
            if(unsigned(i)>=unsigned(N)) returnoops("index error",p[0]);
            return p[i];
        }
        bool contains(const vecni<N> &other) {
            for(int i=0;i<N;i++)
                if(!p[i].icontains(other.p[i]))
                    return false;
            return true;
        }
        bool overlaps(const vecni<N> &other) {
            for(int i=0;i<N;i++)
                if(!p[i].overlaps(other.p[i]))
                    return false;
            return true;
        }
        void intersect(const vecni<N> &other) {
            for(int i=0;i<N;i++) p[i].intersect(other.p[i]);
        }
        bool empty() {
            for(int i=0;i<N;i++)
                if(p[i].empty())
                    return true;
            return false;
        }
        vecni<N> split(int depth,int which) const {
            vecni<N> result = *this;
            switch(which) {
            case 0:
                result[depth%N] = p[depth%N].lefthalf();
                break;
            case 1:
                result[depth%N] = p[depth%N].righthalf();
                break;
            default:
                returnoops("index error (vecni::split)",*this);
            }
            return result;
        }
        vecni<N> splitmax(int which) const {
            int mi = -1; double mw = 0.0;
            for(int i=0;i<N;i++) {
                double w = p[i].width();
                if(w<0) returnoops("empty interval in vecni",*this);
                if(w<=mw) continue;
                mw = w;
                mi = i;
            }
            return split(mi,which);
        }
        double width() const {
            double mw = 0;
            for(int i=0;i<N;i++) {
                double w = p[i].width();
                if(w<0) returnoops("region is empty",-1);
                if(w>mw) mw = w;
            }
            return mw;
        }
    };


    typedef vecni<4> vecni4;

    struct dinterval {
        interval x;
        interval dx;
        dinterval(double x):x(x),dx(0.0) {}
        dinterval(interval x):x(x),dx(0.0) {}
        dinterval(interval x,interval dx):x(x),dx(dx) {}
        dinterval operator+(dinterval other) {
            return dinterval(x+other.x,dx+other.dx);
        }
        dinterval operator-(dinterval other) {
            return dinterval(x-other.x,dx+other.dx);
        }
        dinterval operator*(dinterval other) {
            return dinterval(x*other.x,x*other.dx+dx*other.x);
        }
        dinterval operator/(dinterval other) {
            return dinterval(x/other.x,(dx*other.x-x*other.dx)/(other.x*other.x));
        }
    };

    inline dinterval sqr(dinterval v) {
        return dinterval(v.x*v.x,v.x*v.dx*2);
    }

    inline dinterval sqrt(dinterval v) {
        interval r = sqrt(v.x);
        return dinterval(r,(v.dx/r)*0.5);
    }

    inline dinterval sin(dinterval v) {
        return dinterval(sin(v.x),cos(v.x)*v.dx);
    }

    inline dinterval cos(dinterval v) {
        return dinterval(cos(v.x),-sin(v.x)*v.dx);
    }
}

#endif
