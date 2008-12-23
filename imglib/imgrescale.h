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
// File: imgrescale.h
// Purpose: bicubic (and other) scaling
// Responsible: mezhirov
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#ifndef h_imgrescale_
#define h_imgrescale_

#include "colib/colib.h"

namespace iulib {
    void rough_rescale(colib::floatarray &dst, const colib::floatarray &src, int w, int h);
    void bicubic_rescale(colib::floatarray &dst, const colib::floatarray &src, int w, int h);
    void rescale(colib::floatarray &dst, const colib::floatarray &src, int w, int h);
    void rescale(colib::bytearray &dst, const colib::bytearray &src, int w, int h);
    void rescale_to_width(colib::floatarray &dst, const colib::floatarray &src, int w);
    void rescale_to_width(colib::bytearray &dst, const colib::bytearray &src, int w);
    void rescale_to_height(colib::floatarray &dst, const colib::floatarray &src, int h);
    void rescale_to_height(colib::bytearray &dst, const colib::bytearray &src, int h);
}

#endif
