A library of image understanding-related algorithms.  Provides basic image processing, mathematical morphology, and machine learning algorithms.

iulib
=====

[![Join the chat at https://gitter.im/tmbdev/iulib](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/tmbdev/iulib?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Iulib implements easy-to-use image and video I/O functions, as well as a large number of common image processing functions.

Design goals are high portability, easy-to-understand algorithms, no reliance on STL, and using a minimal set of data structures.  Almost all interfaces are written in terms of just numbers and the simple narray array class (see below).

Usage:

    #include <iulib/imgio.h>
    #include <iulib/imglib.h>
    using namespace iulib;


Functionality includes:

   * image and video I/O functions
   * distance transform with different metrics using brushfires
   * functions related to Canny edge detection
   * ridge detection, laplacians, local maxima/minima, corner detection, median filter
   * 1D and 2D convolution with Gaussians
   * gray level morphology (erosion, dilation, etc.)
   * connected component labeling and analysis
   * rotation, interpolation, scaling
   * valley and peak detection, histograms, cropping
   * binary morphology on unpacked images
   * fast bit-blit operations on packed binary images
   * fast binary morphology on packed binary images
   * fast binary morphology on run length encoded images

If you use the run-length morphological operations in published work, please cite 

Breuel, Thomas M. "Binary Morphology and Related Operations on Run-Length Representations." In VISAPP (1), pp. 159-166. 2008.

colib
=====

Colib is a simple, templatized library of basic data structures and algorithms.  It is the collections library we think C++ should have had.  Colib is designed to be safe and simple, and to use concepts that work across many programming languages.  In particular, colib encourages the use of integer indexes and discourages the use of iterators and pointers.

Usage: 

    #include <colib/colib.h>
    using namespace colib;


Functionality includes:

   * basic assertions and unit testing macros
   * multidimensional arrays
   * mathematical arrays on narrays
   * algorithms on narrays
   * normalized unicode strings
   * sorting of arrays 
   * 2D points and rectangles with integer coordinates
   * reference counted smart pointers
   * parameter passing
   * integer and string hash tables
   * interval arithmetic
   * convenience functions
   * nbest data structure
   * collection of heap allocated objects
   * interfaces for machine learning and pattern recognition classifiers
   * interfaces for OCR components
   * basic data types for C programming, implemented as macros
   * minimal set of macros and functions for cross-platform compatibility

Installation
============

The code is quite portable and has been compiled on different versions of Linux, Solaris, Windows, and OS X.  Right now, we are focusing our efforts on Ubuntu Linux.  To install:

   * check out a copy under the Source tab
   * cd iulib; scons
   * sudo scons install

For full functionality, you need a number of image I/O libraries:

    sudo apt-get install libpng12-dev libjpeg62-dev libtiff4-dev libavcodec-dev libavformat-dev libsdl-gfx1.2-dev libsdl-image1.2-dev 

