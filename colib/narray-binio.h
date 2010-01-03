// -*- C++ -*-
// Copyright 2006 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
// or its licensors, as applicable.
// Copyright 1995-2005 by Thomas M. Breuel
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
// File:
// Purpose:
// Responsible:
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de


#ifndef narray_io_h_
#define narray_io_h_

#include <unistd.h>
#include <sys/stat.h>
//#include "ocropus.h"

// GNU C++ incorrectly warns about type punning and aliasing below;
// all we can do is disable all such warnings
#pragma GCC diagnostic ignored "-Wstrict-aliasing"

namespace narray_io {
    // using namespace ocropus;
    using namespace colib;

    template <class T>
    inline unsigned magic_number() {
        return 0x0abe0000 + sizeof (T);
    }

    inline void string_read(FILE *stream,narray<char> &s) {
        s.resize(100000);
        CHECK(fgets(&s[0],s.length(),stream)!=0);
        int n = strlen(&s[0]);
        if(n>0 && s[n-1]=='\n') s[n-1] = 0;
        s.truncate(strlen(&s[0]));
    }

    inline void string_read(FILE *stream,strg &s_) {
        char s[100000];         // FIXME
        CHECK(fgets(&s[0],100000,stream)!=0);
        int n = strlen(&s[0]);
        if(n>0 && s[n-1]=='\n') s[n-1] = 0;
        s_ = s;
    }

    inline void string_write(FILE *stream,const char *str) {
        CHECK(fputs(str,stream)>=0);
        CHECK(fputc('\n',stream)!=EOF);
    }

    // string object type identifiers
    inline void magic_write(FILE *stream,const char *str) {
        int n = strlen(str);
        CHECK(fwrite(str,1,n,stream)==unsigned(n));
    }
    inline void magic_read(FILE *stream,const char *str) {
        int n = strlen(str);
        char buf[100];
        CHECK(n<99);
        CHECK(fread(buf,1,n,stream)==unsigned(n));
        buf[n] = 0;
        if(strncmp(buf,str,n))
            throwf("magic_read: wanted '%s', got '%s'",str,buf);
    }
    inline void magic_get(FILE *stream,strg &result,int n) {
        CHECK(n<100);
        char buf[100];
        CHECK(fread(buf,1,n,stream)==unsigned(n));
        buf[n] = 0;
        result = buf;
    }

    // integer magic numbers
    inline void magic_read(FILE *stream,unsigned value) {
        unsigned temp;
        CHECK(fread(&temp,sizeof temp,1,stream)==1);
        CHECK(temp==value);
    }
    inline void magic_write(FILE *stream,unsigned value) {
        CHECK(fwrite(&value,sizeof value,1,stream)==1);
    }

    template <class T>
    inline void scalar_read(FILE *stream,T &value) {
        CHECK(fread(&value,sizeof value,1,stream)==1);
    }

    template <class T>
    inline void scalar_write(FILE *stream,T value) {
        CHECK(fwrite(&value,sizeof value,1,stream)==1);
    }

    // array of scalar reading

    template <class T>
    inline void narray_read(FILE *stream,narray<T> &data) {
        if(0) data[0]+0;
        unsigned magic;
        CHECK(sizeof(magic)==4);
        CHECK(fread(&magic,sizeof magic,1,stream)==1);
        CHECK(magic==magic_number<T>());
        int dims[4];
        CHECK(fread(dims,sizeof dims[0],4,stream)==4);
        data.resize(dims[0],dims[1],dims[2],dims[3]);
        if(data.length1d()>0) {
            CHECK((int)fread(&data.at1d(0),sizeof data.at1d(0),
                             data.length1d(),stream)==data.length1d());
        }
    }

    // array of scalar writing

    template <class T>
    inline void narray_write(FILE *stream,narray<T> &data) {
        if(0) data[0]+0;
        unsigned magic = magic_number<T>();
        CHECK(sizeof(magic)==4);
        CHECK(fwrite(&magic,sizeof magic,1,stream)==1);
        CHECK(fwrite(data.dims,sizeof data.dims[0],4,stream)==4);
        if(data.length1d()>0) {
            CHECK((int)fwrite(&data.at1d(0),sizeof data.at1d(0),
                              data.length1d(),stream)==data.length1d());
        }
    }

    // ragged array reading

    template <class T>
    inline void narray_read(FILE *stream,narray< narray<T> > &data) {
        unsigned magic;
        CHECK(sizeof(magic)==4);
        CHECK(fread(&magic,sizeof magic,1,stream)==1);
        CHECK(magic==293843);
        int dims[4];
        CHECK(fread(dims,sizeof dims[0],4,stream)==4);
        data.resize(dims[0],dims[1],dims[2],dims[3]);
        for(int i=0;i<data.length();i++)
            narray_read(stream,data[i]);
    }

    // ragged array writing

    template <class T>
    inline void narray_write(FILE *stream,narray< narray<T> > &data) {
        unsigned magic = 293843;
        CHECK(sizeof(magic)==4);
        CHECK(fwrite(&magic,sizeof magic,1,stream)==1);
        CHECK(fwrite(data.dims,sizeof data.dims[0],4,stream)==4);
        for(int i=0;i<data.length();i++)
            narray_write(stream,data[i]);
    }

    inline unsigned read32(FILE *stream) {
        unsigned result = 0;
        result = (fgetc(stream)&0xff);
        result = (result<<8) | (fgetc(stream)&0xff);
        result = (result<<8) | (fgetc(stream)&0xff);
        result = (result<<8) | (fgetc(stream)&0xff);
        return result;
    }

    inline double mconvert(FILE *stream,int code) {
        volatile char data[8];
        switch(code) {
        case 0x08:
            return (unsigned char)fgetc(stream);
        case 0x09:
            return (signed char)fgetc(stream);
        case 0x0b:
            data[1] = fgetc(stream);
            data[0] = fgetc(stream);
            return *(short*)data;
        case 0x0c:
            for(int i=3;i>=0;i--)
                data[i] = fgetc(stream);
            return *(int*)data;
        case 0x0d:
            for(int i=3;i>=0;i--)
                data[i] = fgetc(stream);
            return *(float*)data;
        case 0x0e:
            for(int i=7;i>=0;i--)
                data[i] = fgetc(stream);
            return *(double*)data;
        }
        throw "oops";
    }

    template <class T>
    inline void read_mnist(narray<T> &data,FILE *stream) {
        ASSERT(sizeof (short)==sizeof (char[2]));
        ASSERT(sizeof (int)==sizeof (char[4]));
        ASSERT(sizeof (float)==sizeof (char[4]));
        ASSERT(sizeof (double)==sizeof (char[8]));
        int magic0 = fgetc(stream);
        CHECK(magic0==0);
        int magic1 = fgetc(stream);
        CHECK(magic1==0);
        int code = fgetc(stream);
        CHECK(code>=0x08 && code<=0x0e);
        int rank = fgetc(stream);
        CHECK(rank>=1 && rank<=4);
        int dims[4];
        for(int i=0;i<4;i++) dims[i] = 0;
        for(int i=0;i<rank;i++ )dims[i] = read32(stream);
        data.resize(dims[0],dims[1],dims[2],dims[3]);
        for(int i=0;i<data.length1d();i++)
            data.at1d(i) = (T)mconvert(stream,code);
    }

    struct MnistStream {
        stdio stream;
        int magic0,magic1;
        int code;
        int rank;
        int dims[4];
        int record;
        void open(const char *file) {
            stream = fopen(file,"r");
            magic0 = fgetc(stream);
            CHECK(magic0==0);
            magic1 = fgetc(stream);
            CHECK(magic1==0);
            code = fgetc(stream);
            CHECK(code>=0x08 && code<=0x0e);
            rank = fgetc(stream);
            CHECK(rank>=1 && rank<=4);
            for(int i=0;i<4;i++) dims[i] = 0;
            for(int i=0;i<rank;i++ )dims[i] = read32(stream);
            record = 0;
        }
        void close() {
            stream.close();
        }
        int length() {
            return dims[0];
        }
        int dim(int i) {
            return dims[i];
        }
        template<class T>
        bool read(narray<T> &data) {
            if(record>=dims[0]) return false;
            if(dims[1]==0) {
                data.resize(1);
            } else {
                data.resize(dims[1],dims[2],dims[3]);
            }
            for(int i=0;i<data.length1d();i++)
                data.at1d(i) = (T)mconvert(stream,code);
            record++;
            return true;
        }
    };
}

#endif
