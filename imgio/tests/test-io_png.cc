// Copyright 2006-2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
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
// Project: roughocr -- mock OCR system exercising the interfaces and useful for testing
// File: test_io_png.cc
// Purpose: Test cases for io_png
// Responsible: Syed Atif Mehdi
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#include <stdio.h>
#include "colib/colib.h"
#include "imgio.h"
#include "imglib.h"


using namespace iulib;
using namespace colib;

static void random_binary_image(bytearray &b, int w, int h) 
{
	b.resize(w, h);
	for(int i = 0; i < b.length1d(); i++) 
	{
		b.at1d(i) = 255. * rand() / RAND_MAX;
	}
	gauss2d(b, 2, 2);
	binarize_by_threshold(b);
}

int main(int argc,char **argv) {

	bytearray a;
	intarray f;

	bytearray b;
	bytearray c;
	intarray d;
	intarray e;

	random_binary_image(b, 200, 200);

	try
	{
		write_png(stdio("test_write.png","w"),b);
	}
	catch(...)
	{
		TEST_OR_DIE(equal(a,b));
	}
	try
	{	
		read_png(c,stdio("test_write.png","r") );
	}
	catch(...)
	{
		TEST_OR_DIE(equal(a,c));
	}
	try
	{
		read_png_packed(d,stdio("test_write.png","r"));
	}
	catch(...)
	{
		TEST_OR_DIE(equal(f,d));
	}
	try
	{
		write_png_packed(stdio("test_image.png","w"),d);
	}
	catch(...)
	{
		TEST_OR_DIE(equal(f,d));
	}

	remove("test_write.png");
	remove("test_image.png");

	//FIXME: might required to test the color image input and output. Syed Atif Mehdi

	return 0;
}

