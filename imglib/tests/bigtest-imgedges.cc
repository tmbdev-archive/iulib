
#include <stdio.h>
#include "colib/colib.h"
#include "imgio.h"
#include "imglib.h"


using namespace iulib;
using namespace colib;


static void random_binary_image(bytearray &b, int w, int h) {
  b.resize(w, h);
  for(int i = 0; i < b.length1d(); i++) {
    b.at1d(i) = 255. * rand() / RAND_MAX;
  }
  gauss2d(b, 2, 2);
  binarize_by_threshold(b);
}

static void zebra(bytearray &b, int w, int h) {
    // Return an image with horizontal black and white stripes.
    // This case causes out-of-bounds exceptions in certain code revisions.
  b.resize(w, h);
  for(int x = 0; x < w; x++) {
    for(int y = 0; y < h; y++) {
      b(x, y) = (x % 4 ? 255 : 0);
    }
  }
}


// Returns true if there is no T-shape in the image:
//
//  @@@
//   @
//
// Here @ stands for nonzero pixel values.
//
// Also, if the corner contains an L shape,
//
// @@
// @
//
// returns false.
bool free_from_t_shapes(bytearray &b) {
  int w = b.dim(0);
  int h = b.dim(1);
  for(int x = 0; x < w; x++) {
    for(int y = 0; y < h; y++) {
      if(!b(x,y)) continue;
      int nneighbors = !!bat(b, x - 1, y, 0)
          + !!bat(b, x + 1, y, 0)
          + !!bat(b, x, y - 1, 0)
          + !!bat(b, x, y + 1, 0);
      if(nneighbors >= 3)
        return false;
            
      int borders_x = (x == 0) + (x == w - 1);
      int borders_y = (y == 0) + (y == h - 1);
      if(nneighbors == 2 && borders_x + borders_y == 2)
        return false;
    }
  }
  return true;
}


int main(int argc,char **argv) {

  bytearray b;
  int r1, r2, r3;
  floatarray gradm;
  read_image_binary(b,stdio("mybitmap.png","rb") );

  intarray image;
  floatarray img;
  copy(image, b);
  copy(img, b);
  

  canny(gradm,img,2.0,2.0,0.3,0.1,0.1);
  TEST_OR_DIE(contains_only(gradm,0) == false);
  
  bytearray x;
  copy(x,b);
  thin(x);

  bytearray b2;
  copy(b2,gradm);
  for (int i = 0; i <gradm.dim(0); i++)
    for (int j = 0; j <gradm.dim(1); j++)
      if(gradm(i,j) )
        b2(i,j)=255;

  write_image_binary("canny_result.png",b2);
  write_image_binary("thin_result.png",x);

  TEST_OR_DIE(free_from_t_shapes(b2));
  
  bytearray c;
  read_image_binary(c,stdio("frog_sketch.png","rb"));
  floatarray gradm2;
  floatarray img2;
  copy(img2, c);
  canny(gradm2,img2, 2.0, 2.0, 0.3, 1.0,1.0 );//2.0, 4.0);
  TEST_OR_DIE(contains_only(gradm2,0) == false);
  
  copy(x,c);
  thin(x);
  
  copy(b2,gradm2);
  for (int i = 0; i <gradm2.dim(0); i++)
    for (int j = 0; j <gradm2.dim(1); j++)
      if(gradm2(i,j) )
        b2(i,j)=255;

  write_image_binary("canny_result2.png",b2);
  write_image_binary("thin_result2.png",x);

  TEST_OR_DIE(free_from_t_shapes(b2));

  
  
}
