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
// File: test_read_write_png.cc
// Purpose: test io_png
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

static void random_binary_image(bytearray &b, int w, int h) {
    b.resize(w, h);
    for(int i = 0; i < b.length1d(); i++) {
        b.at1d(i) = 255. * rand() / RAND_MAX;
    }
    gauss2d(b, 2, 2);
    binarize_by_threshold(b);
}

int main(int argc,char **argv) {
    try {
        bytearray a;
        bytearray b;
        bytearray c;
        bytearray d;
        random_binary_image(b, 200, 200);

        write_image_binary("test_write.png",b);
        read_image_binary(c,stdio("test_write.png","rb") );
        TEST_OR_DIE(equal(b,c));
        read_png(d, stdio("test_write.png","rb"));
        for(int x = 0; x < b.dim(0); x++) {
            for(int y = 0; y < b.dim(1); y++) {
                TEST_OR_DIE(b(x,y) == d(x,y,0));
                TEST_OR_DIE(b(x,y) == d(x,y,1));
                TEST_OR_DIE(b(x,y) == d(x,y,2));
            }
        }
        write_png(stdio("test_image.png","wb"),d);
        intarray image;
        read_png_packed(image, stdio("test_write.png","rb"));
        TEST_OR_DIE(image.rank() == 2);
        for(int x = 0; x < image.dim(0); x++) {
            for(int y = 0; y < image.dim(1); y++) {
                TEST_OR_DIE(image(x,y) == b(x,y) * 0x10101);
            }
        }
        write_png_packed(stdio("test_image.png","wb"), image);
        remove("test_write.png");
        remove("test_image.png");
    } catch(const char *errmsg) {
        fprintf(stderr, "TEST FAILED: %s\n", errmsg);
    }
    return 0;
}

