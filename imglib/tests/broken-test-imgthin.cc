
#include <stdio.h>
#include "colib/colib.h"
#include "imgio.h"
#include "imglib.h"


using namespace iulib;
using namespace colib;

/*

// Checks if there are color components with 8- or 4-neighbouring pixels.
// After CC labelling, this function should return false.
static bool components_touch(intarray &image, bool conn_4 = false) {
    static const int dx_4[] = {-1,  0};
    static const int dy_4[] = { 0, -1};
    static const int dx_8[] = {-1, -1, -1,  0};
    static const int dy_8[] = {-1,  0,  1, -1};
    const int *dx = conn_4 ? dx_4 : dx_8;
    const int *dy = conn_4 ? dy_4 : dy_8;
    int nd = conn_4 ? 2 : 4;

    for(int x = 0; x < image.dim(0); x++) {
        for(int y = 0; y < image.dim(1); y++) {
            int p = image(x, y);
            if(!p) continue;
            for(int i = 0; i < nd; i++) {
                int p2 = bat(image, x + dx[i], y + dy[i], 0);
                if(!p2) continue;
                if(p != p2)
                    return true;
            }
        }
    }
    return false;
}

/// Count the difference in number of squares like this:
/// ..       @@
/// .@  and  @.
/// This should be the difference between the number of black and white CCs
/// (with different 4-connectivity!)
/// The conn_4 is for connectivity of zeros, non-zeros have it the other way.
/// (probably we can count only part of the squares, not all of them)
/// This actually works only with padding, without padding groups touching the
/// border like this:
/// 
/// ........
/// ....@...
/// ________
/// 
/// will lead to the wrong answer.
///
static int count_odd_squares_diff(bytearray &image, bool conn_4) {
    int result = 0;
    for(int x = 0; x < image.dim(0); x++) {
        for(int y = 0; y < image.dim(1); y++) {
            int p = 255 - image(x, y);
            bool ignore_diag = p ? conn_4 : !conn_4;
            for(int dx = -1; dx <= 1; dx += 2) {
                for(int dy = -1; dy <= 1; dy += 2) {
                    if(p == bat(image, x + dx, y, p)
                    && (ignore_diag || p == bat(image, x + dx, y + dy, p))
                    && p == bat(image, x, y + dy, p)) {
                        result += p ? 1 : -1;
                    }
                }
            }
        }
    }
    CHECK_CONDITION(result % 4 == 0);
    return result / 4;
}

static void test_labelling_on_binary_image(bytearray &b) {  
    intarray image;
    copy(image, b);
    label_components(image); // here we test unpadded image for out-of-bounds

    pad_by(b, 1, 1, byte(0));
    copy(image, b);
    int d = count_odd_squares_diff(b, true);
    label_components(image, false);
    int n = max(image);
    for(int i = 0; i < b.length1d(); i++)
        b.at1d(i) = 255 - b.at1d(i);
    copy(image, b);
    label_components(image, true);
    int m = max(image);
    TEST_OR_DIE(d == m - n);
    TEST_OR_DIE(!components_touch(image, true));
}

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
            b(x, y) = (y % 2 ? 255 : 0);
        }
    }
}

int main(int argc,char **argv) {
    intarray image;
    bytearray b;

    zebra(b, 10, 10);
    test_labelling_on_binary_image(b);
    zebra(b, 1, 1);
    test_labelling_on_binary_image(b);
    zebra(b, 1, 5);
    test_labelling_on_binary_image(b);

    for(int i = 0; i < 20; i++) {
        random_binary_image(b, 50, 50);
        test_labelling_on_binary_image(b);
    }

    image.resize(512,512);
    fill(image,0);
    // we start count at 1 because label_components 
    // counts the background as well
    int count = 1;
    for(int i=10;i<500;i+=10) for(int j=10;j<500;j+=10) {
        image(i,j) = 1;
        count++;
    }
    int n = label_components(image);
    TEST_OR_DIE(n==count);
    TEST_OR_DIE(n==max(image)+1);
    // relabeling shouldn't change things
    n = label_components(image);
    TEST_OR_DIE(n==count);
    // recoloring shouldn't change the number of components
    simple_recolor(image);
    n = label_components(image);
    TEST_OR_DIE(n==count);
    narray<rectangle> boxes;
    bounding_boxes(boxes,image);
    TEST_OR_DIE(boxes.length()==count);
    // component 0 is the background
    TEST_OR_DIE(boxes[0].width()==image.dim(0));
    TEST_OR_DIE(boxes[0].height()==image.dim(1));
    // all other bounding boxes should be one pixel large
    for(int i=1;i<boxes.length();i++) {
        TEST_OR_DIE(boxes[i].width()==1);
        TEST_OR_DIE(boxes[i].height()==1);
    }
}
*/

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

    read_image_binary(b,stdio("mybitmap.png","rb") );

    intarray image;
    copy(image, b);
    label_components(image);
    r1 = max(image);

    thin(b);

    bytearray b2;
    copy(b2, b);

    TEST_OR_DIE(free_from_t_shapes(b2));

    intarray image2;
    copy(image2, b);
    label_components(image2);
    r2 = max(image2);

/*
    bytearray resultant;
    read_image_binary(resultant,stdio("mybitmap_result.png","r") );
    intarray image3;
    copy(image3,resultant);
    label_components(image3);
    r3 = max(image3);
*/


    //  printf("old = %d , new = %d, resultant = %d \n",r1 ,r2, r3);
    TEST_OR_DIE(r1>=r2);
    //    write_image_binary("mybitmap_r.png",b);


/*
	bytearray b;
	read_image_binary(b,stdio("letters-results-lbl.png","r") );
    	int r1, r2;
	intarray image;
    copy(image, b);
    label_components(image);
    r1 = max(image);


    thin(b);

    intarray image2;
    copy(image2, b);
    label_components(image2);
    r2 = max(image2);

    printf("old = %d , new = %d \n",r1 ,r2);
	write_image_binary("letters-results-lbl_result.png",b);
*/
}
