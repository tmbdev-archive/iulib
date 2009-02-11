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
// File: test-imggauss.cc
// Purpose: testing gaussian convolution
// Responsible: syed atif mehdi
// Reviewer: 
// Primary Repository: 
// Web Sites: www.iupr.org, www.dfki.de


#include "colib/colib.h"
#include "imgio.h"
#include "imglib.h"
#include <stdarg.h>


using namespace iulib;
using namespace colib;


//generates gaussian and returns gaussain, mean and variance

void GenerateGaussian(floatarray &in, float &variance, float &mean, const int size)
{
  float sum =0;
  //float sigma = 0;
  in.resize(size);
  for(int i=-size/2, j=0; i<= size/2; i++,j++){
    if(size/2 && !i)
      i++;
    in(j)=i;
    sum +=i;
  }

  mean = sum/size;
  sum =0;
  for (int i =0; i<size; i++){
    in(i)=((in(i)-mean)*(in(i)-mean));
    sum+=in(i);
  }
  variance = sum/size;
//  sigma = sqrt(variance);
  float sq = sqrt(2*3.14*variance);
  for (int i =0; i<size; i++){
    in(i)= exp(- (in(i)/(2*variance))) / sq;
  }

}


//properties of gaussain convolution found here
// http://mathworld.wolfram.com/Convolution.html
void GaussianConvProperty(floatarray &in, float variance, float mean)
{
  int size = in.dim(0);
  floatarray out(size);
  for(int i=0; i<size; i++){
    out(i)=/*1/(sqrt(2*3.14*(variance+variance)))* */exp(-(pow(i-mean,2))/(2*(variance+variance))) ;
  }
  float total = 0.0;
  for (int i=0; i<out.dim(0); i++)
    total += out(i);
  for (int i=0; i<out.dim(0); i++)
    out(i) /= total;

  // FIXME unit tests shouldn't print anything
#if 0
  printf("FUNCTION CALCULATES AS \n");
  for(int i=0; i<size;i ++){
    printf("%f = %f \n",in[i], out[i]);
  }
#endif
}


void GenerateGaussian_sigma(floatarray &in, float sigma) {
  
  int range = 1+int(3.0*sigma);
  in.resize(2*range+1);
  for (int i=0; i<=range; i++) {
    double y = exp(-i*i/2.0/sigma/sigma);
    in(range+i) = in(range-i) = y;
  }
  float total = 0.0;
  for (int i=0; i<in.dim(0); i++)
    total += in(i);
  for (int i=0; i<in.dim(0); i++)
    in(i) /= total;
}



int main(int argc,char **argv) {

  floatarray in;
  floatarray out;
  int size =10;
  
  float sigma =4;
  float mean = 1 + int(3*sigma);
  float variance =sigma*sigma;
  /*
  GenerateGaussian(in, variance, mean,size);
  float sigma=sqrt(variance);
  gauss1d(out, in, sigma);
  for (int i = 0; i <in.dim(0);i ++){
    printf("%f = %f \n",in[i], out[i]);
  }
  */
  GenerateGaussian_sigma(in,sigma);
  gauss1d(out, in, sigma);
  // FIXME unit tests shouldn't print anything
#if 0
  for (int i = 0; i <in.dim(0);i ++){
    printf("%f = %f \n",in[i], out[i]);
    //printf("%f \n",in[i]);
  }
#endif
  GaussianConvProperty(in, variance, mean);
}



