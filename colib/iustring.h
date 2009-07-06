// -*- C++ -*-

// Copyright 2009 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
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
// File: iustring.h
// Purpose: iu string
// Responsible: remat
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

#ifndef iustring_h__
#define iustring_h__

/// \file iustring.h
/// \brief iu String

#include <cstdio>
#include <cstdarg>
#include <limits.h>
#include <regex.h>

#include "narray.h"
#include "nustring.h"

namespace colib {

    /**
     * @brief counted string class based on narray
     * implements most methods of std::string with same arguments
     */
    template<class T> class iustrg {
    public:
        iustrg() : len(0) {
        }
        iustrg(int n) : buf(n), len(0) {
            if(n > 0) {
                buf.at(0) = '\0';
            }
        }
        template<class A>
        iustrg(const iustrg<A>& src) : len(0) {
            append(src);
        }
        iustrg(const char* src) : len(0) {
            append(src);
        }
        ~iustrg() {
        }
        int length() const {
            return len;
        }
        int size() const {
            return length();
        }
        int max_size() const {
            return INT_MAX;
        }
        void clear() {
            buf.dealloc();
            len = 0;
        }
        bool empty() const {
            return len == 0;
        }
        const T& operator[](int pos) const {
            return at(pos);
        }
        T& operator[](int pos) {
            return at(pos);
        }
        const T& at(int pos) const {
            if(pos < 0 || unsigned(pos) >= unsigned(len)) {
                throw "out of bounds";
            }
            return buf(pos);
        }
        T& at(int pos) {
            if(pos < 0 || unsigned(pos) >= unsigned(len)) {
                throw "out of bounds";
            }
            return buf(pos);
        }
        iustrg<T>& append(const char* s, int pos, int n) {
            for(int i=pos; i<pos+n && s[i]!='\0'; i++) {
                push_back(s[i]);
            }
            return *this;
        }
        iustrg<T>& append(const char* s, int n) {
            return append(s, 0, n);
        }
        iustrg<T>& append(const char* s) {
            return append(s, 0, strlen(s));
        }
        template <class A>
        iustrg<T>& append(const iustrg<A>& str, int pos, int n) {
            for(int i=pos; i<pos+n && str[i]!='\0'; i++) {
                push_back(str[i]);
            }
            return *this;
        }
        template <class A>
        iustrg<T>& append(const iustrg<A>& str, int n) {
            return append(str, 0, n);
        }
        template <class A>
        iustrg<T>& append(const iustrg<A>& str) {
            return append(str, 0, str.length());
        }
        iustrg<T>& append(int n, T c) {
            for(int i=0; i<n; i++) {
                push_back(c);
            }
        }
        iustrg<T>& append(int x) {
            sprintf_append(*this, "%d", x);
            return *this;
        }
        iustrg<T>& append(long x) {
            sprintf_append(*this, "%ld", x);
            return *this;
        }
        iustrg<T>& append(double x) {
            sprintf_append(*this, "%f", x);
            return *this;
        }
        template <class A>
        iustrg<T>& operator+=(const A& s) {
            return append(s);
        }
        void push_back(T c) {
            buf.grow_to(len + 2); // +1 new char, +1 terminating \0
            buf.at(len) = c;
            buf.at(len+1) = T('\0');
            len++;
        }
        iustrg<T>& assign(const char *s, int pos, int n) {
            clear();
            return append(s, pos, n);
        }
        iustrg<T>& assign(const char* s, int n) {
            return assign(s, 0, n);
        }
        iustrg<T>& assign(const char* s) {
            return assign(s, 0, strlen(s));
        }
        iustrg<T>& assign(const iustrg<T>& str, int pos, int n) {
            clear();
            return append(str, pos, n);
        }
        iustrg<T>& assign(const iustrg<T>& str, int n) {
            return assign(str, 0, n);
        }
        iustrg<T>& assign(const iustrg<T>& str) {
            return assign(str, 0, str.length());
        }
        iustrg<T>& assign(int n, T c) {
            clear();
            return append(n, c);
        }
        template<class A>
        iustrg<T>& assign(const A& x) {
            clear();
            return append(x);
        }
        template<class A>
        iustrg<T>& operator=(const A& x) {
            return assign(x);
        }
        iustrg<T>& operator=(const char* s) {
            return assign(s);
        }
        iustrg<T>& replace(int pos, int n1, const char* s, int n2) {
            iustrg<T> tmp;
            tmp.append(*this, pos);
            tmp.append(s, n2);
            tmp.append(*this, pos+n1, length() - pos - n1);
            return assign(tmp);
        }
        iustrg<T>& replace(int pos, int n1, const char* s) {
            return replace(pos, n1, s, strlen(s));
        }
        iustrg<T>& replace(int pos, int n1, const iustrg<T>& str, int n2) {
            iustrg<T> tmp;
            tmp.append(*this, pos);
            tmp.append(str, n2);
            tmp.append(*this, pos+n1, length() - pos - n1);
            return assign(tmp);
        }
        iustrg<T>& replace(int pos, int n1, const iustrg<T>& str) {
            return replace(pos, n1, str.buf, str.length());
        }
        iustrg<T>& replace(int pos, int n1, int n2, T c) {
            iustrg<T> tmp;
            tmp.append(*this, pos);
            tmp.append(n2, c);
            tmp.append(*this, pos+n1, length() - pos - n1);
            return assign(tmp);
        }
        iustrg<T>& insert(int pos, const char *s, int n) {
            return replace(pos, 0, s, n);
        }
        iustrg<T>& insert(int pos, const char *s) {
            return insert(pos, s,   strlen(s));
        }
        iustrg<T>& insert(int pos, const iustrg<T>& str, int n) {
            return replace(pos, 0, str, n);
        }
        iustrg<T>& insert(int pos, const iustrg<T>& str) {
            return insert(pos, str, str.length());
        }
        iustrg<T>& insert(int pos, int n, T c) {
            return replace(pos, 0, n, c);
        }
        iustrg<T>& erase(int pos, int n) {
            return replace(pos, n, (const char*)0, 0);
        }
        iustrg<T>& erase(int pos) {
            return erase(pos, length()-pos);
        }
        int copy(char* dst, int n, int pos=0) const {
            int nCpy = 0;
            for(int i=pos; i<pos+n && i<len; i++) {
                dst[i] = (char)at(i);
                nCpy++;
            }
            return nCpy;
        }
        void copy_string(char* dst, int pos=0) const {
            for(int i=pos; i<len; i++) {
                dst[i] = (char)at(i);
            }
            dst[len] = '\0';
        }
        void swap(iustrg<T>& str) {
            narray<T> tmp;
            tmp.move(buf);
            buf.move(str.buf);
            str.buf.move(tmp);
        }
        int compare(int pos1, int n1, const iustrg<T>& str, int pos2, int n2) const {
            n1 = min(len-pos1, n1);
            n2 = min(str.length()-pos2, n2);
            for(int i=0; i<n1 && i<n2; i++) {
                if(at(pos1+i) > str[pos2+i]) {
                    return 1;
                } else if(at(pos1+i) < str[pos2+i]) {
                    return -1;
                }
            }
            if(n1 > n2) {
                return 1;
            } else if(n1 < n2) {
                return -1;
            } else {
                return 0;
            }
        }
        int compare(int pos1, int n1, const iustrg<T>& str) const {
            return compare(pos1, n1, str, 0, str.length());
        }
        int compare(const iustrg<T>& str) const {
            return compare(0, len, str, 0, str.length());
        }
        int compare(int pos1, int n1, const char* s, int pos2, int n2) const {
            n1 = min(len-pos1, n1);
            for(int i=0; i<n1 && i<n2; i++) {
                if(at(pos1+i) > s[pos2+i]) {
                    return 1;
                } else if(at(pos1+i) < s[pos2+i]) {
                    return -1;
                }
            }
            if(n1 > n2) {
                return 1;
            } else if(n1 < n2) {
                return -1;
            } else {
                return 0;
            }
        }
        int compare(int pos1, int n1, const char* s) const {
            return compare(pos1, n1, s, 0, strlen(s));
        }
        int compare(const char* s) const {
            return compare(0, len, s, 0, strlen(s));
        }
        iustrg<T> substr(int pos, int n) const {
            return iustrg<T>().append(*this, pos, n);
        }
        iustrg<T> substr(int pos) const {
            return substr(pos, len-pos);
        }
        const T* c_str() {
            return buf.data;
        }
        operator const T*() {
            return c_str();
        }
        operator bool() {
            return !empty();
        }
        narray<T>& data() const {
            return buf;
        }
        int find(const iustrg<T>& str, int pos=0) const {
            pos = limit(0, len-1, pos);
	      for(int i=pos; i<=len-str.length(); i++) {
                if(compare(i, str.length(), str) == 0) {
                    return i;
                }
            }
            return npos;
        }
        int find(const char* s, int pos, int n) const {
            pos = limit(0, len-1, pos);
            for(int i=pos; i<=len-n; i++) {
                if(compare(i, n, s, 0, n) == 0) {
                    return i;
                }
            }
            return npos;
        }
        int find(const char* s, int pos=0) const {
            return find(s, pos, strlen(s));
        }
        int find(T c, int pos=0) const {
            pos = limit(0, len-1, pos);
            for(int i=pos; i<len; i++) {
                if(at(i) == c) {
                    return i;
                }
            }
            return npos;
        }
        int rfind(const iustrg<T>& str, int pos=npos) const {
            if(pos < 0) {
                pos = len-1;
            } else {
                pos = limit(0, len-1, pos);
            }
            for(int i=pos; i>=0; i--) {
                if(compare(i, str.length(), str) == 0) {
                    return i;
                }
            }
            return npos;
        }
        int rfind(const char* s, int pos, int n) const {
            if(pos < 0) {
                pos = len-1;
            } else {
                pos = limit(0, len-1, pos);
            }
            for(int i=pos; i>=0; i--) {
                if(compare(i, n, s, 0, n) == 0) {
                    return i;
                }
            }
            return npos;
        }
        int rfind(const char* s, int pos=npos) const {
            return rfind(s, pos, strlen(s));
        }
        int rfind(T c, int pos=npos) const {
            if(pos < 0) {
                pos = len-1;
            } else {
                pos = limit(0, len-1, pos);
            }
            for(int i=pos; i>=0; i--) {
                if(at(i) == c) {
                    return i;
                }
            }
            return npos;
        }

        static const int npos = -1;

        inline static int limit(int minV, int maxV, int value) {
            return max(minV, min(maxV, value));
        }

        narray<T>& getBuf() {
            return buf;
        }

        // -- nustring compatibility -
        iustrg(iustrg<nuchar>& src) : len(0) {
            append(src);
        }
        iustrg<T>& append(iustrg<nuchar>& str) {
            for(int i=0; i<str.length() && str[i].ord()!='\0'; i++) {
                push_back(str[i].ord());
            }
            return *this;
        }
        void toNustring(iustrg<nuchar>& dst) {
            dst.clear();
            for(int i=0; i<len; i++) {
                dst.push_back(nuchar(buf(i)));
            }
        }

    protected:
        narray<T> buf;    /// the actual characters
        int len;          /// length of the string
    };

    typedef iustrg<char> strg;
    typedef iustrg<u_int32_t> ustrg;

    typedef strg iucstring;
    typedef ustrg iuistring;

    template<class T>
    inline static iustrg<T> operator+(const iustrg<T>& s1, const iustrg<T>& s2) {
        iustrg<T> s;
        s.append(s1);
        s.append(s2);
        return s;
    }
    template<class T, class A>
    inline static iustrg<T> operator+(const iustrg<T>& s1, const A& s2) {
        iustrg<T> s;
        s.append(s1);
        s.append(s2);
        return s;
    }
    template<class T, class A>
    inline static iustrg<T> operator+(const A& s1, const iustrg<T>& s2) {
        iustrg<T> s;
        s.append(s1);
        s.append(s2);
        return s;
    }
    template<class T>
    inline static bool operator==(const iustrg<T>& s1, const iustrg<T>& s2) {
        return s1.compare(s2) == 0;
    }
    template<class T>
    inline static bool operator==(const iustrg<T>& s1, const char* s2) {
        return s1.compare(s2) == 0;
    }
    template<class T>
    inline static bool operator==(const char* s1, const iustrg<T>& s2) {
        return s2.compare(s1) == 0;
    }
    template<class T>
    inline static bool operator!=(const iustrg<T>& s1, const iustrg<T>& s2) {
        return !operator==(s1, s2);
    }
    template<class T>
    inline static bool operator!=(const iustrg<T>& s1, const char* s2) {
        return !operator==(s1, s2);
    }
    template<class T>
    inline static bool operator!=(const char* s1, const iustrg<T>& s2) {
        return !operator==(s1, s2);
    }
    template<class T>
    inline int sprintf(iustrg<T>& str, const char *format, ...) {
        int maxLen = 64;
        char* tmp = NULL;
        int result = 0;
        va_list va;
        do {
            maxLen *= 2;
            tmp = (char*)realloc(tmp, maxLen+1);
            va_start(va, format);
            result = vsnprintf(tmp, maxLen, format, va);
            va_end(va);
        } while(result >= maxLen);
        str.assign(tmp);
        free(tmp);
        return result;
    }
    template<class T>
    inline int sprintf_append(iustrg<T>& str, const char *format, ...) {
        int maxLen = 64;
        char* tmp = NULL;
        int result = 0;
        va_list va;
        do {
            maxLen *= 2;
            tmp = (char*)realloc(tmp, maxLen+1);
            va_start(va, format);
            result = vsnprintf(tmp, maxLen, format, va);
            va_end(va);
        } while(result >= maxLen);
        str.append(tmp);
        free(tmp);
        return result;
    }
    template<class T>
    inline int scanf(iustrg<T>& str, const char *format, ...) {
        const char* buf = str.c_str();
        va_list va;
        va_start(va, format);
        int result = vsscanf(buf, format, va);
        va_end(va);
        return result;
    }
    template<class T>
    inline iustrg<T>& fgets(iustrg<T>& str, FILE* stream = stdin) {
        int c;
        while(((c = fgetc(stream)) != EOF) && (c != '\n')) {
            str.push_back(c);
        }
        return str;
    }
    template<class T>
    inline int fputs(const iustrg<T>& str, FILE* stream = stdout) {
        for(int i=0; i<str.length(); i++) {
            if(fputc(str[i], stream) == EOF) {
                return EOF;
            }
        }
        if(fputc('\n', stream) == EOF) {
            return EOF;
        }
        return str.length() + 1;
    }
    template<class T>
    inline int read(iustrg<T>& str, int n, FILE* stream) {
        T c;
        int i = 0;
        while((i < n) && (fread(&c, sizeof(T), 1, stream) == 1)) {
            str.push_back(c);
            i++;
        }
        if(ferror(stream)) {
            i *= -1;
        }
        return i;
    }
    template<class T>
    inline int fread(iustrg<T>& str, FILE* stream) {
        return read(str, INT_MAX, stream);
    }
    template<class T>
    inline int write(iustrg<T>& str, int n, FILE* stream) {
        n = iustrg<T>::limit(0, str.length(), n);
        int i = 0;
        while((i < n) && (fwrite(&str[i], sizeof(T), 1, stream) == 1)) {
            i++;
        }
        return i;
    }
    template<class T>
    inline int fwrite(iustrg<T>& str, FILE* stream) {
        return write(str, str.length(), stream);
    }

    inline void re_compile(regex_t* regex, const char* pattern, int cflags=0, int eflags=0) {
        int error = regcomp(regex, pattern, cflags);
        if(error) {
            regfree(regex);
            char errMsg[256];
            regerror(error, regex, errMsg, 255);
            throw errMsg;
        }
    }
    inline int re_search(const strg& str, const char* pattern, int cflags=0, int eflags=0) {
        regex_t regex;
        re_compile(&regex, pattern, cflags, eflags);
        char* buf = (char*)malloc(str.length()+1);
        str.copy_string(buf);
        regmatch_t regmatch;
        int index = -1;
        int error;
        if((error = regexec(&regex, buf, 1, &regmatch, eflags)) == 0) {
            index = regmatch.rm_so;
        }
        regfree(&regex);
        free(buf);
        if(error == REG_ESPACE) {
            throw "out of memory";
        }
        return index;
    }
    inline int re_gsub(strg& str, const char* pattern, const char* sub, int n = -1, int cflags=0, int eflags=0) {
        regex_t regex;
        re_compile(&regex, pattern, cflags, eflags);
        const char* buf = str.c_str();
        strg result;
        regmatch_t regmatch;
        int s = 0;
        int nMatches = 0;
        int error = 0;
        while((n<0 || nMatches<n) && ((error = regexec(&regex, buf + s, 1, &regmatch, eflags)) == 0)) {
            result.append(str.substr(s, regmatch.rm_so));
            result.append(sub);
            eflags = REG_NOTBOL;
            s += regmatch.rm_eo;
            nMatches++;
        }
        result.append(str.substr(s));
        regfree(&regex);
        if(error == REG_ESPACE) {
            throw "out of memory";
        }
        str.assign(result);
        return nMatches;
    }
    inline int re_sub(strg& str, const char* pattern, const char* sub, int cflags=0, int eflags=0) {
        return re_gsub(str, pattern, sub, 1, cflags, eflags);
    }

    class EncodedString : protected bytearray {
    protected:
        EncodedString() {}
    public:
        void clear() {
            return bytearray::clear();
        }
        int length() {
            return bytearray::length();
        }
        bool equal(EncodedString& other) {
            return bytearray::equal(other);
        }
        virtual void pushUnicode(unsigned int c) = 0;
        virtual unsigned int getUnicode(int& i) = 0;
        void fromUnicode(const ustrg& src) {
            for(int i=0; i<src.length(); i++) {
                pushUnicode(src[i]);
            }
        }
        void toUnicode(ustrg& dst) {
            dst.clear();
            int i = 0;
            while(i<dim(0)-1) {
                dst.push_back(getUnicode(i));
            }
        }
        int fwrite(FILE *file) {
            return ::fwrite(data, 1, dim(0), file);
        }
        int fread(FILE *file) {
            unsigned char c;
            int i = 0;
            while(::fread(&c, 1, 1, file) == 1) {
                push(c);
                i++;
            }
            return i;
        }
        EncodedString& fgets(FILE* stream = stdin) {
            int c;
            while(((c = fgetc(stream)) != EOF) && (c != '\n')) {
                push(c);
            }
            return *this;
        }
        int fputs(FILE* stream = stdout) {
            for(int i=0; i<length(); i++) {
                if(fputc(at(i), stream) == EOF) {
                    return EOF;
                }
            }
            if(fputc('\n', stream) == EOF) {
                return EOF;
            }
            return length() + 1;
        }
        template<class T>
        void copyTo(narray<T>& dst) {
            dst.copy(*this);
        }
    };

    class utf8strg : public EncodedString {
    public:
        void pushUnicode(unsigned int c) {
            // -- one byte --
            if(c < 128) {
                push(c);
            // -- two bytes --
            } else if(c < 2048) {
                push(0xC0 | (c >> 6));
                push(0x80 | (c & 0x3F));
            // -- three bytes --
            } else if(c < 65536) {
                push(0xE0 | (c >> 12));
                push(0x80 | ((c >> 6) & 0x3F));
                push(0x80 | (c & 0x3F));
            // -- four bytes --
            } else if(c < 2097152) {
                push(0xF0 | (c >> 18));
                push(0x80 | ((c >> 12) & 0x3F));
                push(0x80 | ((c >> 6) & 0x3F));
                push(0x80 | (c & 0x3F));
            } else {
                throw "UTF-8 encoding error";
            }
        }
        unsigned int getUnicode(int& i) {
            return decode(*this, i);
        }
        template<class T>
        static unsigned int decode(T& str, int& i) {
            unsigned int x = 0;
            int b = -1;
            while(b != 0) {
                unsigned char c = str[i++];
                // -- ASCII --
                if(c < 128) {
                    x = c;
                    b = 0;
                // -- not first byte --
                } else if(c < 0xC0) {
                    if(b<=0) {
                        throw "UTF-8 decoding error";
                    }
                    x += (c & 0x3F) << (6*(b-1));
                    b--;
                // -- first of two bytes --
                } else if(c < 0xE0) {
                    x = (c & 0x1F) << 6;
                    b = 1;
                // -- first of three bytes --
                } else if(c < 0xF0) {
                    x = (c & 0xF) << 12;
                    b = 2;
                // -- first of four bytes --
                } else {
                    x = (c & 0x7) << 18;
                    b = 3;
                }
            }
            return x;
        }
    };

    inline int fwrite(EncodedString& s, FILE *file) {
        return s.fwrite(file);
    }
    inline int fread(EncodedString& s, FILE *file) {
        return s.fread(file);
    }
    inline EncodedString& fgets(EncodedString& s, FILE *file) {
        return s.fgets(file);
    }
    inline int fputs(EncodedString& s, FILE *file) {
        return s.fputs(file);
    }
    inline int freadUTF8(ustrg& s, FILE* file) {
        utf8strg utf8;
        int r = fread(utf8, file);
        utf8.toUnicode(s);
        return r;
    }
    inline ustrg& fgetsUTF8(ustrg& s, FILE *file) {
        utf8strg utf8;
        fgets(utf8, file);
        utf8.toUnicode(s);
        return s;
    }
    inline int fwriteUTF8(ustrg& s, FILE* file) {
        utf8strg utf8;
        utf8.fromUnicode(s);
        return fwrite(utf8, file);
    }
    inline int fputsUTF8(ustrg& s, FILE* file) {
        utf8strg utf8;
        utf8.fromUnicode(s);
        return fputs(utf8, file);
    }

#if 0
    class utf16strg : public EncodedString {
    public:
        void pushUnicode(unsigned int c) {
            // -- 2 bytes --
            if(c <= 0xFFFF) {
                push(c & 0xFF);
                push(c >> 8);
            // -- 4 bytes --
            } else {
                c -= 0x10000;
                push(0xDC00 | (c & 0x3FF));
                push(0xD800 | ((c >> 10) & 0x3FF));
            }
        }
        unsigned int getUnicode(int& i) const {
            return decode(*this, i);
        }
        template<class T>
        static unsigned int decode(T str, int& i) {
            unsigned int x = 0;
            unsigned int y = 0;
            int b = 0;
            do {
                unsigned char c = str[i++];
                if(b == 0) {
                    x = c;
                } else if(b == 1) {
                    x |= ((unsigned int)c) << 8;
                    if(x < 0xD800 || x > 0xDFFF) {
                        b = -1;
                    } else if(x < 0xD800 || x > 0xDBFF) {
                        throw "UTF-16 decoding error";
                    }
                } else if(b == 2) {
                    y = c;
                } else if(b == 3) {
                    y |= ((unsigned int)c) << 8;
                    if(y < 0xDC00 || y > 0xDFFF) {
                        throw "UTF-16 decoding error";
                    }
                    x = (((x & 0x3FF) << 10) | (y & 0x3FF)) + 0x10000;
                    b = -1;
                }
                b++;
            } while(b != 0);
            return x;
        }
    };
#endif


    class nustring : public iustrg<nuchar> {
    public:
        nustring(const char* src)  {
            for(int i=0; i<strlen(src); i++) {
                push_back(nuchar(src[i]));
            }
        }
        nustring()  {
        }
        nustring(const nustring& other)  {
            for(int i=0; i<other.length();) {
                push_back(other[i]);
            }
        }
        void of(intarray &data) {
            throw "void of(intarray &data)";
            clear();
            for(int i=0;i<data.length();i++) {
                push_back(nuchar(data[i]));
            }
        }
        void as(intarray &data) {
            throw "void as(intarray &data)";
            data.resize(length());
            for(int i=0;i<length();i++) {
                data[i] = at(i).ord();
            }
        }
        int length1d() {
            return length();
        }
        nuchar& at1d(int index) {
            return (*this)[index];
        }
        void utf8Encode(narray<char> &s) {
            utf8strg utf8;
            for(int i=0; i<length(); i++) {
                utf8.pushUnicode(at(i).ord());
            }
            utf8.copyTo(s);
        }
        void utf8Decode(narray<char> &str) {
            for(int i=0; i<str.length();) {
                push_back(nuchar(utf8strg::decode(str, i)));
            }
        }
        void utf8Encode(char *result,int size) {
            narray<char> temp;
            utf8Encode(temp);
            if(temp.length()>=size) throw "not enough room";
            strncpy(result,&temp[0],temp.length());
            result[temp.length()] = 0;
        }
        void utf8Decode(const char *s,int n) {
            utf8strg utf8;
            int i = 0;
            while(length() < n) {
                push_back(nuchar(utf8strg::decode(s, i)));
            }
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
        void push(const nuchar& x) {
            iustrg<nuchar>::push_back(x);
        }
        void copy(const iustrg<nuchar> &src) {
            clear();
            for(int i=0; i<src.length(); i++) {
                push_back(src[i]);
            }
        }
        const nuchar& operator()(int pos) const {
            return at(pos);
        }
        nuchar& operator()(int pos) {
            return at(pos);
        }
    };

    inline static void copy(nustring &dst, nustring &src) {
        dst.copy(src);
    }
    template<class T>
    inline static void makelike(narray<T>& a, nustring& s) {
        a.makelike(s.getBuf());
    }

}

#endif /* iustring_h__ */
