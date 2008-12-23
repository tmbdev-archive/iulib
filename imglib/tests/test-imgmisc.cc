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
// File: test-imgmisc.cc
// Purpose: test code for imgmisc
// Responsible: Faisal Shafait
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#include "colib/colib.h"
#include "imgio.h"
#include "imglib.h"
#include <stdarg.h>


using namespace iulib;
using namespace colib;

param_bool verbose_test("verbose_test", 0, "print test data to stdout");

template<class T> void fill(narray<T> &a, int count...) {
    a.clear();
    va_list args;
    va_start(args,count);
    for(int i=0; i<count; i++) {
        T val = va_arg(args,T);
        a.push(val);
    }
    va_end(args);
}

int main(int argc, char **argv) {
    {
        intarray ihist, pks, vls;
        floatarray fhist;
        int count = 5;

        // Check different cases for valleys
        fill(ihist, count, 9, 8, 8, 9, 10);
        copy(fhist, ihist);
        valleys(vls, fhist);
        TEST_OR_DIE(vls.length()==1);
        TEST_OR_DIE(vls[0]==2);

        fill(ihist, count, 9, 8, 7, 6, 6);
        copy(fhist, ihist);
        valleys(vls, fhist);
        TEST_OR_DIE(vls.length()==0);

        count = 8;
        fill(ihist, count, 8, 9, 10, 9, 8, 8, 9, 10);
        copy(fhist, ihist);
        valleys(vls, fhist);
        TEST_OR_DIE(vls.length()==1);
        TEST_OR_DIE(vls[0]==5);

        fill(ihist, count, 8, 10, 8, 9, 8, 8, 9, 10);
        copy(fhist, ihist);
        valleys(vls, fhist);
        TEST_OR_DIE(vls.length()==2);
        TEST_OR_DIE(vls[0]==2);
        TEST_OR_DIE(vls[1]==5);

        // Check different cases for peaks
        count = 5;
        fill(ihist, count, 9, 8, 8, 9, 10);
        copy(fhist, ihist);
        peaks(pks, fhist);
        TEST_OR_DIE(pks.length()==0);

        fill(ihist, count, 6, 8, 9, 8, 7);
        copy(fhist, ihist);
        peaks(pks, fhist);
        TEST_OR_DIE(pks.length()==1);
        TEST_OR_DIE(pks[0]==2);

        fill(ihist, count, 7, 8, 8, 8, 7);
        copy(fhist, ihist);
        peaks(pks, fhist);
        TEST_OR_DIE(pks.length()==1);
        TEST_OR_DIE(pks[0]==3);

        count = 8;
        fill(ihist, count, 8, 9, 10, 9, 8, 8, 9, 10);
        copy(fhist, ihist);
        peaks(pks, fhist);
        TEST_OR_DIE(pks.length()==1);
        TEST_OR_DIE(pks[0]==2);

        count = 10;
        fill(ihist, count, 10, 9, 10, 10, 9, 8, 7, 7, 8, 7);
        copy(fhist, ihist);
        peaks(pks, fhist);
        TEST_OR_DIE(pks.length()==2);
        TEST_OR_DIE(pks[0]==3);
        TEST_OR_DIE(pks[1]==8);

    }
    //printf("All tests on %s completed successfully!\n",argv[0]);
}
