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
// Project: roughocr -- mock OCR system exercising the interfaces and useful for testing
// File: test-vidio.cc
// Purpose: test and demonstrate interface for vidio.h
// Responsible: ulges
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

extern "C" {
#include <assert.h>
}

#include "colib/colib.h"
#include "vidio.h"
#include "io_pbm.h"

#define FRAMERATE 3
#define SIZEX 120
#define SIZEY 80
#define QUALITY 2

using namespace iulib;

void usage() {
    fprintf(stderr, "\nTest file for Video I/O interface.\n");
    fprintf(stderr, "  (1) reads the first 5 frames fom a given video\n");
    fprintf(stderr, "      file and writes them out in png format.)\n");
    fprintf(stderr, "  (2) write some frames to a scaled output video.\n");
    fprintf(stderr, 
            "\nUSE:\n"
            "     test-vidio <input_video.avi> <output_video.avi>\n\n");
    exit(0);
}




int main(int argc,char **argv) {
    
    if (argc<3) {
        usage();
        exit(255);
    }

    autodel<VideoInput> vidin(make_VideoInput());
    autodel<VideoOutput> vidout(make_VideoOutput());
    vidin->open(argv[1]);
    vidout->open(argv[2], FRAMERATE, SIZEX, SIZEY, QUALITY);

    bytearray red,green,blue;
    char filename[1024];
    FILE *output;

    // test VidIn interface
    // read the first five frames
    // and store them as ppm's
    for (int i=0; i<5; ++i) {
        bool success = vidin->readFrame(red,green,blue);
        assert(success);
        // write to a file
        sprintf(filename, "frame%.4d.ppm", i);
        output = fopen(filename, "w");
        assert(output);
        write_ppm(output,red,green,blue);
        fclose(output);

    }
    
    // test VidOut interface:
    // store the next 10 frames to a video
    for (int i=0; i<10; ++i) {
        bool success = vidin->readFrame(red,green,blue);
        for(int u=0;u<10;u++) for(int v=0;v<10;v++)
            red(i*5+u,i*5+v) = 255;
        assert(success);
        vidout->writeFrame(red,green,blue);
    }

    return 0;
}
