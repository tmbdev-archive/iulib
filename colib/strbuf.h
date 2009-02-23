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
// File: strbuf.h
// Purpose: string buffer
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

#ifndef strbuf_h__
#define strbuf_h__

/// \file strbuf.h
/// \brief String buffer

#include <cstdio>
#include <cstdarg>

namespace colib {

    struct strbuf {
	// FIXME this class is due to be replaced
        char *buf;
        strbuf() {
            buf = 0;
        }
        strbuf(int n) {
            buf = 0;
            ensure(n);
        }
        ~strbuf() {
            dealloc();
        }
        void dealloc() {
            if(buf) free(buf);
            buf = 0;
        }
        void ensure(int n) {
            if(!buf) {
		buf = (char*)malloc(n+1);
		buf[0] = 0;
	    } else {
		n = max(n,(int)strlen(buf));
		buf = (char*)realloc(buf,n+1);
	    }
        }
        void clear() {
            buf[0] = 0;
        }
        void truncate(int n) {
            if(n<0) n = strlen(buf)+n;
            if(n<0) return;
            buf[min(n,(int)strlen(buf))] = 0;
        }
        int length() {
            if(!buf) return 0;
            return strlen(buf);
        }
        operator bool() {
            return !!buf;
        }
        void operator=(const char *src) {
            ensure(strlen(src));
            strcpy(buf,src);
        }
        void operator=(const strbuf &other) {
            *this = other.buf;
        }
        void operator+=(char other) {
            int n = length();
            ensure(n+2);
            buf[n] = other;
            buf[n+1] = 0;
        }
        void operator+=(const char *other) {
            if(!other) return;
            ensure(length()+strlen(other));
            strcat(buf,other);
        }
        void operator+=(const strbuf &other) {
            if(!other.buf) return;
            ensure(length()+strlen(other.buf));
            strcat(buf,other.buf);
        }
        void format(const char *fmt,...) {
            va_list args;
            const int n = 10000;
            ensure(n+1);
            buf[0] = 0;
            va_start(args,fmt);
            vsnprintf(buf,n,fmt,args);
            va_end(args);
        }
        void appendFormat(const char *fmt,...) {
            va_list args;
            int l = length();
            const int n = 10000;
            ensure(l+n+1);
            va_start(args,fmt);
            vsnprintf(buf+l,n,fmt,args);
            va_end(args);
        }
        operator char*() {
	    ensure(1);
            return buf;
        }
        char *ptr() {
	    ensure(1);
            return buf;
        }
        char *take() {
	    ensure(1);
            char *result = buf;
            buf = 0;
            return result;
        }
    };

    struct utf8buf {
        char *buf;
        utf8buf() {
            buf = 0;
        }
        ~utf8buf() {
            dealloc();
        }
        void dealloc() {
            if(buf) free(buf);
            buf = 0;
        }
        void ensure(int n) {
            if(buf) free(buf);
            buf = (char*)malloc(n+1);
        }
        void operator=(const char *src) {
            ensure(strlen(src));
            strcpy(buf,src);
        }
        void operator=(utf8buf &other) {
            *this = other.buf;
        }
        operator char*() {
            return buf;
        }
        char *take() {
            char *result = buf;
            buf = 0;
            return result;
        }
    };

#if 0
    // FIXME add the necessary methods to make this work
    template <class T>
    inline void na_transfer(strbuf &dst,strbuf &src) {
        dst.setPointer(src.take());
    }
#endif
}

#endif /* strbuf_h__ */
