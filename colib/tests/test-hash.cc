#include "colib.h"

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
// File: test-hash.cc
// Purpose: test cases for hash table implementation
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de


using namespace colib;

int main(int argc,char **argv) {
    inthash<int> data;
    for(int i=0;i<10000;i++)
        data(i) = i*i;
    for(int i=0;i<10000;i++)
        TEST_ASSERT(data(i) == i*i);

    int2hash<int> data2;
    for(int i=0;i<500;i++) for(int j=0;j<500;j++)
        data2(i,j) = i*901+j;
    for(int i=0;i<500;i++) for(int j=0;j<500;j++)
        TEST_ASSERT(data2(i,j) == i*901+j);

    strhash<int> data3;
    char buf[1000];
    for(int i=0;i<1000;i++) {
        sprintf(buf,"%d",i);
        data3(buf) = i*i-17;
    }
    for(int i=0;i<1000;i++) {
        sprintf(buf,"%d",i);
        TEST_ASSERT(data3(buf) == i*i-17);
    }
    return 0;
}
