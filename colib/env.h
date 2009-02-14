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
// File: env.h
// Purpose: getting dynamically bound parameters/variables
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file env.h
/// \brief Getting dynamically bound parameters/variables.

#ifndef h_iupr_env_
#define h_iupr_env_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
using namespace std;

namespace colib {

    // FIXME: should add code to...
    // -- specify an environment variable prefix for each program: env_register_prefix(...)
    // -- specify the set of options recognized by a program/library: env_register_option(...)
    // -- warn about environment variables with the given prefix but not recognized

    inline const char *getenv_(const char *name,const char *dflt) {
        const char *result = getenv(name)?getenv(name):dflt;
        return result;
    }

    class param_ {
        const char *name;
        const char *doc;
        bool once;
    public:
        param_(const char *name,const char *doc):name(name),doc(doc) {
            once = false;
        }
        void notify(const char *fmt,...) {
            if(once) return;
            int where = atoi(getenv_("verbose","0"));
            if(!where) return;
            FILE *stream = stdout;
            if(where==2) stream = stderr;
            fprintf(stream,"param %s=",name);
            va_list args;
            va_start(args,fmt);
            vfprintf(stream,fmt,args);
            va_end(args);
            fprintf(stream," # %s\n",doc);
            once = true;
        }
    };

    class param_string : param_ {
    private:
        const char *value;
    public:
        param_string(const char *name,const char *dflt,const char *doc) : param_(name,doc) {
            value = getenv(name)?getenv(name):dflt;
        }
        operator const char*() {
            notify("%s",value);
            return value;
        }
    };

    class param_bool : param_ {
    private:
        bool value;
    public:
        param_bool(const char *name,bool dflt,const char *doc) : param_(name,doc) {
            value = getenv(name)?atoi(getenv(name)):dflt;
        }
        operator bool() {
            notify("%d",value);
            return value;
        }
    };

    class param_int : param_ {
    private:
        int value;
    public:
        param_int(const char *name,int dflt,const char *doc) : param_(name,doc) {
            value = getenv(name)?atoi(getenv(name)):dflt;
        }
        operator int() {
            notify("%d",value);
            return value;
        }
    };

    class param_float : param_ {
    private:
        float value;
    public:
        param_float(const char *name,float dflt,const char *doc) : param_(name,doc) {
            value = getenv(name)?atof(getenv(name)):dflt;
        }
        operator float() {
            notify("%g",value);
            return value;
        }
    };

    class param_double : param_ {
    private:
        double value;
    public:
        param_double(const char *name,double dflt,const char *doc) : param_(name,doc) {
            value = getenv(name)?atof(getenv(name)):dflt;
        }
        operator double() {
            notify("%g",value);
            return value;
        }
    };
}

#endif
