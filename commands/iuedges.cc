extern "C" {
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
};

#include "colib/colib.h"
#include "imglib.h"
#include "imgio.h"

using namespace colib;
using namespace iulib;

param_float sigma("sigma",2.0,"size of Gaussian used for smoothing");
param_float frac("frac",0.3,"Canny fractile");
param_float low("low",2.0,"Canny low parameter");
param_float high("high",4.0,"Canny high parameter");

int main(int argc,char **argv) {
    try {
        bytearray image;
        read_image_gray(image,argv[1]);
        floatarray fimage;
        copy(fimage,image);
        floatarray out;
        canny(out,fimage,sigma,sigma,frac,low,high);
        bytearray edges;
        makelike(edges,out);
        for(int i=0;i<out.length1d();i++)
            edges.at1d(i) = (out.at1d(i)>0)?255:0;
        write_png(stdio(argv[2],"w"),edges);
    } catch(const char *message) {
        fprintf(stderr,"ERROR: %s\n",message);
        exit(255);
    }
}
