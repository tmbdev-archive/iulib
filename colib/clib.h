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

#ifndef UNSAFE

inline int check(int condition) {
    if(!condition) abort();
    return 1;
}

inline int range(int i,int n) {
    if((unsigned)i>=(unsigned)n) abort();
    return i;
}

#else

#define check(x) 1
#define range(i,n) i

#endif

#define salloc() \
    (2+(int*)calloc(sizeof (int),2))

#define sfree(pointer) \
    free(-2+(int*)pointer)

#define slength(pointer) \
    (((int*)pointer)[-1])

#define stotal(pointer) \
    (((int*)pointer)[-2])

#define sappend(pointer) \
    do { \
        if(slength(pointer)==stotal(pointer)) { \
            int ntotal = 2*stotal(pointer)+1; \
            pointer = (void*)(2+(int*)realloc(-2+(int*)pointer,ntotal*(sizeof *pointer)+2*sizeof (int))); \
            stotal(pointer) = ntotal; \
        } \
        slength(pointer)++;  \
    } while(0)

#define stos(pointer) \
    pointer[slength(pointer)-1]

#define spush(pointer,value) \
    do { sappend(pointer); stos(pointer) = (value); } while(0)

#define spop(pointer) \
    (check(slength(pointer)>0),pointer[--slength(pointer)])

#define sref(pointer,i) \
    pointer[range(i,slength(pointer))]

#define array2d(type) type **

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

inline void free2d_(void *p) {
    int *data = -2+(int*)p;
    void **pointers = p;
    free(pointers[0]);
    free(data);
}

#define dim2d(p,i) ((int*)p)[-2+i]

#define aref2d(a,i,j) \
    a[range(i,dim(a,0))][range(j,dim(a,1))]

#define alloc2d(w,h,type) \
    (type**)alloc2d_(w,h,sizeof (type))

#define free2d(p) \
    free2d_(p)

#endif
