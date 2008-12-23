#undef UNSAFE
#include "colib.h"

#include <math.h>

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

#define check_throws(seq) \
    ({ bool result = false; try { seq; } catch(const char *) { result = true; } result; })
#define check_nothrows(seq) \
    ({ bool result = true; try { seq; } catch(const char *) { result = false; } result; })

static int instances_total = 0;

struct Instances {
    Instances() {
        instances_total++;
    }
    ~Instances() {
        --instances_total;
    }
};

using namespace colib;

int main(int argc,char **argv) {
    narray<short> sa;
    TEST_ASSERT(check_throws(sa(0)));
    TEST_ASSERT(check_throws(sa(0,0)));
    sa.resize(100);
    TEST_ASSERT(check_nothrows(sa(99)));
    TEST_ASSERT(check_throws(sa(1,1)));
    TEST_ASSERT(check_throws(sa(100)));
    for(int n=1;n<1000;n++) {
        narray<double> a(n);
        for(int i=0;i<a.dim(0);i++)
            a(i) = int(12.34234809*i);
        for(int i=0;i<a.dim(0);i++)
            TEST_ASSERT(a(i) == int(12.34234809*i));
    }
    for(int n=1;n<17;n++) for(int m=1;m<17;m++) {
        narray<float> a(n,m);
        TEST_ASSERT(a.dim(0)==n);
        TEST_ASSERT(a.dim(1)==m);
        for(int i=0;i<a.dim(0);i++) for(int j=0;j<a.dim(1);j++)
            a(i,j) = int(12.34234809*i+1.9203842*j+1.93252084);
        for(int i=0;i<a.dim(0);i++) for(int j=0;j<a.dim(1);j++)
            TEST_ASSERT(a(i,j) = int(12.34234809*i+1.9203842*j+1.93252084));
    }
    // FIXME add more test cases for higher dimensional arrays
    // narray< integer<0> > a;
    narray<int> a;
    a.fill(0);
    a.resize(100);
    // check reshape permitted only to same total
    TEST_ASSERT(check_throws(a.reshape(101)));
    TEST_ASSERT(check_nothrows(a.reshape(100)));
    TEST_ASSERT(check_throws(a.reshape(99)));
    a.resize(100,101);
    TEST_ASSERT(check_nothrows(a.reshape(101,100)));
    TEST_ASSERT(check_throws(a.reshape(100,100)));
    // check reallocation on large resizes (this isn't guaranteed by
    // the API, but it should be happening if the implementation is
    // working)
    a.dealloc();
    a.resize(100);
    a(50) = 50;
    a.resize(100000);
    TEST_ASSERT(a(50)!=50);
    // verify no reallocation on resizes to smallers sizes (again,
    // not guaranteed, but important for efficient function)
    a(50) = 50;
    a.resize(100);
    TEST_ASSERT(a(50)==50);
    // verify that various operations actually get rid of things
    TEST_ASSERT(instances_total==0);
    {
        narray<Instances> a(100);
        TEST_ASSERT(instances_total>=100);
        a.dealloc();
        TEST_ASSERT(instances_total==0);
        a.resize(50);
        TEST_ASSERT(instances_total>=50);
        a.dealloc();
        TEST_ASSERT(instances_total==0);
        a.resize(101,113);
        TEST_ASSERT(instances_total>=101*113);
        a.resize(1,1);
        TEST_ASSERT(instances_total>=101*113);
    }
    TEST_ASSERT(instances_total==0);
}
