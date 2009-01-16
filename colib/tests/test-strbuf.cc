#include <colib.h>
#include <limits.h>

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
// File: test-strbuf.cc
// Purpose: testing string buffer
// Responsible: Syed Atif Mehdi
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de



using namespace colib;

int main(int argc,char **argv) {
	
	strbuf strobj2;
        strobj2 = "0123456789";
	TEST_ASSERT(strobj2.length() == 10);
	{
		strbuf strobj1;
		TEST_ASSERT(strobj1.length() ==0 );
		strobj1 += "Hello World";
		strobj2 = strobj1;
		strobj2 +=strobj1;
	}
	
	TEST_ASSERT( strcmp( (char*)strobj2, "Hello WorldHello World") == 0 );

	//strbuf strobj3(INT_MAX - strlen( (char*)strobj2) -1) ;
	//TEST_ASSERT(strobj3.length() == (INT_MAX - strlen( (char*)strobj2)) );
	//strobj3 +=strobj2;
	//TEST_ASSERT(strobj3.length() == INT_MAX);
	

	utf8buf obj1;
	obj1 = (char*)strobj2;
	TEST_ASSERT( strcmp( (char*)obj1, "Hello WorldHello World") == 0 );

	return 0;
}
