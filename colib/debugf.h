// -*- C++ -*-

// Copyright 2006-2008 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
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
// Project: OCRopus
// File: struct.h
// Purpose: A collection of useful data structures
// Responsible: Faisal Shafait (faisal.shafait@dfki.de)
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

#ifndef debugf_h_
#define debugf_h_

namespace {
    void iprintf(FILE *stream,int depth,const char *fmt,...) {
        fprintf(stream,"%*s",depth,"");
        va_list args;
        va_start(args,fmt);
        vfprintf(stream,fmt,args);
        va_end(args);
    }

    bool strflag(const char *list,const char *key) {
        while(*list) {
            // at the beginning of a potential match here
            bool match = true;
            const char *p = key;
            while(*list && *list!=',' && *p) {
                if(*list++!=*p++) {
                    match = false;
                    break;
                }
            }
            // check that the match ended with a separator
            if(match&&(!*list||*list==',')&&!*p)
                return true;
            // advance to the next separator
            while(*list && *list!=',')
                list++;
            if(*list==',')
                list++;
        }
        return false;
    }

    bool debug(const char *which) {
        const char *env = getenv("debug");
        if(!env) env = "info";
        return strflag(env,which);
    }

    void debugf(const char *which,const char *fmt,...) {
        if(!debug(which)) return;
        va_list args;
        fprintf(stderr,"[%s] ",which);
        va_start(args,fmt);
        vfprintf(stderr,fmt,args);
        va_end(args);
    }
}

#endif
