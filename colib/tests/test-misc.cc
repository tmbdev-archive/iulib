#include <limits.h>
#include <float.h>
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
// File: test-misc.cc
// Purpose: test cases for misc implementation
// Responsible: Syed Atif Mehdi
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de



using namespace colib;



int main(int argc,char **argv) 
{

	int i_arr[2]={INT_MIN,INT_MAX};
	float f_arr[2]={FLT_MIN,FLT_MAX };
	char c_arr[2]={'a','A'};
	Boolean<true> b;
	Integer<INT_MAX> i;
//	floating<FLT_MAX> f;

	Floating<INT_MAX> f;

	swap (i_arr[0], i_arr[1]);
	TEST_ASSERT(i_arr[0] > i_arr[1]);
	swap (f_arr[0], f_arr[1]);
	TEST_ASSERT(f_arr[0] > f_arr[1]);
	swap (c_arr[0], c_arr[1]);
	TEST_ASSERT(c_arr[0] < c_arr[1]);

	
	TEST_ASSERT( min(i_arr[0], i_arr[1]) == INT_MIN);
	TEST_ASSERT( min(f_arr[0], f_arr[1]) == FLT_MIN);
	TEST_ASSERT( min(c_arr[0], c_arr[1]) == 'A');


	TEST_ASSERT( max(i_arr[0], i_arr[1]) == INT_MAX);
	TEST_ASSERT( max(f_arr[0], f_arr[1]) == FLT_MAX);
	TEST_ASSERT( max(c_arr[0], c_arr[1]) == 'a');


	TEST_ASSERT( sqr(50) == 50*50 );
	TEST_ASSERT( sqr(50.5) == 50.5*50.5 );
	
	
	TEST_ASSERT( abs(i_arr[0]) == INT_MAX );
	TEST_ASSERT( abs(i_arr[1]) == INT_MAX + 1);

	TEST_ASSERT( abs(f_arr[0]) == FLT_MAX );
	//TEST_ASSERT( abs(f_arr[1]) == FLT_MAX );


	TEST_ASSERT( heaviside(INT_MIN) == 0 );
	TEST_ASSERT( heaviside(INT_MAX) == INT_MAX );

	TEST_ASSERT( heaviside(-FLT_MIN) == 0 );
	TEST_ASSERT( heaviside(FLT_MAX) == FLT_MAX );

//	TEST_ASSERT( (bool)b == true );
//	TEST_ASSERT( (int)i == INT_MAX );
//	TEST_ASSERT( (floating)f == FLT_MAX );
//	TEST_ASSERT( (floating)f == INT_MAX );


    return 0;
}

