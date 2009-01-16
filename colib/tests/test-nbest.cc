#include "colib.h"
using namespace colib;

// -*- C++ -*-

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
// File: test-nbest.cc
// Purpose: testing nbest
// Responsible: Syed Atif Mehdi
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

/// \file test-nbest.cc
/// \brief testing n-best



int main(/*int argc,char **argv*/) {

	int size=10;
	NBest best(size);
	
	for (int i =0; i<size; i++)
	{
		best.add(i, (double)i*size);
	}

	TEST_ASSERT(best.length() == size);

        try {
	    TEST_ASSERT(best.value(-8) == 8*size) ;
	    TEST_ASSERT(!"NBest allowed negative indexing!");
        } catch(const char *err) { /* OK */}

	TEST_ASSERT(best[0] == 9);
	TEST_ASSERT(best[1] == 8);
	TEST_ASSERT(best[2] == 7);
	best.clear();

	for (int i =0; i<size*2; i++)
	{
		best.add(i, (double)i*size);
	}
	TEST_ASSERT(best.value(0) == size*(size*2-1));
	TEST_ASSERT(best[0] == size*2-1);
	best.clear();
    return 0;
}

