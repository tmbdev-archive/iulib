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
// File: misc.h
// Purpose: miscellaneous utility functions
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

/// \file misc.h
/// \brief A few utility functions.


#ifndef h_misc__
#define h_misc__

#include <string.h>
#include <stdlib.h>

namespace colib {

    /// Swap two values

    template <class T,class U>
    inline void swap(T &a,U &b) {
        T temp = a;
        a = b;
        b = temp;
    }

    /// The min of two integer values.

    template <class T,class U>
    inline T min(T a,U b) {
        return a<b?a:b;
    }

    /// The max of two integer values.

    template <class T,class U>
    inline T max(T a,U b) {
        return a>b?a:b;
    }

    /// Square of a value.

    template <class T>
    inline T sqr(T x) {
        return x*x;
    }

    /// Absolute value of a value.

    template <class T>
    inline T abs(T x) {
        return x<0?-x:x;
    }

    /// Heaviside function of a value.

    template <class T>
    inline T heaviside(T x) {
        return x<0?0:x;
    }

    /// Boolean values, initialized to false; useful with hashtable implementations.

    template <bool INITIAL=false>
    struct Boolean {
        bool value;
        Boolean() {
            value = INITIAL;
        }
        operator bool&() {
            return value;
        }
        void operator=(bool other) {
            value = other;
        }
    };

    struct Bool {
        bool value;
        Bool() {
            value = false;
        }
        operator bool&() {
            return value;
        }
        void operator=(bool other) {
            value = other;
        }
    };

    /// Integer values initialized to the given value; useful with hashtable implementations.

    template <int INITIAL>
    struct Integer {
        int value;
        Integer() {
            value = INITIAL;
        }
        operator int&() {
            return value;
        }
        void operator=(int other) {
            value = other;
        }
    };

    struct Int {
        int value;
        Int() {
            value = 0;
        }
        operator int&() {
            return value;
        }
        void operator=(int other) {
            value = other;
        }
    };

    /// Floating point values initialized to the given value; useful with hashtable implementations.

    template <int INITIAL=0> // cannot be float due to C++ restrictions
    struct Floating {
        float value;
        Floating() {
            value = INITIAL;
        }
        operator float&() {
            return value;
        }
        void operator=(int other) {
            value = other;
        }
    };

    struct Float {
        float value;
        Float() {
            value = 0.0;
        }
        operator float&() {
            return value;
        }
        void operator=(int other) {
            value = other;
        }
    };

    /// Abort and exception as functions.

    inline bool die(const char *s) {
        fprintf(stderr,"FATAL: %s\n",s);
        abort();
        return 0;
    }

    inline bool error(const char *s) {
        throw s;
        return 0;
    }
}

#endif
