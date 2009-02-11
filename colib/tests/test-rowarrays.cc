#include <stdio.h>
#include "narray.h"
#include "quicksort.h"
#include "narray-util.h"
#include "rowarrays.h"

using namespace colib;

void print(floatarray &a) {
    for(int i=0;i<a.dim(0);i++) {
        for(int j=0;j<a.dim(1);j++) 
            printf("%g ",a(i,j));
        printf("\n");
    }
    printf("\n");
}

int main(int argc,char **argv) {
    floatarray a;
    make_random(a,25,1.0);
    make_random(a,25,1.0);
    a.reshape(5,5);
    // print(a);
    rowsort(a);
    // print(a);
    check_rowsorted(a);
}
