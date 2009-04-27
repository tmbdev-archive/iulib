// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
// Copyright 1995-2005 by Thomas M. Breuel
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
// File: 
// Purpose: 
// Responsible: 
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

extern "C" {
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
};

#include "colib/colib.h"
#include "imglib.h"
#include "imgio.h"

using namespace colib;
using namespace iulib;

param_float sigma("sigma",2.0,"size of Gaussian used for smoothing");
param_float frac("frac",0.3,"Canny fractile");
param_float low("low",2.0,"Canny low parameter");
param_float high("high",4.0,"Canny high parameter");

int main(int argc,char **argv) {
    try {
        bytearray image;
        read_image_gray(image,argv[1]);
        floatarray fimage;
        copy(fimage,image);
        floatarray out;
        canny(out,fimage,sigma,sigma,frac,low,high);
        bytearray edges;
        makelike(edges,out);
        for(int i=0;i<out.length1d();i++)
            edges.at1d(i) = (out.at1d(i)>0)?255:0;
        write_png(stdio(argv[2],"w"),edges);
    } catch(const char *message) {
        fprintf(stderr,"ERROR: %s\n",message);
        exit(255);
    }
}
