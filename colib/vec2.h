#ifndef h_vec2__
#define h_vec2__

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
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
// File: vec2.h
// Purpose: 2D vectors
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de


/// \file vec2.h
/// \brief 2D vectors

extern "C" {
#include <math.h>
}

namespace colib {

struct vec2 {
    static inline void range(int i,int n) {
        if(unsigned(i)>=unsigned(n)) throw "range error in vec2";
    }
    template <class T> 
    static inline T sqr(T x) {return x*x;}
    template <class T> 
    static inline T min(T a,T b) { return a<b?a:b; }
    template <class T> 
    static inline T max(T a,T b) { return a>b?a:b; }
    enum {N=2};
    float data[2];
    explicit vec2(float v0=0,float v1=0) {
        data[0] = v0;
        data[1] = v1;
    }
    int length() const {
        return N;
    }
    float at(int i) const {
        range(i,N);
        return data[i];
    }
    float operator()(int i) const {
        range(i,N);
        return data[i];
    }
    float &operator()(int i) {
        range(i,N);
        return data[i];
    }
    float operator[](int i) const {
        range(i,N);
        return data[i];
    }
    float &operator[](int i) {
        range(i,N);
        return data[i];
    }
    vec2 operator+(const vec2 &other) const {
        return vec2(at(0)+other(0),at(1)+other(1));
    }
    vec2 operator-(const vec2 &other) const {
        return vec2(at(0)-other(0),at(1)-other(1));
    }
    float operator*(const vec2 &other) const {
        return at(0)*other(0)+at(1)*other(1);
    }
    vec2 operator*(float scale) const {
        return vec2(at(0)*scale,at(1)*scale);
    }
    vec2 operator/(float scale) const {
        return vec2(at(0)/scale,at(1)/scale);
    }
    float magnitude() const {
        return sqrt(sqr(data[0])+sqr(data[1]));
    }
    float angle() const {
        return atan2(data[1],data[0]);
    }
    float magnitude_squared() const {
        return sqr(data[0])+sqr(data[1]);
    }
    vec2 normalized() const {
        return operator*(1.0/magnitude());
    }
    vec2 normal() const {
        return vec2(-data[1],data[0]);
    }
    float distance(const vec2 &b) {
        const vec2 &a = *this;
        return (a-b).magnitude();
    }
    inline vec2 cmul(const vec2 &b) const {
        const vec2 &a = *this;
        return vec2(a[0]*b[0]-a[1]*b[1],a[0]*b[1]+a[1]*b[0]);
    }
    inline vec2 cdiv(const vec2 &b) const {
        const vec2 &a = *this;
        double n = sqr(b[0])+sqr(b[1]);
        return vec2((a[0]*b[0]+a[1]*b[1])/n,(a[1]*b[0]-a[0]*b[1])/n);
    }
};

}

#endif
