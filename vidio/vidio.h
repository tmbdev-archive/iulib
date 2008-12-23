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
// Project: vidio -- reading and writing video streams
// File: vidio.h
// Purpose: interface to vidio.cc
// Responsible: ulges
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#ifndef h_vidio__
#define h_vidio__

#include "colib/colib.h"

namespace iulib {
    using namespace colib;
    
    struct VideoInput {
        virtual void set(const char *key,const char *value) { 
            throw "VideoInput::set(char*,char*) unimplemented by subclass";
        }
        virtual void set(const char *key,double value) { 
            throw "VideoInput::set(char*,double) unimplemented by subclass";
        }
        virtual void open(const char *file) = 0;
        virtual void rewind() { throw "rewind() unimplemented"; }
        virtual void start() { }
        virtual void pause() { throw "pause() unimplemented"; }
        virtual bool readFrameGray(bytearray &frame) = 0;
        virtual bool readFrame(bytearray &rgb) = 0;
        virtual bool readFrame(bytearray &red,
                               bytearray &green,
                               bytearray &blue) = 0;
        virtual ~VideoInput() {}
    };

    struct VideoOutput {
        virtual void open(char *filename, 
                          int framerate=25,
                          int framewidth=320,
                          int frameheight=240,
                          int compr=3) = 0;
        virtual bool writeFrame(bytearray &red,
                                bytearray &green,
                                bytearray &blue) = 0;
        virtual ~VideoOutput() {}
    };
    VideoInput *make_VideoInput();
    VideoOutput *make_VideoOutput();
    VideoInput *make_V4L2Capture();
}

#endif
