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
// File: hash.h
// Purpose: simple hash table imlpementation
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de


/// \file hash.h
/// \brief Simple hash table implementations.

#ifndef h_hash_
#define h_hash_

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "colib/narray.h"
#include "smartptr.h"

namespace colib {

    namespace {

        inline int hash_next(int size) {
            int i = 1;
            while(i<=size) i<<=1;
            return i+1;
        }

        inline int hash_value(int key,int n) {
            ASSERT(n>0);
            return int(unsigned(98.209328340918084289029384908 * key
                                + 0.2349082034802840982092834098)%n);
        }

        inline int hash_value(int key1,int key2,int n) {
            ASSERT(n>0);
            return int(unsigned(1793.02934802983402808942308402389908 * key1 +
                                34.34983902935890238058909480840238 * key2
                                + 0.8949173412390482890423908048) % n);
        }

        inline int hash_value(const char *p,int n) {
            double total = 0.0;
            while(*p) {
                total = total * 1.29008352390840238 + (*p++) * 0.209384902384;
            }
            return int(unsigned(total)%n);
        }

    }
    


    /// \brief A simple int-to-something hash class.
    ///
    /// This is not particularly high performance, but it should be good enough for most uses.

    template <class T>
    class inthash {
    private:
        enum { hash_empty = ~0 };

        struct kvp {
            int key;
            T value;
            kvp() { key = hash_empty; }
        };
        narray<kvp> data;
        int fill;
        void maybe_grow() {
            if(fill>data.length()/2)
                grow(data.length()+1);
        }

    public:

        /// Allocate a hash table with a given initial size.

        inthash(int initial=3) {
            data.resize(max(3,initial));
            fill = 0;
        }

        /// Make the hash table empty again and deallocate all the data it holds.
        
        void dealloc() {
            data.dealloc();
            data.resize(3);
            fill = 0;
        }

        /// Grow the hash table by the given amount.

        void grow_by(int n) {
            ASSERT(n>=0);
            inthash nhash(data.length()+n);
            for(int i=0;i<data.length();i++) {
                kvp &entry = data[i];
                if(entry.key==hash_empty) continue;
                nhash(entry.key) = entry.value;
            }
            move(data,nhash.data);
            fill = nhash.fill;
        }

        /// Return a reference to the location associated with the given key.

        T &operator()(int key) {
            if(key==hash_empty) throw "key value reserved for hash table implementation";
            int n = data.length();
            int base = hash_value(key,n);
            for(int i=0;i<n;i++) {
                int index = (base+i)%n;
                kvp &entry = data[index];
                if(entry.key==key)
                    return entry.value;
                if(entry.key==hash_empty) {
                    if(fill<data.length()/2) {
                        fill++;
                        entry.key = key;
                        return entry.value;
                    }
                    grow_by(hash_next(data.length())-data.length());
                    return operator()(key);
                }
            }
            throw "internal error: no empty hash bucket found";
        }

        /// Return a pointer to the location associated with the given key, or
        /// null if there is no such location.

        T *find(int key) {
            if(key==hash_empty) throw "key value reserved for hash table implementation";
            int n = data.length();
            int base = hash_value(key,n);
            for(int i=0;i<n;i++) {
                int index = (base+i)%n;
                kvp &entry = data[index];
                if(entry.key==key)
                    return &entry.value;
                if(entry.key==hash_empty) {
                    return 0;
                }
            }
        }

        T &at(int key) {
            return this->operator()(key);
        }

        /// Return a list of keys.

        void keys(narray<int> &result) {
            result.clear();
            for(int i=0;i<data.length();i++) {
                kvp &entry = data[i];
                if(entry.key!=hash_empty) {
                    result.push(entry.key);
                }
            }
        }
    };

    /// \brief A simple pair-of-int-to-something hash class.
    ///
    /// This is not particularly high performance, but it should be good enough for most uses.

    template <class T>
    class int2hash {
    private:
        enum { hash_empty = ~0 };
        struct kvp {
            int key1,key2;
            T value;
            kvp() { key1 = hash_empty; }
        };
        narray<kvp> data;
        int fill;
        void maybe_grow() {
            if(fill>data.length()/2)
                grow(data.length()+1);
        }

    public:

        /// Allocate a hash table with a given initial size.

        int2hash(int initial=3) {
            data.dealloc();
            data.resize(max(3,initial));
            fill = 0;
        }

        /// Make the hash table empty again and deallocate all the data it holds.
        
        void dealloc() {
            data.dealloc();
            data.resize(3);
            fill = 0;
        }

        /// Grow the hash table by the given amount.

        void grow_by(int n) {
            ASSERT(n>=0);
            int2hash<T> nhash(data.length()+n);
            for(int i=0;i<data.length();i++) {
                kvp &entry = data[i];
                if(entry.key1==hash_empty) continue;
                nhash(entry.key1,entry.key2) = entry.value;
            }
            move(data,nhash.data);
            fill = nhash.fill;
        }

        /// Return a reference to the location associated with the given key.

        T &operator()(int key1,int key2) {
            if(key1==hash_empty) throw "key value reserved for hash table implementation";
            int n = data.length();
            int base = hash_value(key1,key2,n);
            for(int i=0;i<n;i++) {
                int index = (base+i)%n;
                kvp &entry = data[index];
                if(entry.key1==key1 && entry.key2==key2)
                    return entry.value;
                if(entry.key1==hash_empty) {
                    if(fill<data.length()/2) {
                        fill++;
                        entry.key1 = key1;
                        entry.key2 = key2;
                        return entry.value;
                    }
                    grow_by(hash_next(data.length())-data.length());
                    return operator()(key1,key2);
                }
            }
            throw "internal error: no empty hash bucket found";
        }

        /// Return a pointer to the location associated with the keys, or null
        /// if there is no entry.

        T *find(int key1,int key2) {
            if(key1==hash_empty) throw "key value reserved for hash table implementation";
            int n = data.length();
            int base = hash_value(key1,key2,n);
            for(int i=0;i<n;i++) {
                int index = (base+i)%n;
                kvp &entry = data[index];
                if(entry.key1==key1 && entry.key2==key2)
                    return &entry.value;
                if(entry.key1==hash_empty) {
                    return 0;
                }
            }
            throw "internal error: no empty hash bucket found";
        }
    };

    /// A string-to-something hash class.

    /// \brief A simple string-to-something hash class.
    ///
    /// This is not particularly high performance, but it should be good enough for most uses.

    template <class T>
    class strhash {
    private:
        struct kvp {
            autofree<char> key;
            T value;
        };
        narray<kvp> data;
        int fill;
        void maybe_grow() {
            if(fill>data.length()/2)
                grow(data.length()+1);
        }

    public:

        /// Allocate a hash table with a given initial size.

        strhash(int initial=3) {
            data.dealloc();
            data.resize(max(3,initial));
            fill = 0;
        }

        /// Make the hash table empty again and deallocate all the data it holds.
        
        void dealloc() {
            data.dealloc();
            data.resize(3);
            fill = 0;
        }

        /// Grow the hash table by the given amount.

        void grow_by(int n) {
            ASSERT(n>=0);
            strhash nhash(data.length()+n);
            for(int i=0;i<data.length();i++) {
                kvp &entry = data[i];
                if(entry.key.ptr()==0) continue;
                nhash(entry.key.ptr()) = entry.value;
            }
            move(data,nhash.data);
            fill = nhash.fill;
        }

        /// Return a pointer to the location of the value, or null if
        /// the key doesn't exist.

        T *find(const char *key) {
            if(key==0) throw "key value reserved for hash table implementation";
            int n = data.length();
            int base = hash_value(key,n);
            for(int i=0;i<n;i++) {
                int index = (base+i)%n;
                kvp &entry = data[index];
                if(entry.key.ptr() && !strcmp(entry.key.ptr(),key))
                    return &entry.value;
                if(!entry.key) {
                    return 0;
                }
            }
            throw "internal error: no empty hash bucket found";
        }
        /// Return a reference to the location associated with the given key.

        T &operator()(const char *key) {
            if(key==0) throw "key value reserved for hash table implementation";
            int n = data.length();
            int base = hash_value(key,n);
            for(int i=0;i<n;i++) {
                int index = (base+i)%n;
                kvp &entry = data[index];
                if(entry.key.ptr() && !strcmp(entry.key.ptr(),key))
                    return entry.value;
                if(!entry.key) {
                    if(fill<data.length()/2) {
                        fill++;
                        entry.key = strdup(key);
                        return entry.value;
                    }
                    grow_by(hash_next(data.length())-data.length());
                    return operator()(key);
                }
            }
            throw "internal error: no empty hash bucket found";
        }

        /// Return a list of keys.
        /// FIXME change this to use the new iustring

        void keys(narray<const char*> &result) {
            result.clear();
            for(int i=0;i<data.length();i++) {
                kvp &entry = data[i];
                if(entry.key.ptr()!=0) {
                    result.push() = entry.key.ptr();
                }
            }
        }
    };
}

#endif
