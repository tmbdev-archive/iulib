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
// File: checks.h
// Purpose: contains macros for assertions, checks, and unit testing
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file checks.h
/// \brief Contains macros for assertions, checks, and unit testing

#ifndef colib_checks_
#define colib_checks_

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

namespace {
    void throwf(const char *format, ...) {
        va_list v;
        va_start(v, format);
        static char buf[1000];
        vsnprintf(buf, sizeof(buf), format, v);
        va_end(v);
        throw (const char *) buf;
    }
}

namespace colib {
    // Use this for methods that haven't been implemented yet.  This
    // is for methods that are optional, and callers can catch this
    // exception to perform some alternative action.
    struct Unimplemented {};

    // Use this for methods that haven't been tested yet and therefore
    // can't be relied on.
    struct Untested {};

    // By convention, we throw "const char *" for exceptions
    // that leave the program in a defined state but that don't usually
    // have any meaningful automatic.  If you must, you can document
    // this by using something like 'throw Exception("foo")', but
    // it's recommended that you simply 'throw "foo"'.
    inline const char *Exception(const char *s) {
        return s;
    }
};

#ifndef UNSAFE

/// Check the assertion and die if it fails.
///
/// USE: during testing, to verify that an algorithm works as expected
//  use EXPENSIVE_ASSERT for checks that are unusually expensive (e.g., looping over a data structure)
/// DO NOT USE: to validate input data

#define ASSERT(X) while(!(X)) {fprintf(stderr,"%s:%d FAILED ASSERT %s\n",__FILE__,__LINE__,#X); abort();}
#define EXPENSIVE_ASSERT(X) while(!(X)) {fprintf(stderr,"%s:%d FAILED ASSERT %s\n",__FILE__,__LINE__,#X); abort();}

#else

#define ASSERT(X) /*nothing*/
#define EXPENSIVE_ASSERT(X) /*nothing*/

#endif

#define STR__(X) #X
#define STR___(X) STR__(X)

/// Check the assertion and warn once if it fails.
///
/// USE: for warnings about features that may later become errors

#ifndef UNSAFE

#define ASSERTWARN(X) while(!(X)) {\
    static int count=0; \
    if(!count++) fprintf(stderr,"%s:%d FAILED ASSERT (WARNING) %s\n",__FILE__,__LINE__,#X); break;}

#else

#define ASSERTWARN(X) /*nothing*/

#endif

/// Check the assertion and throw an exception (const char*) if it fails.
///
/// USE: for validating input data, checking internal conditions that might
/// happen during normal operation; when these checks fail, the program should
/// remain in a defined state (no leaks, no pointer errors, no undefined effects)
///
/// DO NOT USE: for conditions that leave the program in an undefined state or
/// for error conditions that an end user might see

#define CHECK_ARG2(X,S) do{while(!(X)) throw "CHECK " __FILE__ ":" STR___(__LINE__) " " S;}while(0)
#define CHECK_CONDITION2(X,S) do{while(!(X)) throw "CHECK " __FILE__ ":" STR___(__LINE__) " " S;}while(0)

/// These are simpler, non-preferred forms because they don't give the user
/// a good indication of what's wrong.

#define CHECK_ARG(X) do{while(!(X)) throw "CHECK " __FILE__ ":" STR___(__LINE__) " " #X;}while(0)
//#define CHECK(X) do{while(!(X)) throw "CHECK " __FILE__ ":" STR___(__LINE__) " " #X;}while(0)
#ifndef CHECK
// FIXME remove usages of CHECK from the rest of the code
#define CHECK(X) CHECK_ARG(X)
#endif
#define CHECK_CONDITION(X) do{while(!(X)) throw "CHECK " __FILE__ ":" STR___(__LINE__) " " #X;}while(0)

/// Check the assertion and die if it fails.  Use this if the program
/// is in an undefined state after the condition fails, or if recovery
/// just isn't possible.  This error is NOT disabled by defining UNSAFE.
///
/// USE: for rare, unrecoverable conditions; consistency checks that should be performed in production code
///
/// DO NOT USE: for conditions that are the result of bad user input

#define ALWAYS_ASSERT(X) while(!(X)) {fprintf(stderr,"%s:%d FAILED REQUIRE %s\n",__FILE__,__LINE__,#X); abort();}

#ifndef DEBUG_TEST
#define DEBUG_TEST
#endif

/// Assert a test case result.
///
/// USE: in test cases, when the test case fails but further tests can be performed safely
///
/// DO NOT USE: in production code, library code, etc.

#define TEST_ASSERT(X) \
    do {if(!(X)) { fprintf(stderr,"%s\t[%s:%d] %s\n","FAILED",__FILE__,__LINE__,#X); DEBUG_TEST; }} while(0)

/// Assert a test case result.
///
/// USE: in test cases, when the test case fails but further tests can be performed safely
///
/// DO NOT USE: in production code, library code, etc.

#define TEST_EQ(U,V) \
    do { double u_ = (U), v_ = (V); \
        if(u_!=v_) { fprintf(stderr,"%s\t[%s:%d] %s==%s %g!=%g\n","FAILED",__FILE__,__LINE__,#U,#V,u_,v_); \
                   DEBUG_TEST; }} while(0)

/// Assert an unrecoverable test case result.
///
/// USE: in test cases, when the test case fails and the test environment is left in an undefined state
///
/// DO NOT USE: in production code, library code, etc.

#define TEST_OR_DIE(X) \
    if(!(X)) {fprintf(stderr,"%s\t[%s:%d] %s\n","FAILED",__FILE__,__LINE__,#X); abort(); }

/// Assert that something should fail.
///
/// USE: to test that error and condition checks are actually working
///
/// DO NOT USE: in production code, library code, etc.

#define TEST_FAILURE(X) \
    do{try{ X; fprintf(stderr,"%s\t[%s:%d] %s\n","FAILED TO FAIL",__FILE__,__LINE__,#X); abort(); } catch(...) {}}while(0)

/// Conditional debug display.

#ifndef IFDD
#define IFDD if(getenv("DEBUG_DISPLAY") && strstr(getenv("DEBUG_DISPLAY"),__FILE__))
#endif
#ifndef IFDP
#define IFDP if(getenv("DEBUG_PRINT") && strstr(getenv("DEBUG_PRINT"),__FILE__))
#endif

/// Deprecate functions (supported in gcc, not other compilers)

#if !defined(DISABLE_DEPRECATION) && defined(__GNUC__)
#define WARN_DEPRECATED __attribute__ ((deprecated))
#else
#define WARN_DEPRECATED
#endif

#endif
