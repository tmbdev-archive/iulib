// -*- C++ -*-

// Copyright 2006-2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz 
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
// Project: iulib -- image understanding library
// File: test-imgbrushfire.cc
// Purpose: testing brushfire
// Responsible: mezhirov
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de

#include <math.h>
#include "colib/colib.h"
#include "imgbrushfire.h"


using namespace iulib;
using namespace colib;

namespace iulib {

    typedef double Metric(int, int, int, int);

    static double metric_1(int x1, int y1, int x2, int y2) {
        return abs(x1-x2) + abs(y1-y2);
    }

    static double metric_2(int x1, int y1, int x2, int y2) {
        return sqrt((x1-x2) * (x1-x2) + (y1-y2) * (y1-y2));
    }

    static double metric_inf(int x1, int y1, int x2, int y2) {
        return max(abs(x1-x2), abs(y1-y2));
    }

    /// This is a direct implementation of distance transform to test brushfire().
    static void distance_transform(Metric metric, floatarray &distance,
            narray<point> &cloud, float maxdist=1e30) {
        for (int x = 0; x < distance.dim(0); x++) {
            for (int y = 0; y < distance.dim(1); y++) {
                double best = maxdist;
                for (int i = 0; i < cloud.length(); i++) {
                    double d = metric(x, y, cloud[i].x, cloud[i].y);
                    if (d < best)
                        best = d;
                }
                distance(x, y) = best;
            }
        }
    }

    static double runif() {
        return double(rand()) / RAND_MAX;
    }

    static int runif_int(int n) {
        return int(runif() * n);
    }

    static void fill_random_points(floatarray &f, narray<point> &cloud, int w,
            int h, int n) {
        f.resize(w, h);
        fill(f, 0);
        cloud.clear();
        for (int i = 0; i < n; i++) {
            int x = runif_int(w);
            int y = runif_int(h);
            cloud.push(point(x, y));
            f(x, y) = 1;
        }
    }

    void test_brushfire(Metric metric, float maxdist, void brushfire(floatarray&, float), int w, int h, int npoints) {
        narray<point> cloud;
        for (int attempt=0; attempt < 10; attempt++) {
            floatarray f;
            fill_random_points(f, cloud, w, h, npoints);
            floatarray dt_result;
            makelike(dt_result, f);
            distance_transform(metric, dt_result, cloud, maxdist);
            brushfire(f, maxdist);

            //now compare dt_result with f
            ASSERT(samedims(dt_result, f));
            for (int i = 0; i < f.length1d(); i++)
                f.at1d(i) = fabs(f.at1d(i) - dt_result.at1d(i));
            if (max(f) > 1)
                printf("%g\n", max(f));
        }
    }

    void test_brushfire(float maxdist, int w, int h, int npoints) {
        test_brushfire(metric_1, maxdist, brushfire_1, w, h, npoints);
        test_brushfire(metric_2, maxdist, brushfire_2, w, h, npoints);
        test_brushfire(metric_inf, maxdist, brushfire_inf, w, h, npoints);
    }

}


int main() {
    float maxdist = 1e30; // for lesser maxdist it simply doesn't work!
    //for(int maxdist = 1; maxdist < 1e10; maxdist*=2)
    for (int w = 4; w < 20; w+=3)
        for (int h = 4; h < 20; h+=3)
            for (int n = 2; n < 20; n++)
                test_brushfire(maxdist, w, h, n);
    return 0;
}

