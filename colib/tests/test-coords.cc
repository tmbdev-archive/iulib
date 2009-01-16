#include "colib.h"
#include "limits.h"


// -*- C++ -*-

// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
// or its licensors, as applicable.
// Copyright 1995-2005 Thomas M. Breuel.
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
// File: test-coords.cc
// Purpose: testing rectangles with integer coordinates
// Responsible: Syed Atif Mehdi
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

/// \file test-coords.cc
/// \brief Testing rectangles with integer coordinates


using namespace colib;

int main(int argc,char **argv) {

	int x0= 0,y0= 0,x1=INT_MAX,y1 =INT_MAX;
	int x2= 30,y2= 30,x3=50, y3=50;
	rectangle r1;
	
	TEST_ASSERT(r1.empty() == true);
	TEST_ASSERT(r1.contains(0,0) == false);
	TEST_ASSERT(r1.contains(-2,-2) == false);
	TEST_ASSERT(r1.area() == 0);
        /*printf("%d %d %d %d", r.x0, r.y0, r.x1, r.y1);
	printf ("area is %d",(r1.grow(5)).area() );
	TEST_ASSERT( (r1.grow(5)).area() == 100);*/

	r1.include(10,10);
	TEST_ASSERT(r1.area() == 1);
	r1.include(20,20);
	TEST_ASSERT(r1.area() == (21-10) * (21-10));
	

	rectangle r2(x0,y0, x1 ,y1);

	TEST_ASSERT(r2.width() == INT_MAX);
	TEST_ASSERT(r2.height() == INT_MAX);
	TEST_ASSERT(r2.contains(10,10) == true);
	TEST_ASSERT(r2.includes (r1) ==true);	

	//TEST_ASSERT(r2.inclusion(r1) == r2);
	TEST_ASSERT(r2.overlaps(r1) == true);

	rectangle r3(x2,y2,x3,y3);
	TEST_ASSERT(r3.aspect() == (float) ((y3-y2)/(float)(x3-x2) ) );
	TEST_ASSERT(r3.xcenter() == (x2+x3)/2 );
	TEST_ASSERT(r3.ycenter() == (y2+y3)/2 );
	TEST_ASSERT(r3.fraction_covered_by(r1) == 0);

	return 0;
}
