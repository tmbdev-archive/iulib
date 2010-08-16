// -*- C -*- (C99)

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
// File: clib.h
// Purpose: simple data structures for plain ANSI C99 programs
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

/// \file clib.h
/// \brief Simple data structures for plain ANSI C99 programs

#ifndef h_clib__
#define h_clib__

#include <stdio.h>
#include <stdlib.h>

/* Simple macros for condition and/or range checking. */

#ifndef UNSAFE

/* Abort if the given condition is false. */

inline int check(int condition) {
    if(!condition) abort();
    return 1;
}

/* Abort if i is outside the range 0...n-1 */

inline int range(int i,int n) {
    if((unsigned)i>=(unsigned)n) abort();
    return i;
}

#else

#define check(x) 1
#define range(i,n) i

#endif

/******************************************************************

   A 1D variable size vector.  Here is a simple example: 

    float *a = salloc();
    for(int i=0;i<100;ii++)
        spush(a,i*i);
    sfree(a);

******************************************************************/

/* Allocate a variable size vector. */

#define salloc() \
    (2+(int*)calloc(sizeof (int),2))

/* Free a variable size vector. */

#define sfree(pointer) \
    free(-2+(int*)pointer)

/* Return the current length of the vector. */

#define slength(pointer) \
    (((int*)pointer)[-1])

/* Return the total available space in the vector. */

#define stotal(pointer) \
    (((int*)pointer)[-2])

/* Append an element to the vector. */

#define sappend(pointer) \
    do { \
        if(slength(pointer)==stotal(pointer)) { \
            int ntotal = 2*stotal(pointer)+1; \
            pointer = (void*)(2+(int*)realloc(-2+(int*)pointer,ntotal*(sizeof *pointer)+2*sizeof (int))); \
            stotal(pointer) = ntotal; \
        } \
        slength(pointer)++;  \
    } while(0)

/* Return an lvalue corresponding to the last element. */

#define stos(pointer) \
    pointer[slength(pointer)-1]

/* Push an element onto the end of the vector. */

#define spush(pointer,value) \
    do { sappend(pointer); stos(pointer) = (value); } while(0)

/* Remove the last element from the vector and return it. */

#define spop(pointer) \
    (check(slength(pointer)>0),pointer[--slength(pointer)])

/* Return the element at position i in the variable length vector; performs range checking. */

#define sref(pointer,i) \
    pointer[range(i,slength(pointer))]

/*****************************************************************

   A 2D array that can be used as an array of pointers. The array
   size is stored in the pointer. 

   Example:

   array2d(float) a = alloc2d(117,232,float);
   for(int i=0;i<dim(a,0);i++) {
       for(int j=0;j<dim(a,1);j++) {
          // access elements with range checks
          aref2d(a,i,j) = 17;
          // access elements directly
          a[i][j] = 234;
       }
   }

******************************************************************/

#define array2d(type) type **

/* Allocate a 2D array. */

inline void *alloc2d_(int d0,int d1,int size) {
    int i;
    void **pointers;
    int *data = malloc(2*sizeof (int)+d0 * sizeof *pointers);
    char *base = malloc(d0*d1*size);
    pointers = (void**)(data+2);
    data[0] = d0;
    data[1] = d1;
    for(i=0;i<d0;i++) pointers[i] = base+i*d1*size;
    return pointers;
}

/* Free a 2D array. */

inline void free2d_(void *p) {
    int *data = -2+(int*)p;
    void **pointers = p;
    free(pointers[0]);
    free(data);
}

/* Get dim 0 and dim 1 for the 2D array. */

#define dim2d(p,i) ((int*)p)[-2+i]

/* Array subscripting with range checking. */

#define aref2d(a,i,j) \
    a[range(i,dim(a,0))][range(j,dim(a,1))]

/* Allocate a 2D array of a given type. */

#define alloc2d(w,h,type) \
    (type**)alloc2d_(w,h,sizeof (type))

/* Free a 2D array. */

#define free2d(p) \
    free2d_(p)

#endif
