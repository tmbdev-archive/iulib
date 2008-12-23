// -*- C++ -*-

#ifndef h_autoinvert__
#define h_autoinvert__

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
// File: autoinvert.h
// Purpose: interface to autoinvert.cc
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#include "colib/colib.h"

namespace iulib {
    void make_page_black(colib::bytearray &image);
    void make_page_normalized_and_black(colib::bytearray &image);
    void make_page_binary_and_black(colib::bytearray &image);
    void make_page_binary_and_black(colib::intarray &image);
}

#endif
