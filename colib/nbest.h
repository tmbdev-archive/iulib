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
// File: nbest.h
// Purpose: simple data structure for keeping track of nbest results
// Responsible: tmb
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

/// \file nbest.h
/// \brief A structure to keep n best objects.

#ifndef h_nbest__
#define h_nbest__


namespace colib {

    class NBest {
    public:
        int n;
        int fill;
        int *ids;
        double *values;
        /// constructor for a NBest data structure of size n
        NBest(int n):n(n) {
            ids = new int[n+1];
            values = new double[n+1];
            clear();
        }
        /// destructor
        ~NBest() {
            delete [] ids;
            delete [] values;
            memset(this,0,sizeof *this);
        }
        /// remove all elements
        void clear() {
            fill = 0;
            for(int i=0;i<=n;i++) ids[i] = -1;
            for(int i=0;i<=n;i++) values[i] = -1e38;
        }
        /// add the id with the corresponding value
        bool add(int id,double value) {
            if(fill==n) {
                int i = n-1;
                if(values[i]>=value) return false;
                while(i>0) {
                    if(values[i-1]>=value) break;
                    values[i] = values[i-1];
                    ids[i] = ids[i-1];
                    i--;
                }
                values[i] = value;
                ids[i] = id;
            } else if(fill==0) {
                values[0] = value;
                ids[0] = id;
                fill++;
            } else {
                int i = fill;
                while(i>0) {
                    if(values[i-1]>=value) break;
                    values[i] = values[i-1];
                    ids[i] = ids[i-1];
                    i--;
                }
                values[i] = value;
                ids[i] = id;
                fill++;
            }
            return true;
        }
        /// get the value corresponding to rank i
        double value(int i) {
            if(unsigned(i)>=unsigned(n)) throw "range error";
            return values[i];
        }
        /// get the id corresponding to rank i
        int operator[](int i) {
            if(unsigned(i)>=unsigned(n)) throw "range error";
            return ids[i];
        }
        /// get the number of elements in the NBest structure (between 0 and n)
        int length() {
            return fill;
        }
    };

}

#endif
