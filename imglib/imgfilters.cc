// -*- C++ -*-

// Copyright 2006-2007 Deutsches Forschungszentrum fuer Kuenstliche Intelligenz
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
// File: imgfilters.cc
// Purpose: misc image filters: ridges, zero crossings, etc.
// Responsible: tmb
// Reviewer:
// Primary Repository:
// Web Sites: www.iupr.org, www.dfki.de

extern "C" {
#include <math.h>
}

#include "colib/colib.h"
#include "imglib.h"


using namespace colib;

namespace iulib {

    // ridge detection

    struct RidgeDetector {
        floatarray &mk2;
        floatarray mk1;
        floatarray mpx;
        floatarray mpy;
        floatarray mdx;
        floatarray mdy;
        int dummy;

        inline bool isridge(int x, int y, int dx, int dy) {
            int i0 = &mk2(x, y) - &mk2(0, 0);
            int i1 = &mk2(x+dx, y+dy) - &mk2(0, 0);
#define a(p) p.unsafe_at1d(i0)
#define b(p) p.unsafe_at1d(i1)
#define dot(u,v,s,t)((u)*(s)+(v)*(t))
            if (a(mk2) >= 0.0)
                return 0;
            if (b(mk2) >= 0.0)
                return 0;
            if (fabs(a(mk1)) >= fabs(a(mk2)))
                return 0;
            if (fabs(b(mk1)) >= fabs(b(mk2)))
                return 0;
            if (dot(a(mdx), a(mdy), b(mdx), b(mdy)) >
            fabs(dot(a(mpx), a(mpy), b(mpx), b(mpy))))
                return 0;
            if (dot(a(mdx), a(mdy), a(mpx), a(mpy)) *
            dot(b(mdx), b(mdy), b(mpx), b(mpy)) *
            dot(a(mpx), a(mpy), b(mpx), b(mpy)) > 0.0)
                return 0;
            return 1;
#undef dot
#undef b
#undef a
        }

        RidgeDetector(floatarray &im, floatarray &zero, floatarray &strength,
                floatarray &angle) : mk2(strength) {
            makelike(mk2, im);
            mk2 = 0;
            mk1 = mk2;
            mpx = mk2;
            mpy = mk2;
            mdx = mk2;
            mdy = mk2;
            angle = mk2;

            int w = im.dim(0), h = im.dim(1);

#define mfi(x,y) ext(im,x,y)
#define vis_hypot(x,y) sqrt((x)*(x)+(y)*(y))

            int x;
            for (x=w-2; x>=1; x--) {
                for (int y=h-2; y>=1; y--) {
                    float ndx = 0.5 *(ext(im, x + 1, y) - ext(im, x - 1, y));
                    float ndy = 0.5 *(ext(im, x, y + 1) - ext(im, x, y - 1));
                    float t= vis_hypot(ndx, ndy);
                    if (!t)
                        t = 1.0;
                    float dx = ndx / t;
                    float dy = ndy / t;
                    float dxx = ext(im, x - 1, y) + ext(im, x + 1, y) - 2.0
                            * ext(im, x, y);
                    float dxy = .25 *(ext(im, x - 1, y - 1) - ext(im, x - 1, y
                            + 1) - ext(im, x + 1, y - 1)
                            + ext(im, x + 1, y + 1));
                    float dyy = ext(im, x, y - 1) + ext(im, x, y + 1) - 2.0
                            * ext(im, x, y);
                    float di2 = dyy * dyy - 2 * dxx * dyy + 4 * dxy * dxy + dxx
                            * dxx;
                    float di = sqrt(fabs(di2));
                    float k2 =(-di + dyy + dxx) / 2.0;
                    float k1 =(di + dyy + dxx) / 2.0;
                    float ny2 = dxy ? -(di - dyy + dxx) / dxy / 2.0 : 0.0;
                    float x2 = 1.0 /(t = vis_hypot(1.0, ny2));
                    float y2 = ny2 / t;

                    mk2(x, y) = k2;
                    mk1(x, y) = k1;
                    mpx(x, y) = x2;
                    mpy(x, y) = y2;
                    mdx(x, y) = dx;
                    mdy(x, y) = dy;
                }
            }

            fill(zero, 0);

            for (x=w-2; x>=1; x--) {
                for (int y=h-2; y>=1; y--) {
                    zero(x, y) = (isridge(x, y, 0, -1) || isridge(x, y, -1, 0) ||
                            isridge(x, y, 0, 1) || isridge(x, y, 1, 0));
                    angle(x, y) = mpx(x,y) ? atan2(mpy(x,y), mpx(x,y)) : 0.0;
                }
            }
        }
    };

    /// Perform ridge detection using the Horn-Riley ridge detector.

    void horn_riley_ridges(floatarray &im, floatarray &zero,
            floatarray &strength, floatarray &angle) {
        RidgeDetector detector(im, zero, strength, angle);
    }

    /// Compute the laplacian of an image.
    /// This function actually computes minus laplacian.
    /// Making it deprecated until we're sure the users updated to
    /// plus_laplacian(). --IM
    void laplacian(floatarray &result, floatarray &image) {
        makelike(result, image);
        for (int i=0; i<image.dim(0); i++) {
            for (int j=0; j<image.dim(1); j++) {
                float v = 4 * ext(image, i, j) - ext(image, i-1, j) - ext(
                        image, i+1, j) - ext(image, i, j-1)
                        - ext(image, i, j+1);
                result(i, j) = v;
            }
        }
    }

    /// Compute the laplacian of an image.
    /// (This function should be renamed to laplacian() once nobody uses
    ///  old laplacian() anymore). --IM
    void plus_laplacian(floatarray &result, floatarray &image) {
        makelike(result, image);
        for (int i=0; i<image.dim(0); i++) {
            for (int j=0; j<image.dim(1); j++) {
                result(i, j) = ext(image, i-1, j) + ext(image, i+1, j)
                             + ext(image, i, j-1) + ext(image, i, j+1)
                             - 4 * ext(image, i, j);
            }
        }
    }

    //inline int fsign_(float v) {if(v<0) return -1; if(v>0) return 1; return 0;}
    inline int fsign_(float v) {
        if (v<0)
            return -1;
        else
            return 1;
    }

    /// Find zerocrossings in an image.

    void zero_crossings(bytearray &result, floatarray &image) {
        result.resize(image.dim(0), image.dim(1));
        fill(result, 0);
        for (int i=image.dim(0)-2; i>=1; i--) {
            for (int j=image.dim(1)-2; j>=1; j--) {
                int sgn = fsign_(image(i, j));
                if (fsign_(image(i+1, j+1))!=sgn) {
                    result(i, j)=1;
                    continue;
                }
                if (fsign_(image(i, j+1))!=sgn) {
                    result(i, j)=1;
                    continue;
                }
                if (fsign_(image(i+1, j))!=sgn) {
                    result(i, j)=1;
                    continue;
                }
                if (fsign_(image(i-1, j+1))!=sgn) {
                    result(i, j)=1;
                    continue;
                }
            }
        }
    }

    /// Find local minima in an image.

    void local_minima(bytearray &result, floatarray &image) {
        CHECK_ARG(image.rank()==2);
        makelike(result, image);
        fill(result, 0);
        for (int i=image.dim(0)-2; i>=1; i--) {
            for (int j=image.dim(1)-2; j>=1; j--) {
                if (image(i, j)>=image(i+1, j))
                    continue;
                if (image(i, j)>=image(i+1, j+1))
                    continue;
                if (image(i, j)>=image(i+1, j-1))
                    continue;
                if (image(i, j)>=image(i, j+1))
                    continue;
                if (image(i, j)>=image(i, j-1))
                    continue;
                if (image(i, j)>=image(i-1, j))
                    continue;
                if (image(i, j)>=image(i-1, j+1))
                    continue;
                if (image(i, j)>=image(i-1, j-1))
                    continue;
                result(i, j) = 1;
            }
        }
    }

    /// Find local maxima in an image.

    void local_maxima(bytearray &result, floatarray &image) {
        CHECK_ARG(image.rank()==2);
        makelike(result, image);
        fill(result, 0);
        for (int i=image.dim(0)-2; i>=1; i--) {
            for (int j=image.dim(1)-2; j>=1; j--) {
                if (image(i, j)<=image(i+1, j))
                    continue;
                if (image(i, j)<=image(i+1, j+1))
                    continue;
                if (image(i, j)<=image(i+1, j-1))
                    continue;
                if (image(i, j)<=image(i, j+1))
                    continue;
                if (image(i, j)<=image(i, j-1))
                    continue;
                if (image(i, j)<=image(i-1, j))
                    continue;
                if (image(i, j)<=image(i-1, j+1))
                    continue;
                if (image(i, j)<=image(i-1, j-1))
                    continue;
                result(i, j) = 1;
            }
        }
    }

    /// Gradient-Based Corner Detector

    void gradient_based_corners(floatarray &image) {
        int w = image.dim(0), h = image.dim(1);
        floatarray dx2(w, h), dy2(w, h), dxdy(w, h);
        fill(dx2, 0);
        fill(dy2, 0);
        fill(dxdy, 0);

        for (int i=w-2; i>=1; i--)
            for (int j=h-2; j>=1; j--) {
                float v = image(i, j);
                float dx = v - image(i-1, j);
                float dy = v - image(i, j-1);
                dx2(i, j) = dx*dx;
                dy2(i, j) = dy*dy;
                dxdy(i, j) = dx*dy;
            }

        fill(image, 0);

        for (int i=w-2; i>=1; i--)
            for (int j=h-2; j>=1; j--) {
                // taken from old ridge computation code
                float dxx=dx2(i, j);
                float dxy=dxdy(i, j);
                float dyy=dy2(i, j);
                float di2=dyy*dyy-2*dxx*dyy+4*dxy*dxy+dxx*dxx;
                float di=sqrt(fabs(di2));
                float k2=(-di+dyy+dxx)/2.0;
                image(i, j) = k2; // necessarily the smaller eigenvalue
                //float k1=(di+dyy+dxx)/2.0;
                //float ny2=dxy?-(di-dyy+dxx)/dxy/2.0:0.0;
                //float x2=1.0/(t=vis_hypot(1.0,ny2));
                //float y2=ny2/t;
            }
    }

    /// Kitchen Rosenfeld Corner Detector
    ///
    /// Computes the curvature of the level curves.  If masked is true,
    /// then on input, non-zero elements in "corners" indicate where the
    /// curvature is to be computed.

    void kitchen_rosenfeld_corners(floatarray &corners, floatarray &image) {
        makelike(corners, image);
        fill(corners, 0);
        pad_by(corners, 1, 1, 0.0f);
        int w = image.dim(0), h = image.dim(1);
        for (int i=w-2; i>=1; i--)
            for (int j=h-2; j>=1; j--) {
                float dx = 0.5 * (image(i+1, j) - image(i-1, j));
                float dy = 0.5 * (image(i, j+1) - image(i, j-1));
                float dx2 = dx * dx;
                float dy2 = dy * dy;
                float grad2 = dx2+dy2;
                if (grad2==0.0)
                    continue;
                float dxx = 2*image(i, j)-image(i-1, j)-image(i+1, j);
                float dxy = image(i-1, j-1)+image(i+1, j+1)-image(i+1, j-1)
                        -image(i-1, j+1);
                float dyy = 2*image(i, j)-image(i, j-1)-image(i, j+1);
                float curv = (dxx*dy2 -2.0*dxy*dx*dy + dyy*dx2)/grad2;
                corners(i, j) = curv;
            }
        pad_by(corners, -1, -1, 0.0f);
    }

    /// Kitchen and Rosenfeld Corner Detector
    ///
    /// Same as above, but result has been multiplied by the square of the
    /// gradient magnitude; this is supposedly a better measure of
    /// "cornerness".

    void kitchen_rosenfeld_corners2(floatarray &corners, floatarray &image) {
        makelike(corners, image);
        fill(corners, 0);
        pad_by(corners, 1, 1, 0.0f);
        int w = image.dim(0), h = image.dim(1);
        for (int i=w-2; i>=1; i--)
            for (int j=h-2; j>=1; j--) {
                float dx = 0.5 * (image(i+1, j) - image(i-1, j));
                float dy = 0.5 * (image(i, j+1) - image(i, j-1));
                float dx2 = dx * dx;
                float dy2 = dy * dy;
                float dxx = 2*image(i, j)-image(i-1, j)-image(i+1, j);
                float dxy = image(i-1, j-1)+image(i+1, j+1)-image(i+1, j-1)
                        -image(i-1, j+1);
                float dyy = 2*image(i, j)-image(i, j-1)-image(i, j+1);
                float curv = (dxx*dy2 -2.0*dxy*dx*dy + dyy*dx2);
                corners(i, j) = curv;
            }
        pad_by(corners, -1, -1, 0.0f);
    }

    /// Perform median filtering of the image; the median filter is
    /// applied over a rectangle of size 2rx+1 by 2ry+1.

    void median_filter(bytearray &image, int rx, int ry) {
        int w = image.dim(0);
        int h = image.dim(1);

        int hist[256];

        bytearray out;
        makelike(out, image);

        int medg =(2*rx+1)*(2*ry+1)/2;
        for (int y=0; y<h; y++) {
            int i;
            for (i = 0; i < 256; i++)
                hist[i] = 0;
            for (i=-1-rx; i<=-1+rx; i++)
                for (int j=y-ry; j<=y+ry; j++)
                    hist[ext(image,i, j)]++;
            int med = 0;
            int medn = 0;
            while (medn<medg) {
                medn += hist[med];
                med++;
            }
            while (medn>medg) {
                med--;
                medn -= hist[med];
            }
            for (int x=0; x<w; x++) {
                int j;
                for (j=y-ry; j<=y+ry; j++) {
                    int v = ext(image, x-rx-1, j);
                    hist[v]--;
                    if (v<med)
                        medn--;
                }
                for (j=y-ry; j<=y+ry; j++) {
                    int v = ext(image, x+rx, j);
                    hist[v]++;
                    if (v < med)
                        medn++;
                }
                while (medn < medg) {
                    medn += hist[med];
                    med++;
                }
                while (medn > medg) {
                    med--;
                    medn -= hist[med];
                }
                out(x, y) = med;
            }
        }
        for (int i=0; i<image.length1d(); i++)
            image.at1d(i) = out.at1d(i);
    }

}
