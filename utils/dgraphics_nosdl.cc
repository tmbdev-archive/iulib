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

#include <stdlib.h>
#include "colib/colib.h"
#include "imglib.h"

#include "dgraphics.h"

using namespace colib;

namespace iulib {
    param_bool dgraphics_enabled("dgraphics",true,"debugging graphics");

    void dinit(int w,int h, bool force) {
        if(force || dgraphics_enabled) {
            fprintf(stderr,"no image display, since dgraphics disabled in iulib\n");
        }
    }

    template <class T>
    void dshow(narray<T> &data,const char *spec,double angle,int smooth,int rgb) {
    }

    template void dshow(narray<unsigned char> &data,const char *spec,double angle,int smooth,int rgb);
    template void dshow(narray<int> &data,const char *spec,double angle,int smooth,int rgb);
    template void dshow(narray<float> &data,const char *spec,double angle,int smooth,int rgb);

    template <class T>
    void dshown(narray<T> &data,const char *spec,double angle, int smooth, int rgb) {}
    template void dshown(narray<unsigned char> &data,const char *spec,double angle,int smooth,int rgb);
    template void dshown(narray<int> &data,const char *spec,double angle,int smooth,int rgb);
    template void dshown(narray<float> &data,const char *spec,double angle,int smooth,int rgb);

    void dshowr(intarray &data,const char *spec,double angle,int smooth,int rgb) {
    }

    void dshow(floatarray &data,const char *spec,double angle,int smooth,int rgb) {
    }

    void dshow1d(floatarray &data,const char *spec) {
    }

    void dclear(int rgb) {
    }

    void dwait() {
    }
}
