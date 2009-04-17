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
    template<class T> class iustring {
    public:
        iustring() : len(0) {
        }
        iustring(int n) : buf(n), len(0) {
            if(n > 0) {
                buf.at(0) = '\0';
            }
        }
        iustring(const iustring<T>& src) : len(0) {
            append(src);
        }
        iustring(const char* src) : len(0) {
            append(src);
        }
        iustring(const nustring& src) : len(0) {
            append(src);
        }
        ~iustring() {
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
        iustring<T>& append(const char* s, int pos, int n) {
            for(int i=pos; i<pos+n && s[i]!='\0'; i++) {
                push_back(s[i]);
            }
            return *this;
        }
        iustring<T>& append(const char* s, int n) {
            return append(s, 0, n);
        }
        iustring<T>& append(const char* s) {
            return append(s, 0, strlen(s));
        }
        iustring<T>& append(const iustring<T>& str, int pos, int n) {
            for(int i=pos; i<pos+n && str[i]!='\0'; i++) {
                push_back(str[i]);
            }
            return *this;
        }
        iustring<T>& append(const iustring<T>& str, int n) {
            return append(str, 0, n);
        }
        iustring<T>& append(const iustring<T>& str) {
            return append(str, 0, str.length());
        }
        iustring<T>& append(int n, T c) {
            for(int i=0; i<n; i++) {
                push_back(c);
            }
        }
        iustring<T>& append(int x) {
            sprintf_append(*this, "%d", x);
            return *this;
        }
        iustring<T>& append(long x) {
            sprintf_append(*this, "%ld", x);
            return *this;
        }
        iustring<T>& append(double x) {
            sprintf_append(*this, "%f", x);
            return *this;
        }
        iustring<T>& append(const nustring& str) {
            for(int i=0; i<str.dim(0); i++) {
                push_back(str(i).ord());
            }
            return *this;
        }
        template <class A>
        iustring<T>& operator+=(const A& s) {
            return append(s);
        }
        void push_back(T c) {
            buf.grow_to(len + 2); // +1 new char, +1 terminating \0
            buf.at(len) = c;
            buf.at(len+1) = '\0';
            len++;
        }
        iustring<T>& assign(const char *s, int pos, int n) {
            clear();
            return append(s, pos, n);
        }
        iustring<T>& assign(const char* s, int n) {
            return assign(s, 0, n);
        }
        iustring<T>& assign(const char* s) {
            return assign(s, 0, strlen(s));
        }
        iustring<T>& assign(const iustring<T>& str, int pos, int n) {
            clear();
            return append(str, pos, n);
        }
        iustring<T>& assign(const iustring<T>& str, int n) {
            return assign(str, 0, n);
        }
        iustring<T>& assign(const iustring<T>& str) {
            return assign(str, 0, str.length());
        }
        iustring<T>& assign(int n, T c) {
            clear();
            return append(n, c);
        }
        template<class A>
        iustring<T>& assign(const A& x) {
            clear();
            return append(x);
        }
        template<class A>
        iustring<T>& operator=(const A& x) {
            return assign(x);
        }
        iustring<T>& replace(int pos, int n1, const char* s, int n2) {
            iustring<T> tmp;
            tmp.append(*this, pos);
            tmp.append(s, n2);
            tmp.append(*this, pos+n1, length() - pos - n1);
            return assign(tmp);
        }
        iustring<T>& replace(int pos, int n1, const char* s) {
            return replace(pos, n1, s, strlen(s));
        }
        iustring<T>& replace(int pos, int n1, const iustring<T>& str, int n2) {
            iustring<T> tmp;
            tmp.append(*this, pos);
            tmp.append(str, n2);
            tmp.append(*this, pos+n1, length() - pos - n1);
            return assign(tmp);
        }
        iustring<T>& replace(int pos, int n1, const iustring<T>& str) {
            return replace(pos, n1, str.buf, str.length());
        }
        iustring<T>& replace(int pos, int n1, int n2, T c) {
            iustring<T> tmp;
            tmp.append(*this, pos);
            tmp.append(n2, c);
            tmp.append(*this, pos+n1, length() - pos - n1);
            return assign(tmp);
        }
        iustring<T>& insert(int pos, const char *s, int n) {
            return replace(pos, 0, s, n);
        }
        iustring<T>& insert(int pos, const char *s) {
            return insert(pos, s,   strlen(s));
        }
        iustring<T>& insert(int pos, const iustring<T>& str, int n) {
            return replace(pos, 0, str, n);
        }
        iustring<T>& insert(int pos, const iustring<T>& str) {
            return insert(pos, str, str.length());
        }
        iustring<T>& insert(int pos, int n, T c) {
            return replace(pos, 0, n, c);
        }
        iustring<T>& erase(int pos, int n) {
            return replace(pos, n, (const char*)0, 0);
        }
        iustring<T>& erase(int pos) {
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
        void swap(iustring<T>& str) {
            narray<T> tmp;
            tmp.move(buf);
            buf.move(str.buf);
            str.buf.move(tmp);
        }
        int compare(int pos1, int n1, const iustring<T>& str, int pos2, int n2) const {
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
        int compare(int pos1, int n1, const iustring<T>& str) const {
            return compare(pos1, n1, str, 0, str.length());
        }
        int compare(const iustring<T>& str) const {
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
        iustring<T> substr(int pos, int n) const {
            return iustring<T>().append(*this, pos, n);
        }
        iustring<T> substr(int pos) const {
            return substr(pos, len-pos);
        }
        const T* c_str() {
            if(!buf.data) return "";
            return buf.data;
        }
        operator const T*() {
            return c_str();
        }
        narray<T>& data() const {
            return buf;
        }
        int find(const iustring<T>& str, int pos=0) const {
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
        int find(char c, int pos=0) const {
            pos = limit(0, len-1, pos);
            for(int i=pos; i<len; i++) {
                if(at(i) == c) {
                    return i;
                }
            }
            return npos;
        }
        int rfind(const iustring<T>& str, int pos=npos) const {
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
        int rfind(char c, int pos=0) const {
            pos = limit(0, len-1, pos);
            for(int i=pos; i>=0; i--) {
                if(at(i) == c) {
                    return i;
                }
            }
            return npos;
        }

        void toNustring(nustring& dst) {
            dst.clear();
            for(int i=0; i<len; i++) {
                dst.push(nuchar(buf(i)));
            }
        }

        static const int npos = -1;

        inline static int limit(int minV, int maxV, int value) {
            return max(minV, min(maxV, value));
        }

    private:
        narray<T> buf;    /// the actual characters
        int len;          /// length of the string
    };

    template<class T>
    inline static iustring<T> operator+(const iustring<T>& s1, const iustring<T>& s2) {
        iustring<T> s;
        s.append(s1);
        s.append(s2);
        return s;
    }
    template<class T, class A>
    inline static iustring<T> operator+(const iustring<T>& s1, const A& s2) {
        iustring<T> s;
        s.append(s1);
        s.append(s2);
        return s;
    }
    template<class T, class A>
    inline static iustring<T> operator+(const A& s1, const iustring<T>& s2) {
        iustring<T> s;
        s.append(s1);
        s.append(s2);
        return s;
    }
    template<class T>
    inline static bool operator==(const iustring<T>& s1, const iustring<T>& s2) {
        return s1.compare(s2) == 0;
    }
    template<class T>
    inline static bool operator==(const iustring<T>& s1, const char* s2) {
        return s1.compare(s2) == 0;
    }
    template<class T>
    inline static bool operator==(const char* s1, const iustring<T>& s2) {
        return s2.compare(s1) == 0;
    }
    template<class T>
    inline static bool operator!=(const iustring<T>& s1, const iustring<T>& s2) {
        return !operator==(s1, s2);
    }
    template<class T>
    inline static bool operator!=(const iustring<T>& s1, const char* s2) {
        return !operator==(s1, s2);
    }
    template<class T>
    inline static bool operator!=(const char* s1, const iustring<T>& s2) {
        return !operator==(s1, s2);
    }
    template<class T>
    inline int sprintf(iustring<T>& str, const char *format, ...) {
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
    inline int sprintf_append(iustring<T>& str, const char *format, ...) {
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
    inline int scanf(iustring<T>& str, const char *format, ...) {
        const char* buf = str.c_str();
        va_list va;
        va_start(va, format);
        int result = vsscanf(buf, format, va);
        va_end(va);
        return result;
    }
    template<class T>
    inline iustring<T>& fgets(iustring<T>& str, FILE* stream = stdin) {
        int c;
        while(((c = fgetc(stream)) != EOF) && (c != '\n')) {
            str.push_back(c);
        }
        return str;
    }
    template<class T>
    inline int fputs(const iustring<T>& str, FILE* stream = stdout) {
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
    inline int read(iustring<T>& str, int n, FILE* stream) {
        T c;
        int i = 0;
        while((i < n) && (fread(&c, sizeof(T), 1, stream) == 1)) {
            str.push_back(c);
            i++;
        }
        if(ferror(stream)) {
            i *= -1;
        }
        return -i;
    }
    template<class T>
    inline int fread(iustring<T>& str, FILE* stream) {
        return read(str, INT_MAX, stream);
    }
    template<class T>
    inline int write(iustring<T>& str, int n, FILE* stream) {
        n = iustring<T>::limit(0, str.length(), n);
        int i = 0;
        while((i < n) && (fwrite(&str[i], sizeof(T), 1, stream) == 1)) {
            i++;
        }
        return i;
    }
    template<class T>
    inline int fwrite(iustring<T>& str, FILE* stream) {
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
    template<class T>
    inline int re_search(const iustring<T>& str, const char* pattern, int cflags=0, int eflags=0) {
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
    template<class T>
    inline int re_gsub(iustring<T>& str, const char* pattern, const char* sub, int n = -1, int cflags=0, int eflags=0) {
        regex_t regex;
        re_compile(&regex, pattern, cflags, eflags);
        const char* buf = str.c_str();
        iustring<T> result;
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
    template<class T>
    inline int re_sub(iustring<T>& str, const char* pattern, const char* sub, int cflags=0, int eflags=0) {
        return re_gsub(str, pattern, sub, 1, cflags, eflags);
    }
    template<class T>
    void encodeUTF8(bytearray& dst, iustring<T>& src) {
        for(int i=0; i<src.length(); i++) {
            T c = src[i];
            if(c < 128) {
                dst.push(c);
            } else if(c < 2048) {
                dst.push(0xC0 | (c >> 6));
                dst.push(0x80 | (c & 0x3F));
            } else if(c < 65536) {
                dst.push(0xE0 | (c >> 12));
                dst.push(0x80 | ((c >> 6) & 0x3F));
                dst.push(0x80 | (c & 0x3F));
            } else if(c < 2097152) {
                dst.push(0xF0 | (c >> 18));
                dst.push(0x80 | ((c >> 12) & 0x3F));
                dst.push(0x80 | ((c >> 6) & 0x3F));
                dst.push(0x80 | (c & 0x3F));
            } else {
                throw "UTF-8 encoding error";
            }
        }
    }
    template<class T>
    void decodeUTF8(iustring<T>& dst, const char* src, int n) {
        dst.clear();
        unsigned int x = 0;
        int b = -1;
        for(int i=0; i<n; i++) {
            unsigned char c = src[i];
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
            // -- check if data type is big enough --
            if(sizeof(T) < unsigned(b+1)) {
                throw "UTF-8 decoding error";
            }
            // -- character complete --
            if(b==0) {
                dst.push_back(x);
            }
        }
    }
    template<class T>
    void encodeUTF16(bytearray& dst, iustring<T>& src) {
        for(int i=0; i<src.length(); i++) {
            T c = src[i];
            // -- 2 bytes --
            if(c <= 0xFFFF) {
                dst.push(c & 0xFF);
                dst.push(c >> 8);
            // -- 4 bytes --
            } else {
                c -= 0x10000;
                dst.push(0xDC00 | (c & 0x3FF));
                dst.push(0xD800 | ((c >> 10) & 0x3FF));
            }
        }
    }
    template<class T>
    void decodeUTF16(iustring<T>& dst, const char* src, int n) {
        dst.clear();
        unsigned int x = 0;
        unsigned int y = 0;
        int b = 0;
        for(int i=0; i<n; i++) {
            unsigned char c = src[i];
            if(b == 0) {
                x = c;
            } else if(b == 1) {
                x |= ((unsigned int)c) << 8;
                if(x < 0xD800 || x > 0xDFFF) {
                    dst.push_back(x);
                    b = -1;
                } else if(x < 0xD800 || x > 0xDBFF) {
                    throw "UTF-16 decoding error";
                }
            } else if(b == 2){
                y = c;
            } else if(b == 3){
                y |= ((unsigned int)c) << 8;
                if(y < 0xDC00 || y > 0xDFFF) {
                    throw "UTF-16 decoding error";
                }
                x = (((x & 0x3FF) << 10) | (y & 0x3FF)) + 0x10000;
                dst.push_back(x);
                b = -1;
            }
            b++;
        }
    }

    typedef iustring<char> iucstring;
    typedef iustring<wchar_t> iuwstring;
    typedef iustring<int> iuistring;
    typedef iustring<unsigned char> iubstring;
}

#endif /* iustring_h__ */
