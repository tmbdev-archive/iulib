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
// File: test-heap.cc
// Purpose: test cases for heap implementation
// Responsible: Syed Atif Mehdi
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de


using namespace colib;


int main(int argc,char **argv) 
{
    heap<int> data;
    int count = 1000;

		srand(0); 
    int random_integer; 
    int lowest=-count, highest=count;
    int range=(highest-lowest)+1;
    
		for(int index=0; index<count; index++)
		{ 
        random_integer = lowest+ (rand()%range); 
        data.insert (random_integer, random_integer);
    } 

		int returned_value=count+1;
		// property of heap: when data removed, its in sorted order.	

		TEST_OR_DIE(data.length() == count);
		for(int index=0; index<count; index++)
		{
			int ret= (int)data.extractMax();
			if (ret > returned_value)
				TEST_OR_DIE(1 == 0);
			returned_value = ret;
		}

    return 0;
}
