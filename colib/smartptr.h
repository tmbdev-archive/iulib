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
// Project: iulib -- image understanding library
// File: smartptr.h
// Purpose: smart pointers--automatically deallocate objects when they go out of scope
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file smartptr.h
/// \brief Smart pointers

#ifndef h_smartptr_
#define h_smartptr_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "colib/checks.h"

namespace colib {

    /// \brief General class for remembering cleanup actions (e.g., for pointers).
    ///
    /// Declare a cleanup function as
    ///
    /// JpegImage *image = jpeg_open(...);
    /// cleanup image_cleanup(jpeg_close,image);
    ///
    /// If you don't want the cleanup action to be executed, you can call the "forget" method.
    /// E.g.,
    ///
    /// image_cleanup.forget();
    /// jpeg_close(image);

    class cleanup {
    private:

        /// Internal helper class.

        class Cleaner {
        public:
            virtual void cleanup() = 0;
            virtual ~Cleaner() {}
        };
        template <class F,class T>
        class TheCleaner:public Cleaner {
        public:
            F f;
            T t;
            TheCleaner(F f,T t):f(f),t(t) {
            }
            void cleanup() {
                f(t);
            }
        };
        Cleaner *cleaner;

    public:

        /// Initialize the cleanup object with a cleanup function f and a target object t.

        template <class F,class T>
        cleanup(F f,T t) {
            cleaner = new TheCleaner<F,T>(f,t);
        }

        /// Tell the cleanup object to forget about cleaning up the target object.

        void forget() {
            if(cleaner) {
                delete cleaner;
                cleaner = 0;
            }
        }

        /// Destroy the cleanup object, cleaning up the target by calling the cleanup function if necessary.

        ~cleanup() {
            if(cleaner) {
                cleaner->cleanup();
                delete cleaner;
                cleaner = 0;
            }
        }
    };

    /// \brief Automatic deletion, linear assignment.
    ///
    /// A smart pointer class that deletes the pointer it holds when it
    /// goes out of scope.  Assignment is like it is for linear types: on
    /// assignment, the pointer gets moved to the destination, and the
    /// source gets set to NULL (this is convenient for returning values,
    /// for use in lists, and similar settings).

    template <class T>
    class autodel {
    private:
        T *pointer;
        autodel(autodel<T> &other);

    public:

        /// Default constructor sets pointer to null.

        autodel() {
            pointer = 0;
        }

        /// Destructor deletes any pointer held by the class.

        ~autodel() {
            if(pointer) delete pointer;
        }

        /// Initialization with a pointer transfers ownership to the class.

        explicit autodel(T *other) {
            pointer = other;
        }

        /// Assignment of a pointer deletes any old pointer held by the class and
        /// transfers ownership of the pointer to the class.

        void operator=(T *other) {
            if(pointer && pointer != other)
                delete pointer;
            pointer = other;
        }

        /// Smart pointer dereference; throws an exception if the pointer is null

        T *operator->() const {
            if(!pointer) throw "autodel: attempt to dereference null smartpointer";
            return pointer;
        }

        /// Explicit pointer dereference; throws an exception if the pointer is null

        T &operator*() const {
            if(!pointer) throw "autodel: attempt to dereference null smartpointer";
            return *pointer;
        }

        /// Same as operator*()

        T &ref() const {
            return operator*();
        }

        /// Conversion to pointer.

        T *ptr() const {
            return pointer;
        }

        /// Testing whether the pointer is null.

        bool operator!() const {
            return !pointer;
        }

        /// Testing whether the pointer is null.

        operator bool() const {
            return !!pointer;
        }

        /// Linear assignment: get the pointer from the other smart pointer,
        /// and set the other smart pointer to null.

        void operator=(autodel<T> &other) {
            T *new_pointer = other.move();
            if(pointer && pointer != new_pointer)
                delete pointer;
            pointer = new_pointer;
        }

        /// Take ownership away from this smart pointer and set the smart pointer to null.

        T *move() {
            T *result = pointer;
            pointer = 0;
            return result;
        }
    };

    /// \brief A simple smart pointer class for holding malloc-allocated pointers
    ///
    /// This is completely analogous to autodel, it just calls "free" to free the pointer.
    /// We could refactor this and have a common baseclass for autodel/autofree, but
    /// let's keep this simple.

    template <class T>
    class autofree {
    private:
        T *pointer;
	autofree(autofree<T> &);

    public:

        /// Default constructor sets pointer to null.

        autofree() {
            pointer = 0;
        }

        /// Destructor deletes any pointer held by the class.

        ~autofree() {
            if(pointer) free(pointer);
        }

        /// Initialization with a pointer transfers ownership to the class.

        explicit autofree(T *other) {
            pointer = other;
        }

        /// Assignment of a pointer deletes any old pointer held by the class and
        /// transfers ownership of the pointer to the class.

        void operator=(T *other) {
            if(pointer) free(pointer);
            pointer = other;
        }

        /// Smart pointer dereference; throws an exception if the pointer is null,
        /// unless compiled UNSAFE.

        T *operator->() const {
            if(!pointer) throw "autofree: attempt to dereference null smartpointer";
            return pointer;
        }

        /// Explicit pointer dereference; throws an exception if the pointer is null,
        /// unless compiled UNSAFE.

        T &operator*() const {
            if(!pointer) throw "autofree: attempt to dereference null smartpointer";
            return *pointer;
        }

        /// Same as operator*().

        T &ref() const {
            return operator*();
        }

        /// Conversion to pointer.

        T *ptr() const {
            return pointer;
        }

        /// Testing whether the pointer is null.

        bool operator!() const {
            return !pointer;
        }

        /// Testing whether the pointer is null.

        operator bool() const {
            return !!pointer;
        }

        /// Linear assignment: get the pointer from the other smart pointer,
        /// and set the other smart pointer to null.

        void operator=(autofree<T> &other) {
            if(pointer) delete pointer;
            pointer = other.move();
        }

        /// Take ownership away from this smart pointer and set the smart pointer to null.

        T *move() {
            T *result = pointer;
            pointer = 0;
            return result;
        }
    };

    /// \brief Automatic allocation and deletion, linear assignment.
    ///
    /// A smart pointer class that automatically allocates an object when
    /// an attempt is made to access and dereference the pointer.
    /// Assignment is linear (ownership gets transferred from the source of
    /// the assignment to the destination, and the source gets set to
    /// null).

    template <class T>
    class autoref {
    private:
        T *pointer;
	autoref(autoref<T> &);

    public:

        /// Default initializer, sets object to null.

        autoref() {
            pointer = 0;
        }

        /// Destructor deallocates object, if any.

        ~autoref() {
            if(pointer)
                delete pointer;
        }

        /// Smart pointer dereference allocates object if none is held, then returns a pointer.
        /// This always succeeds, unless the default constructor for T throws an exception.

        T *operator->() {
            if(!pointer)
                pointer = new T();
            return pointer;
        }

        /// Pointer dereference allocates object if none is held, then returns a pointer.
        /// This always succeeds, unless the default constructor for T throws an exception.

        T &operator*() {
            if(!pointer)
                pointer = new T();
            return *pointer;
        }

        /// Same as operator*

        T &ref() {
            return operator*();
        }

        /// Conversion to pointer.  Does not allocate an object.

        T *ptr() {
            return pointer;
        }

        /// Pointer dereference allocates object if none is held, then returns a pointer.
        /// This always succeeds, unless the default constructor for T throws an exception.

        T &deref() {
            if(!pointer)
                pointer = new T();
            return *pointer;
        }

        /// Set to new pointer value, deleting any old object, and transfering ownership to the class.

        void operator=(T *other) {
            if(pointer && pointer != other)
                delete pointer;
            pointer = other;
        }

        /// Linear assignment: get the pointer from the other smart pointer,
        /// and set the other smart pointer to null.

        void operator=(autoref<T> &other) {
            T *new_pointer = other.move();
            if(pointer && pointer != new_pointer)
                delete pointer;
            pointer = new_pointer;
        }

        /// Take ownership away from this smart pointer and set the smart pointer to null.

        T *move() {
            T *result = pointer;
            pointer = 0;
            return result;
        }

        /// Deallocate the object.

        void dealloc() {
            if(pointer) {
                delete pointer;
                pointer = 0;
            }
        }
    };

    /// \brief A simple class for holding stdio streams.

    class stdio {
    private:
        FILE *stream;

    public:

        /// Default constructor sets stream to null.

        stdio() {
            stream = 0;
        }

        /// Constructor that calls fopen.  This constructor also recognizes
        /// "-" as a special file name to refer to stdin/stdout (depending on the mode).

        stdio(const char *file,const char *mode) {
            if(!file) throw "no file name given (file name is NULL)";
            if(!mode) throw "no mode given (mode is NULL)";
            if(!file[0]) throw "empty file name given";
            if(!mode[0]) throw "empty mode given";
            if(!strcmp(file,"-")) {
                if(mode[0]=='r') stream = stdin;
                else stream = stdout;
            } else {
                stream = fopen(file,mode);
                if(!stream) {
                    if(mode[0]=='w' || mode[0]=='a')
                        throwf("%s: cannot open file for writing",file);
                    else
                        throwf("%s: cannot open file for reading",file);
                }
            }
        }

        /// Destructor deletes any stream held by the class.

        ~stdio() {
            close();
        }

        /// Initialization with a stream transfers ownership to the class.

        explicit stdio(FILE *other) {
            if(!other) throw "stdio: attempt to set stream to null";
            stream = other;
        }

        /// Assignment of a stream deletes any old stream held by the class and
        /// transfers ownership of the stream to the class.

        void operator=(FILE *other) {
            if(!other) throw "stdio: attempt to set stream to null";
            close();
            stream = other;
        }

        /// Implicit conversion to stream, convenient for passing as an argument.

        operator FILE*() const {
            return stream;
        }

        /// Testing whether the stream is null.

        bool operator!() const {
            return !stream;
        }

        /// Testing whether the stream is not null.

        operator bool() const {
            return !!stream;
        }

        /// Linear assignment: get the stream from the other smart stream,
        /// and set the other smart stream to null.

        void operator=(stdio &other) {
            close();
            stream = other.move();
        }

        /// Take ownership away from this holder and return the stream.

        FILE *move() {
            FILE *result = stream;
            stream = 0;
            return result;
        }

        /// Close the stream if it's open.

        void close() {
            if(stream) {
                if(stream != stdout && stream != stdin)
                    fclose(stream);
                stream = 0;
            }
        }
    };

    template <class T>
    class counted {
        struct TC : T {
            int refcount_;
        };
        TC *p;
    public:
        counted() {
            p = 0;
        }
        counted(const counted<T> &other) {
            other.incref();
            p = other.p;
        }
        counted(counted<T> &other) {
            other.incref();
            p = other.p;
        }
        ~counted() {
            decref();
            p = 0;
        }
        void operator=(counted<T> &other) {
            other.incref();
            decref();
            p = other.p;
        }
        void operator=(const counted<T> &other) {
            other.incref();
            decref();
            p = other.p;
        }
        void operator*=(counted<T> &other) {
            other.incref();
            decref();
            p = other.p;
            other.drop();
        }
        void operator*=(const counted<T> &other) {
            other.incref();
            decref();
            p = other.p;
            other.drop();
        }
        bool allocated() {
            return !p;
        }
        void allocate() {
            decref();
            p = new TC();
            p->refcount_ = 1;
        }
        operator bool() {
            return !!p;
        }
        void drop() {
            decref();
            p = 0;
        }
        T &operator *() {
            if(!p) allocate();
            return *(T*)p;
        }
        T *operator->() {
            if(!p) allocate();
            return (T*)p;
        }
        operator T&() {
            if(!p) allocate();
            return *(T*)p;
        }
        operator T*() {
            if(!p) allocate();
            return (T*)p;
        }
        void incref() const {
            check();
            if(p) {
                if(p->refcount_>10000000) abort();
                if(p->refcount_<0) abort();
                p->refcount_++;
            }
        }
        void decref() const {
            check();
            if(p) {
                if(--p->refcount_==0) delete p;
                ((counted<T>*)this)->p = 0;
            }
        }
        void check() const {
            if(!p) return;
            if(p->refcount_>10000000) abort();
            if(p->refcount_<0) abort();
        }
    };

    // Define na_transfer to allow use in narray.

    template <class T>
    inline void na_transfer(autodel<T> &dst,autodel<T> &src) {
        dst = src.move();
    }
    template <class T>
    inline void na_transfer(autoref<T> &dst,autoref<T> &src) {
        dst = src.move();
    }
    template <class T>
    inline void na_transfer(autofree<T> &dst,autofree<T> &src) {
        dst = src.move();
    }
}

#endif
