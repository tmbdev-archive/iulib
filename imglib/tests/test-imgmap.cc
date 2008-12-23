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
// File: test-imgmap.cc
// Purpose: test code for scaling operations in imgmap
// Responsible: ulges
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#include "colib/colib.h"
#include "imgio.h"
#include "imglib.h"
#include <stdarg.h>


using namespace iulib;
using namespace colib;

int main(int argc, char **argv) {
    {

        bytearray img;
        bytearray res1, res2, res3, res4;
        float scale1 = 0.5;
        float scale2 = 0.3;

        //// read a graylevel image
        //read_pnm_gray(stdin, img);
        img.resize(23, 17);

        // call rescaling
        scale_sample(res1, img, scale1, scale1);
        TEST_OR_DIE(res1.dim(0)==(int)(scale1*img.dim(0)));
        TEST_OR_DIE(res1.dim(1)==(int)(scale1*img.dim(1)));
        //display(res1);

        // call rescaling
        scale_interpolate(res2, img, scale2, scale2);
        TEST_OR_DIE(res2.dim(0)==(int)(scale2*img.dim(0)));
        TEST_OR_DIE(res2.dim(1)==(int)(scale2*img.dim(1)));
        //display(res2);

        // call rescaling
        scale_interpolate(res3, img, scale1, scale2);
        TEST_OR_DIE(res3.dim(0)==(int)(scale1*img.dim(0)));
        TEST_OR_DIE(res3.dim(1)==(int)(scale2*img.dim(1)));
        //display(res3);

        // call rescaling
        scale_interpolate(res4, img, 50, 30);
        TEST_OR_DIE(res4.dim(0)==50);
        TEST_OR_DIE(res4.dim(1)==30);
        //display(res4);
    }
    //printf("All tests on %s completed successfully!\n",argv[0]);
}
