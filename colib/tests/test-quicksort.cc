#include "colib.h"
#include "quicksort.h"

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
// Project: iulib -- image understanding library
// File: test-quicksort.cc
// Purpose: test cases for quicksort implementation
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de


#include "colib.h"
using namespace colib;

int main(int argc,char **argv) {
    srand(0);
    for(int trial=0;trial<1000;trial++) {
        int n = rand() % 1000;
        narray<float> data;
        narray<int> perm;
        for(int i=0;i<n;i++)
            data.push(rand()%100);
        quicksort(perm,data);
        for(int i=1;i<n;i++)
            CHECK_CONDITION(data[perm[i-1]]<=data[perm[i]]);
        permute(data,perm);
        for(int i=1;i<n;i++)
            CHECK_CONDITION(data[i-1]<=data[i]);
    }
    for(int trial=0;trial<1000;trial++) {
        int n = rand() % 1000;
        narray<float> data;
        for(int i=0;i<n;i++)
            data.push(rand()%100);
        quicksort(data);
        for(int i=1;i<n;i++)
            CHECK_CONDITION(data[i-1]<=data[i]);
    }
    return 0;
}
