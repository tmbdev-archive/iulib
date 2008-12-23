// -*- C++ -*-

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
// Project:
// File:
// Purpose:
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de, www.ocropus.org

/// \file nustring.h
/// \brief Unicode strings.

#ifndef nustring_h__
#define nustring_h__

#include <stdlib.h>
#include <string.h>
#include "colib/narray.h"


// Normalized Unicode characters.  These are 32bit code points, but only a subset
// of those defined by Unicode are permitted.  Roughly, only codepoints corresponding
// to actual glyphs are permitted; no combining characters, no control characters other than
// whitespace, etc.
//
// FIXME add checks to exclude oddball character

namespace colib {
    struct nuchar {
        int value;
        nuchar() {
            value = 0;
        }
        nuchar(const nuchar &c) {
            value = c.value;
        }
        explicit nuchar(int x) {
            value = x;
        }
        int ord() {
            return value;
        }
        bool operator==(const nuchar &c) {
            return value==c.value;
        }
        bool operator!=(const nuchar &c) {
            return value!=c.value;
        }

    };

    // Normalized Unicode string.  This is a UTF-32 string with no control character
    // other than whitespace (newline, etc.), combining characters, changes in writing
    // direction, etc.
    //
    // FIXME add checks to exclude oddball character

    namespace {
        static int encode_nuchar(narray<char> &utf8, nuchar c) {
            if(c.value < 0) {
                return -1;
            } if(c.value < 0x80) {
                utf8.push(c.value);
                return 1;
            } else if(c.value < 0x800) {
                utf8.push(0xC0 | ((c.value >> 6) & 0x1F));
                utf8.push(0x80 | (c.value & 0x3F));
                return 2;
            } else if(c.value < 0x10000) {
                utf8.push(0xE0 | ((c.value >> 12) & 0xF));
                utf8.push(0x80 | ((c.value >> 6) & 0x3F));
                utf8.push(0x80 | (c.value & 0x3F));
                return 3;
            } else if(c.value < 0x110000) {
                utf8.push(0xF0 | ((c.value >> 18) & 7));
                utf8.push(0x80 | ((c.value >> 12) & 0x3F));
                utf8.push(0x80 | ((c.value >> 6) & 0x3F));
                utf8.push(0x80 | (c.value & 0x3F));
                return 4;
            } else {
                return -1;
            }
        }

        static int nustring_length(unsigned char a) {
            if(a < 128)
                return 1;
            int i;
            for(i = 0; i < 8; i++) {
                if(!((a << i) & 0x80))
                    break;
            }
            return i;
        }

        static bool decode_nuchar(nuchar &c, int &offset, const char *utf8,int n) {
            unsigned char a = utf8[offset];
            int length = nustring_length(a);
            if(offset + length > n || length < 1 || length > 4)
                return false;
            if(a < 128) {
                if(length!=1) throw "bad encoding";
                c = nuchar(a);
                offset += length;
                return true;
            }
            int result = (0x7F >> length) & a;
            if(a >> 6 != 3)
                return false;
            for(int i = 1; i < length; i++) {
                a = utf8[offset + i];
                if(a >> 6 != 2)
                    return false;
                result <<= 6;
                result |= a & 0x3F;
            }
            c = nuchar(result);
            offset += length;
            return true;
        }
    }

    class nustring : public narray<nuchar> {
    public:
        nustring() {
        }
        nustring(int n):narray<nuchar>(n) {
        }
        nustring(const char *s) {
            utf8Decode(s,strlen(s));
        }
        bool operator==(nustring &s){
            if(length()!=s.length()) return 0;
            for(int i=0;i<length();i++)
                if(at(i)!=s[i]) return 0;
            return 1;
        }
        bool operator==(const char *s){
            nustring sn(s);
            return *this==sn;
        }
        bool operator!=(nustring &s){
            return !(*this==s);
        }
        bool operator!=(const char *s){
            return !(*this==s);
        }
        void of(narray<int> &data) {
            resize(data.length());
            for(int i=0;i<data.length();i++) at(i) = nuchar(data[i]);
        }
        void as(narray<int> &data) {
            data.resize(length());
            for(int i=0;i<length();i++) data[i] = at(i).ord();
        }
        int utf8Length() {
            narray<char> temp;
            utf8Encode(temp);
            return temp.length();
        }
        void utf8Encode(narray<char> &s) {
            s.clear();
            for(int i=0;i<length();i++) {
                int l = encode_nuchar(s,at(i));
                if(l<0) throw "bad nustring";
            }
        }
        void utf8Decode(const char *s,int n) {
            if(n<0) n = strlen(s);
            int offset = 0;
            nuchar c;
            clear();
            while(offset < n && decode_nuchar(c,offset,s,n)) {
                push(c);
            }
            if(offset<n) throw "bad utf8 encoding";
        }
        void utf8Decode(narray<char> &str) {
            utf8Decode(&str[0],str.length());
        }
        void utf8Encode(char *result,int size) {
            narray<char> temp;
            utf8Encode(temp);
            if(temp.length()>=size) throw "not enough room";
            strncpy(result,&temp[0],temp.length());
            result[temp.length()] = 0;
        }
        char *mallocUtf8Encode() {
            narray<char> temp;
            utf8Encode(temp);
            int size = temp.length();
            char *result = (char *)malloc(size+1);
            if(size>0) strncpy(result,&temp[0],size);
            result[size] = 0;
            return result;
        }
        char *newUtf8Encode() {
            narray<char> temp;
            utf8Encode(temp);
            int size = temp.length();
            char *result = new char[size+1];
            if(size>0) strncpy(result,&temp[0],size);
            result[size] = 0;
            return result;
        }

    };

    // convenience functions for intarrays

    inline void utf8_decode(intarray &result,const char *s,int n=-1) {
        if(n<0) n = strlen(s);
        int offset = 0;
        nuchar c;
        result.clear();
        while(decode_nuchar(c,offset,s,n))
            result.push(c.value);
        if(offset<n) throw "bad utf8 encoding";
    }

    inline void utf8_encode(narray<char> &s,intarray &a) {
        s.clear();
        for(int i=0;i<a.length();i++) {
            int l = encode_nuchar(s,nuchar(a(i)));
            if(l<0) throw "bad nustring";
        }
    }

    inline void utf8_encode(bytearray &s,intarray &a) {
        narray<char> temp;
        utf8_encode(temp,a);
        copy(s,temp);
    }

    inline const char *malloc_utf8_encode(intarray &data) {
        narray<char> temp;
        utf8_encode(temp,data);
        int size = temp.length();
        char *result = (char *)malloc(size+1);
        if(size>0) strncpy(result,&temp[0],size);
        result[size] = 0;
        return result;
    }
}

#endif /* nustring_h__ */

