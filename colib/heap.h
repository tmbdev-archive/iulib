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

#ifndef H_HEAP__
#define H_HEAP__

#include "colib/colib.h"

#undef ASSERT
#define ASSERT(X) do{if(!(X)) throw __FILE__ ": assertion failed " #X;}while(0)
#define RANGE(I,N) do{if(unsigned(I)>=unsigned(N)) throw __FILE__ ": range check failed"; }while(0)


/////////////////////////////////////////////////////////////////////
///
/// \struct heap
/// Purpose: Implementation of the heapsort algorithm
///
//////////////////////////////////////////////////////////////////////

template <class T,bool use_bitswap=true>
struct heap {
    struct Item {
        double priority;
        T object;
        bool operator>(Item &other) { return priority>other.priority; }
    };
  static void bitswap(Item &a,Item &b) {
        char buf[sizeof (Item)];
        memcpy(buf,&a,sizeof (Item));
        memcpy(&a,&b,sizeof (Item));
        memcpy(&b,buf,sizeof (Item));
    }
    colib::narray<Item> data;
    int left(int i) { return 2*i; }
    int right(int i) { return 2*i+1; }
    int parent(int i) { return int(i/2); }
    Item &A(int i) { return data.at(i-1); }
    void clear() {
        data.clear();
    }
  /// Rearrange the data in the heap after extractMax operation
    void heapify(int i) {
        int heapsize = data.length();
        int l = left(i);
        int r = right(i);
        int largest = -1;
        if(l<=heapsize && A(l).priority>A(i).priority)
            largest = l;
        else
            largest = i;
        if(r<=heapsize && A(r).priority > A(largest).priority)
            largest = r;
        if(largest!=i) {
            if(use_bitswap) bitswap(A(i),A(largest));
            //else swap(A(i),A(largest));
            heapify(largest);
        }
    }
  ///Return priority of the root node in the heap
    double topPriority() {
        ASSERT(data.length()>=1);
        return data.at(0).priority;
    }
  ///Return (but not remove) the element at the root node
    T &top() {
        ASSERT(data.length()>=1);
        return data.at(0).object;
    }
  ///Return and remove the element at the top (root node).
    T &extractMax() {
        ASSERT(data.length()>=1);
        if(use_bitswap) bitswap(A(1),A(data.length()));
        //else swap(A(1),A(data.length()));
        T &result = data.pop().object;
        heapify(1);
        return result;
    }
  ///Insert an element in the heap
    void insert(T &object,double priority) {
        Item &item = data.push();
        item.object = object;
        item.priority = priority;
        int i = data.length();
        while(i>1 && priority>A(parent(i)).priority) {
            A(i) = A(parent(i));
            i = parent(i);
        }
        A(i).object = object;
        A(i).priority = priority;
    }
  ///Return number of elements in the heap
    int length() {
        return data.length();
    }
};

#endif
