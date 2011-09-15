// -*- C++ -*-

%{
#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#pragma GCC diagnostic ignored "-Wuninitialized"
%}

%module(docstring="Low-level bindings to the iulib library.") iulib;
%feature("autodoc",1);
%include "typemaps.i"
#ifdef SWIGPYTHON
%include "cstring.i"
#endif
%{
#include <malloc.h>
#include <string.h>
#include <colib/narray.h>
#include <colib/narray-util.h>
#include <colib/narray-ops.h>
#include <colib/nustring.h>
#include <colib/iustring.h>
#include <iulib/imgio.h>
#include <iulib/imgedges.h>
#include <iulib/imgmisc.h>
#include <iulib/imgbrushfire.h>
#include <iulib/imgfilters.h>
#include <iulib/imggauss.h>
#include <iulib/imggraymorph.h>
#include <iulib/imglabels.h>
#include <iulib/imgmap.h>
#include <iulib/imgmorph.h>
#include <iulib/imgops.h>
#include <iulib/imgrescale.h>
#include <iulib/imgthin.h>
#include <iulib/imgtrace.h>
#include <iulib/imgbits.h>
#include <iulib/imgrle.h>
#include <iulib/dgraphics.h>
using namespace colib;
using namespace iulib;
using namespace narray_ops;
using namespace imgbits;
using namespace imgrle;
%}

#ifdef SWIGPYTHON
%exception {
    try {
        $action
    }
    catch(const char *s) {
        PyErr_SetString(PyExc_IndexError,s);
        return NULL;
    }
    catch(...) {
        PyErr_SetString(PyExc_IndexError,"unknown exception in iulib");
        return NULL;
    }
}
#endif

%{
#include "numpy/arrayobject.h"
%}

%init %{
import_array();
%}

%inline
%{
    template <class T> void makelike(narray<T> &a,PyObject *object_) {
        if(!object_) throw "null pointer";
        if(!PyArray_Check(object_)) throw "expectd a numpy array";
        PyArrayObject *obj = (PyArrayObject *)object_;
        if(obj->nd<1) throw "rank <1";
        if(obj->nd>4) throw "rank >4";
        npy_intp *d = obj->dimensions;
        if(obj->nd==1) a.resize(d[0]);
        else if(obj->nd==2) a.resize(d[0],d[1]);
        else if(obj->nd==3) a.resize(d[0],d[1],d[2]);
        else if(obj->nd==4) a.resize(d[0],d[1],d[2],d[3]);
    }
    int refcount(PyObject *object) {
        return object->ob_refcnt;
    }
    template <class T> void narray_of_numpy(narray<T> &a,PyObject *object_) {
        if(!object_) throw "null pointer";
        if(!PyArray_Check(object_)) throw "expectd a numpy array";
        PyArrayObject *obj = (PyArrayObject *)object_;
        if((obj->flags&NPY_CONTIGUOUS)==0) {
            obj = (PyArrayObject*)PyArray_ContiguousFromObject(object_,obj->descr->type_num,1,4);
            if(!obj) throw "oops";
        }
        makelike(a,(PyObject*)obj);
        int t = obj->descr->type_num;
        int n = a.length();
        if(t==PyArray_FLOAT) {
            float *data = (float*)PyArray_DATA(obj);
            for(int i=0;i<n;i++) a.unsafe_at1d(i) = data[i];
        }
        else if(t==PyArray_DOUBLE) {
            double *data = (double*)PyArray_DATA(obj);
            for(int i=0;i<n;i++) a.unsafe_at1d(i) = data[i];
        }
        else if(t==PyArray_INT) {
            npy_int *data = (npy_int*)PyArray_DATA(obj);
            for(int i=0;i<n;i++) a.unsafe_at1d(i) = data[i];
        }
        else if(t==PyArray_INT32) {
            npy_int32 *data = (npy_int32*)PyArray_DATA(obj);
            for(int i=0;i<n;i++) a.unsafe_at1d(i) = data[i];
        }
        else if(t==PyArray_INT64) {
            npy_int64 *data = (npy_int64*)PyArray_DATA(obj);
            for(int i=0;i<n;i++) a.unsafe_at1d(i) = data[i];
        }
        else if(t==PyArray_UBYTE) {
            unsigned char *data = (unsigned char *)PyArray_DATA(obj);
            for(int i=0;i<n;i++) a.unsafe_at1d(i) = data[i];
        }
        else {
            throw "unknown numpy array type";
        }
        if((PyObject*)obj!=object_) Py_DECREF(obj);
    }
    template <class T> void numpy_of_narray(PyObject *object_,narray<T> &a) {
        if(!object_)
            throw "null pointer";
        if(!PyArray_Check(object_))
            throw "expectd a numpy array";
        PyArrayObject *obj = (PyArrayObject *)object_;
        int n = a.length();
        if(n!=PyArray_Size(object_))
            throw "input and output array have different sizes";
        if((obj->flags&NPY_CONTIGUOUS)==0)
            throw "output array is not contiguous";
        int t = obj->descr->type_num;
        if(t==PyArray_FLOAT) {
            float *data = (float*)PyArray_DATA(obj);
            for(int i=0;i<n;i++) data[i] = a.unsafe_at1d(i);
        }
        else if(t==PyArray_DOUBLE) {
            double *data = (double*)PyArray_DATA(obj);
            for(int i=0;i<n;i++) data[i] = a.unsafe_at1d(i);
        }
        else if(t==PyArray_INT) {
            int32_t *data = (int32_t*)PyArray_DATA(obj);
            for(int i=0;i<n;i++) data[i] = a.unsafe_at1d(i);
        }
        else if(t==PyArray_UBYTE) {
            unsigned char *data = (unsigned char *)PyArray_DATA(obj);
            for(int i=0;i<n;i++) data[i] = a.unsafe_at1d(i);
        }
        else {
            throw "unknown numpy array type";
        }
    }
%}

%pythoncode
%{
from numpy import zeros
def numpy(a,type='f'):
    shape = []
    for i in range(4):
        if a.dim(i)==0: break
        shape.append(a.dim(i))
    result = zeros(shape,type)
    numpy_of_narray(result,a)
    return result
def narray(a,type='f'):
    result = None
    if type=='B':
        result = bytearray()
    elif type=='i':
        result = intarray()
    elif type=='f':
        result = floatarray()
    else:
        raise Exception('%s: unknown type',type)
    narray_of_numpy(result,a)
    return result
%}

void narray_of_numpy(bytearray &,PyObject *);
void narray_of_numpy(intarray &,PyObject *);
void narray_of_numpy(floatarray &,PyObject *);
void numpy_of_narray(PyObject *,bytearray &);
void numpy_of_narray(PyObject *,intarray &);
void numpy_of_narray(PyObject *,floatarray &);

////////////////////////////////////////////////////////////////
// narray.h
////////////////////////////////////////////////////////////////

typedef unsigned char byte;

template <class T>
class narray {
    public:

    narray();
    narray(int);
    narray(int,int);
    narray(int,int,int);
    narray(int,int,int,int);

    int length();
    int rank();
    int dim(int);
    void clear();
    void resize(int,int d1=0,int d2=0,int d3=0);
    void reshape(int,int d1=0,int d2=0,int d3=0);

    T at(int);
    T at(int,int);
    T at(int,int,int);
    T at(int,int,int,int);

    void put(int,T);
    void put(int,int,T);
    void put(int,int,int,T);
    void put(int,int,int,int,T);

    T at1d(int);
    void put1d(int,T);

    void set(T);
    void set(T,T);
    void set(T,T,T);
    void set(T,T,T,T);

    void fill(T);
    void copy(narray &);
    void append(narray &);
    void swap(narray &);
    void move(narray &);

    void copy(floatarray &);
    void copy(intarray &);
    void copy(bytearray &);
    void copy(shortarray &);


    void makelike(floatarray &);
    void makelike(intarray &);
    void makelike(bytearray &);
    void makelike(shortarray &);

};

%template(floatarray) narray<float>;
typedef narray<float> floatarray;
%template(intarray) narray<int>;
typedef narray<int> intarray;
%template(bytearray) narray<byte>;
typedef narray<byte> bytearray;
%template(shortarray) narray<short>;
typedef narray<short> shortarray;

%extend narray<byte> {
#ifdef SWIGPYTHON
    %cstring_output_allocate_size(char **s,int *slen,free(*$1));
    void tostring(char **s,int *slen) {
        *slen = $self->total * sizeof *$self->data;
        *s = (char*)malloc(*slen);
        memcpy(*s,$self->data,*slen);
    }
#endif
    void fromstring(char *s,int len) {
        $self->resize(len/sizeof *$self->data);
        //memcpy($self->data,s,len);
        memcpy($self->data,s,len);
    }

    //extend the operators from narray-ops.h here
    bytearray &operator+=(byte x) {
        ::operator+=(*$self, x);
        return *$self;
    }
    bytearray &operator+=(bytearray &x) {
        ::operator+=(*$self, x);
        return *$self;
    }
    bytearray &operator-=(byte x) {
        ::operator-=(*$self, x);
        return *$self;
    }
    bytearray &operator-=(bytearray &x) {
        ::operator-=(*$self, x);
        return *$self;
    }
    bytearray &operator*=(byte x) {
        ::operator*=(*$self, x);
        return *$self;
    }
    bytearray &operator*=(bytearray &x) {
        ::operator*=(*$self, x);
        return *$self;
    }
    bytearray &operator/=(byte x) {
        ::operator/=(*$self, x);
        return *$self;
    }
    bytearray &operator/=(bytearray &x) {
        ::operator/=(*$self, x);
        return *$self;
    }

    bool operator==(bytearray & x) {
        if( $self->rank() != x.rank() )
                return false;
        for( int i = 0; i < x.rank(); i++ ) {
            if($self->dim(i) != x.dim(i))
                return false;
        }
        for( int i = 0; i < x.length1d(); i++ ) {
            if($self->at1d(i) != x.at1d(i))
                return false;
        }
        return true;
    }

}


%extend narray<float> {
#ifdef SWIGPYTHON
    %cstring_output_allocate_size(char **s,int *slen,free(*$1));
    void tostring(char **s,int *slen) {
        *slen = $self->total * sizeof *$self->data;
        *s = (char*)malloc(*slen);
        memcpy(*s,$self->data,*slen);
    }
#endif
    void fromstring(char *s,int len) {
        $self->resize(len/sizeof *$self->data);
        memcpy($self->data,s,len);
    }
    floatarray &operator+=(float x) {
        ::operator+=(*$self, x);
        return *$self;
    }
    floatarray &operator+=(floatarray &x) {
        ::operator+=(*$self, x);
        return *$self;
    }
    floatarray &operator-=(float x) {
        ::operator-=(*$self, x);
        return *$self;
    }
    floatarray &operator-=(floatarray &x) {
        ::operator-=(*$self, x);
        return *$self;
    }
    floatarray &operator*=(float x) {
        ::operator*=(*$self, x);
        return *$self;
    }
    floatarray &operator*=(floatarray &x) {
        ::operator*=(*$self, x);
        return *$self;
    }
    floatarray &operator/=(float x) {
        ::operator/=(*$self, x);
        return *$self;
    }
    floatarray &operator/=(floatarray &x) {
        ::operator/=(*$self, x);
        return *$self;
    }

    bool operator==(floatarray & x) {
        if( $self->rank() != x.rank() )
                return false;
        for( int i = 0; i < x.rank(); i++ ) {
            if($self->dim(i) != x.dim(i))
                return false;
        }
        for( int i = 0; i < x.length1d(); i++ ) {
            if($self->at1d(i) != x.at1d(i))
                return false;
        }
        return true;
    }

}

%extend narray<int> {
#ifdef CPYTHON
    %cstring_output_allocate_size(char **s,int *slen,free(*$1));
    void tostring(char **s,int *slen) {
        *slen = $self->total * sizeof *$self->data;
        *s = (char*)malloc(*slen);
        memcpy(*s,$self->data,*slen);
    }
#endif
    void fromstring(char *s,int len) {
        $self->resize(len/sizeof *$self->data);
        memcpy($self->data,s,len);
    }
    intarray &operator+=(int x) {
        ::operator+=(*$self, x);
        return *$self;
    }
    intarray &operator+=(intarray &x) {
        ::operator+=(*$self, x);
        return *$self;
    }
    intarray &operator-=(int x) {
        ::operator-=(*$self, x);
        return *$self;
    }
    intarray &operator-=(intarray &x) {
        ::operator-=(*$self, x);
        return *$self;
    }
    intarray &operator*=(int x) {
        ::operator*=(*$self, x);
        return *$self;
    }
    intarray &operator*=(intarray &x) {
        ::operator*=(*$self, x);
        return *$self;
    }
    intarray &operator/=(int x) {
        ::operator/=(*$self, x);
        return *$self;
    }
    intarray &operator/=(intarray &x) {
        ::operator/=(*$self, x);
        return *$self;
    }

    bool operator==(intarray & x) {
        if( $self->rank() != x.rank() )
                return false;
        for( int i = 0; i < x.rank(); i++ ) {
            if($self->dim(i) != x.dim(i))
                return false;
        }
        for( int i = 0; i < x.length1d(); i++ ) {
            if($self->at1d(i) != x.at1d(i))
                return false;
        }
        return true;
    }

}

%template(floatarrayarray) narray<floatarray>;
typedef narray<floatarray> floatarrayarray;

%extend narray<floatarray> {
    void get(floatarray &result,int i) {
        result = $self->at(i);
    }
    void put(int i,floatarray &v) {
        $self->at(i) = v;
    }
}

%extend narray<short> {
#ifdef SWIGPYTHON
    %cstring_output_allocate_size(char **s,int *slen,free(*$1));
    void tostring(char **s,int *slen) {
        *slen = $self->total * sizeof *$self->data;
        *s = (char*)malloc(*slen);
        memcpy(*s,$self->data,*slen);
    }
#endif
    void fromstring(char *s,int len) {
        $self->resize(len/sizeof *$self->data);
        memcpy($self->data,s,len);
    }
}

////////////////////////////////////////////////////////////////
// rectangles and rectarrays
////////////////////////////////////////////////////////////////

struct rectangle {
    int x0,y0,x1,y1;
    rectangle();
    rectangle(const rectangle &r);
    rectangle(int x0,int y0,int x1,int y1);
    bool empty();
    void pad_by(int dx,int dy);
    void shift_by(int dx,int dy);
    int width();
    int height();
    void include(int x,int y);
    bool contains(int x,int y);
    bool contains(point p);
    void intersect(rectangle other);
    void include(rectangle other);
    rectangle intersection(rectangle other);
    rectangle inclusion(rectangle other);
    rectangle grow(int offset);
    int xcenter();
    int ycenter();
    //void print(FILE *stream=stdout);
    //void println(FILE *stream=stdout);
    int area();
    bool overlaps(const rectangle &other);
    bool includes(int x,int y);
    bool includes(float x,float y);
    bool includes(const rectangle &other);
    rectangle dilated_by(int dx0,int dy0,int dx1,int dy1);
    float aspect();
    float centricity(const rectangle &other);
    float fraction_covered_by(const rectangle &other);
};

%pythoncode
%{
def rectangle_tuple(self):
    return (self.x0,self.y0,self.x1,self.y1)

import new
rectangle.tuple = new.instancemethod(rectangle_tuple,None,rectangle)
%}

// We need to special-case rectarray because rectangles dont
// support the same operations as numbers.

class rectarray {
public:
    rectarray();
    rectarray(int);
    rectarray(int,int);
    rectarray(int,int,int);
    rectarray(int,int,int,int);

    int length();
    int rank();
    int dim(int);
    void clear();
    void resize(int,int d1=0,int d2=0,int d3=0);
    void reshape(int,int d1=0,int d2=0,int d3=0);

    rectangle at(int);
    rectangle at(int,int);
    rectangle at(int,int,int);
    rectangle at(int,int,int,int);

    void put(int,rectangle);
    void put(int,int,rectangle);
    void put(int,int,int,rectangle);
    void put(int,int,int,int,rectangle);

    void fill(rectangle);
    void copy(rectarray &);
    void append(rectarray &);
    void swap(rectarray &);
    void move(rectarray &);
};

////////////////////////////////////////////////////////////////
// narray-util.h
////////////////////////////////////////////////////////////////

template <class T> T absmax(narray<T> &);
%template(absmax) absmax<float>;
%template(absmax) absmax<int>;
%template(absmax) absmax<byte>;
template <class T> T max(narray<T> &);
%template(max) max<float>;
%template(max) max<int>;
%template(max) max<byte>;
template <class T> T min(narray<T> &);
%template(min) min<float>;
%template(min) min<int>;
%template(min) min<byte>;
template <class T> double sum(narray<T> &);
%template(sum) sum<float>;
%template(sum) sum<int>;
%template(sum) sum<byte>;
template <class T> double product(narray<T> &);
%template(product) product<float>;
%template(product) product<int>;
%template(product) product<byte>;
template <class T> int argmax(narray<T> &);
%template(argmax) argmax<float>;
%template(argmax) argmax<int>;
%template(argmax) argmax<byte>;
template <class T> int argmin(narray<T> &);
%template(argmin) argmin<float>;
%template(argmin) argmin<int>;
%template(argmin) argmin<byte>;
void make_unit_vector(floatarray &,int ,int );
template <class T> void randomly_permute(narray<T> &);
%template(randomly_permute) randomly_permute<float>;
%template(randomly_permute) randomly_permute<int>;
%template(randomly_permute) randomly_permute<byte>;
double dist2(floatarray &,floatarray &);
double norm2(floatarray &);
void normalize2(floatarray &);
double norm1(floatarray &);
void normalize1(floatarray &);
void fill_uniform(floatarray &,float ,float );
void perturb(floatarray &,float );
template <class T> void iota(narray<T> &,int );
%template(iota) iota<float>;
%template(iota) iota<int>;
%template(iota) iota<byte>;
template <class T> void reverse(narray<T> &);
%template(reverse) reverse<float>;
%template(reverse) reverse<int>;
%template(reverse) reverse<byte>;
template <class T> void reverse(narray<T> &,narray<T> &);
%template(reverse) reverse<float>;
%template(reverse) reverse<int>;
%template(reverse) reverse<byte>;
template <class T> void remove_left(narray<T> &,int);
%template(remove_left) remove_left<float>;
%template(remove_left) remove_left<int>;
%template(remove_left) remove_left<byte>;
template <class T>  void addscaled(narray<T> &,narray<T> &,double );
%template(addscaled) addscaled<float>;
%template(addscaled) addscaled<int>;
%template(addscaled) addscaled<byte>;
template <class T> T ext(narray<T> &,int );
%template(ext) ext<float>;
%template(ext) ext<int>;
%template(ext) ext<byte>;
template <class T> T ext(narray<T> &,int,int );
%template(ext) ext<float>;
%template(ext) ext<int>;
%template(ext) ext<byte>;
template <class T,class U> T bat(narray<T> &, int, U);
%template(bat) bat<float,float>;
%template(bat) bat<int,int>;
%template(bat) bat<byte,byte>;
template <class T,class U> T bat(narray<T> &, int ,int, U);
%template(bat) bat<float,float>;
%template(bat) bat<int,int>;
%template(bat) bat<byte,byte>;
template <class T> void remove_element(narray<T> &,int);
%template(remove_element) remove_element<float>;
%template(remove_element) remove_element<int>;
%template(remove_element) remove_element<byte>;
template <class T,class S> void remove_value(narray<T> &,S );
%template(remove_value) remove_value<int,int>;
/*%template(remove_value) remove_value<float,float>;
%template(remove_value) remove_value<byte,byte>;*/ //these two are not defined
template <class T,class S> int first_index_of(narray<T> &,S );
%template(first_index_of) first_index_of<float,float>;
%template(first_index_of) first_index_of<int,int>;
%template(first_index_of) first_index_of<byte,byte>;
template <class T> static void insert_at(narray<T> &,int );
%template(insert_at) insert_at<float>;
%template(insert_at) insert_at<int>;
%template(insert_at) insert_at<byte>;
template <class T> static void delete_at(narray<T> &,int );
%template(delete_at) delete_at<float>;
%template(delete_at) delete_at<int>;
%template(delete_at) delete_at<byte>;
template <class T> static void insert_at(narray<T> &,int,T );
%template(insert_at) insert_at<float>;
%template(insert_at) insert_at<int>;
%template(insert_at) insert_at<byte>;
template <class T> T clamp(T ,T ,T );
%template(clamp) clamp<float>;
%template(clamp) clamp<int>;
%template(clamp) clamp<byte>;
template <class T,class U> void clampscale(narray<T> &,narray<U> &,T ,T );
%template(clampscale) clampscale<float,float>;
%template(clampscale) clampscale<int,int>;
%template(clampscale) clampscale<byte,byte>;
template<class T,class U> bool contains_only(narray<T> &, U);
%template(contains_only) contains_only<float,float>;
%template(contains_only) contains_only<int,int>;
%template(contains_only) contains_only<byte,byte>;
template<class T,class U,class V> bool contains_only(narray<T> &, U,V);
%template(contains_only) contains_only<float,float,float>;
%template(contains_only) contains_only<int,int,int>;
%template(contains_only) contains_only<byte,byte,byte>;
template<class T> void get_dims(intarray &, narray<T> &);
%template(get_dims) get_dims<float>;
%template(get_dims) get_dims<int>;
%template(get_dims) get_dims<byte>;
template<class T> void set_dims(narray<T> &, intarray &);
%template(set_dims) set_dims<float>;
%template(set_dims) set_dims<int>;
%template(set_dims) set_dims<byte>;

////////////////////////////////////////////////////////////////
// narray-ops.h
////////////////////////////////////////////////////////////////

// arithmetic

template <class T,class S> void add(narray<T> &,S);
template <class T,class S> void add(narray<T> &,narray<S> &);
%template(add) add<float,float>;
%template(add) add<int,int>;
%template(add) add<byte,byte>;

template <class T,class S> void sub(narray<T> &,S );
template <class T,class S> void sub(S ,narray<T> &);
template <class T,class S> void sub(narray<T> &,narray<S> &);
template <class T,class S> void sub(narray<T> &,narray<S> &,narray<S> &);
%template(sub) sub<float,float>;
%template(sub) sub<int,int>;
%template(sub) sub<byte,byte>;

template <class T,class S> void mul(narray<T> &,S);
template <class T,class S> void mul(narray<T> &,narray<S> &);
template <class T,class S> void mul(narray<T> &,narray<S> &,narray<S> &);
%template(mul) mul<float,float>;
%template(mul) mul<int,int>;
%template(mul) mul<byte,byte>;

template <class T,class S> void div(narray<T> &,S);
template <class T,class S> void div(S,narray<T> &);
template <class T,class S> void div(narray<T> &,narray<S> &);
template <class T,class S> void div(narray<T> &,narray<S> &,narray<S> &);
%template(div) div<float,float>;
%template(div) div<int,int>;
%template(div) div<byte,byte>;

template <class T,class S> void mod(narray<T> &,S);
template <class T,class S> void mod(S,narray<T> &);
template <class T,class S> void mod(narray<T> &,narray<S> &);
template <class T,class S> void mod(narray<T> &,narray<S> &,narray<S> &);
%template(mod) mod<int,int>;
%template(mod) mod<byte,byte>;

template <class T,class S> void pow(narray<T> &,S);
template <class T,class S> void pow(narray<T> &,narray<S> &);
%template(pow) pow<float,float>;
%template(pow) pow<int,int>;
%template(pow) pow<byte,byte>;

// bit operations

template <class T,class S> void bits_and(narray<T> &,S);
template <class T,class S> void bits_and(narray<T> &,narray<S> &);
%template(bits_and) bits_and<byte,byte>;
%template(bits_and) bits_and<int,int>;

template <class T,class S> void bits_or(narray<T> &,S);
template <class T,class S> void bits_or(narray<T> &,narray<S> &);
%template(bits_or) bits_or<byte,byte>;
%template(bits_or) bits_or<int,int>;

template <class T,class S> void bits_xor(narray<T> &,S);
template <class T,class S> void bits_xor(narray<T> &,narray<S> &);
%template(bits_xor) bits_xor<byte,byte>;
%template(bits_xor) bits_xor<int,int>;

template <class T,class S> void bits_lshift(narray<T> &,S);
template <class T,class S> void bits_lshift(narray<T> &,narray<S> &);
%template(bits_lshift) bits_lshift<byte,byte>;
%template(bits_lshift) bits_lshift<int,int>;

template <class T,class S> void bits_rshift(narray<T> &,S);
template <class T,class S> void bits_rshift(narray<T> &,narray<S> &);
%template(bits_rshift) bits_rshift<byte,int>;
%template(bits_rshift) bits_rshift<int,int>;

// min/max

template <class T,class S> void max(narray<T> &,S );
template <class T,class S> void max(narray<T> &out,narray<S> &);
%template(max) max<float,float>;
%template(max) max<int,int>;
%template(max) max<byte,byte>;

template <class T,class S> void min(narray<T> &,S );
template <class T,class S> void min(narray<T> &out,narray<S> &);
%template(min) min<float,float>;
%template(min) min<int,int>;
%template(min) min<byte,byte>;

#if 0
%template <class T,class S,class R> void greater(narray<T> &, S ,R ,R );
%template(greater) greater<float,float,float>;
%template(greater) greater<int,int,int>;
%template(greater) greater<byte,byte,byte>;

template <class T,class S,class R> void less(narray<T> &, S ,R ,R );
%template(less) less<float,float,float>;
%template(less) less<int,int,int>;
%template(less) less<byte,byte,byte>;
#endif

// more arithmetic operations

template <class T> void neg(narray<T> &);
%template(neg) neg<float>;
%template(neg) neg<int>;
%template(neg) neg<byte>;

template <class T> void abs(narray<T> &);
%template(abs) abs<float>;
%template(abs) abs<int>;
%template(abs) abs<byte>;

template <class T> void log(narray<T> &);
%template(log) log<float>;
%template(log) log<int>;
%template(log) log<byte>;

template <class T> void exp(narray<T> &);
%template(exp) exp<float>;
%template(exp) exp<int>;
%template(exp) exp<byte>;

template <class T> void sin(narray<T> &);
%template(sin) sin<float>;
%template(sin) sin<int>;
%template(sin) sin<byte>;

template <class T> void cos(narray<T> &);
%template(cos) cos<float>;
%template(cos) cos<int>;
%template(cos) cos<byte>;

////////////////////////////////////////////////////////////////
// nustring.h
////////////////////////////////////////////////////////////////

struct nuchar {
    nuchar();
    nuchar(const nuchar &c);
    explicit nuchar(int x);
    int ord();
    bool operator==(const nuchar &c);
    bool operator!=(const nuchar &c);
    bool operator>(const nuchar &c);
    bool operator<(const nuchar &c);
};


class nustring {
public:
    nustring();
    nustring(int n);
    nustring(const char *s);
    bool operator==(nustring &s);
    bool operator==(const char *s);
    bool operator!=(nustring &s);
    bool operator!=(const char *s);
//%rename(of_intarray) of;
//    void of(narray<int> &data);
//%rename(as_intarray) as;
//    void as(narray<int> &data);
    int utf8Length();
%apply (char *STRING, int LENGTH) { (char *str, int len) };
%rename(utf8Decode) of_utf8;
    void utf8Decode(char *str,int len);
%rename(utf8Encode) as_utf8;
    void utf8Encode(char *str,int len);
//%newobject mallocUtf8Encode;
//%rename(mallocUtf8Encode) as_utf8;
//    char *mallocUtf8Encode();
    int length();
};

%extend nustring {
    int ord(int i) {
        return $self->at(i).ord();
    }
    void put(int i,int c) {
        $self->at(i) = nuchar(c);
    }
    void append(int c) {
        $self->push(nuchar(c));
    }
    char * as_string()
    {
        char *result = (char *)malloc($self->utf8Length()+1);
        for(int i=0;i<$self->utf8Length();i++) result[i] = $self->at(i).ord();
        result[$self->utf8Length()] = 0;
        return result;
    }
};

// iustring

class strg {
public:
    strg();
    strg(int);
    strg(const char *);
    void assign(const char *);
    const char *c_str();
    int length();
    const char &at(int);
};

class ustrg {
public:
    ustrg();
    ustrg(int);
    ustrg(const char *);
    int length();
    void clear();
    void assign(const char *s);
    const int &at(int);
};

%extend ustrg {
    void put(int i,int value) {
        $self->at(i) = nuchar(value);
    }
    int ord(int i) {
        return $self->at(i).ord();
    }
    char *as_string() {
        char *result = (char *)malloc($self->utf8Length()+1);
        for(int i=0;i<$self->utf8Length();i++) result[i] = $self->at(i).ord();
        result[$self->utf8Length()] = 0;
        return result;
    }
    void as_intarray(intarray &a) {
        a.clear();
        for(int i=0;i<$self->length();i++)
            a.push() = $self->at(i).ord();
    }
}

%{
#include "unicodeobject.h"
%}

%inline
%{
    void ustrg_of_pyunicode(ustrg &a,PyObject *object_) {
        if(!object_) throw "null pointer";
        if(!PyUnicode_Check(object_)) throw "expected a unicode object";
        Py_UNICODE* unicodestr = PyUnicode_AS_UNICODE(object_);
        int length = PyUnicode_GetSize(object_);
        //a.clear();
        for(int i=0;i<length;i++) a.push_back(unicodestr[i]);
    }

    PyObject * pyunicode_of_ustrg(PyObject *object_, ustrg &a) {
        if(!object_) throw "null pointer";
        if(!PyUnicode_Check(object_)) throw "expected a unicode object";
        Py_UNICODE unicodestr[a.length()+1];

        for(int i=0;i<a.length();i++)
        {
            int value = a.at(i).ord();
            if(value>=0 && value<0x100000) unicodestr[i] = value;
            //printf("%d",unicodestr[i]);
        }
        object_ = PyUnicode_FromUnicode(unicodestr, a.length());
        return object_;
    }
%}

%pythoncode
%{
def unicode2ustrg(a):
    result = ustrg()
    ustrg_of_pyunicode(result,a)
    return result
def ustrg2unicode(a):
    result = unicode()
    result = pyunicode_of_ustrg(result,a)
    return result
%}


////////////////////////////////////////////////////////////////
// imgio.h
////////////////////////////////////////////////////////////////

void read_image_packed(intarray &,const char *path);
void read_image_rgb(bytearray &, const char *path);
void read_image_gray(bytearray &, const char *path);
void read_image_binary(bytearray &, const char *path);
void write_image_packed(const char *path,intarray &);
void write_image_rgb(const char *path, bytearray &);
void write_image_gray(const char *path,bytearray &);
void write_image_binary(const char *path,bytearray &);

struct Tiff {
    Tiff(const char* filename, const char* mode);
    Tiff(FILE* file, const char* mode);
    ~Tiff();
    int numPages();
    void getPageRaw(bytearray &image, int page, bool gray=true);
    void getPageRaw(intarray &image, int page, bool gray=false);
    void getPage(bytearray &image, int page, bool gray=true);
    void getPage(intarray &image, int page, bool gray=false);
    void setPage(bytearray &image, int page);
    void setPage(intarray &image, int page);
};


////////////////////////////////////////////////////////////////
// imgedges.h
////////////////////////////////////////////////////////////////

void rawedges(floatarray &edges, floatarray &smoothed);
float nonzero_fractile(floatarray &edges, float frac, int nbins=1000);
void nonmaxsup(bytearray &out, floatarray &gradm, floatarray &gradx, floatarray &grady);
void hysteresis_thresholding(floatarray &image, float lo, float hi);
void gradients(floatarray &gradm,floatarray &gradx,floatarray &grady,floatarray &smoothed);
void canny(floatarray &gradm,floatarray &image,float sx,float sy,
           float frac=0.3,float tlow=2.0,float thigh=4.0);

////////////////////////////////////////////////////////////////
// imgbrushfire.h
////////////////////////////////////////////////////////////////

void brushfire_2(floatarray &distance, float maxdist=1e30);
/// Same as brushfire_2(), but with sum-abs (1-norm) metric.
void brushfire_1(floatarray &distance, float maxdist=1e30);
/// Same as brushfire_2(), but with max-abs (infinity-norm) metric.
void brushfire_inf(floatarray &distance, float maxdist=1e30);

/// Perform distance transform, return the closest point coordinates as a WxHx2 array,
/// where the closest cloud point for the (x,y) pixel has ((x,y,0), (x,y,1)) coordinates.
void brushfire_2(floatarray &distance, intarray &source, float maxdist=1e30);
void brushfire_1(floatarray &distance, intarray &source, float maxdist=1e30);
void brushfire_inf(floatarray &distance, intarray &source, float maxdist=1e30);

/// Dilate with a diamond (metric figure of 1-norm). Uses distance transform.
void dilate_1(floatarray &image, float r);

/// Dilate with a circle (metric figure of 2-norm).    Uses distance transform.
void dilate_2(floatarray &image, float r);

/// Dilate with a square (metric figure of infinity-norm).    Uses distance transform.
void dilate_inf(floatarray &image, float r);

/// Erode with a diamond (metric figure of 1-norm).    Uses distance transform.
void erode_1(floatarray &image, float r);

/// Erode with a circle (metric figure of 2-norm).    Uses distance transform.
void erode_2(floatarray &image, float r);

/// Erode with a square (metric figure of infinity-norm).   Uses distance transform.
void erode_inf(floatarray &image, float r);
void brushfire_inf_scaled(floatarray &distance, float sx, float sy,float maxdist=1e38);
void brushfire_2_scaled(floatarray &distance, float a, float b, float c,float d, float maxdist=1e38);


////////////////////////////////////////////////////////////////
// imgfilters.h
////////////////////////////////////////////////////////////////

void horn_riley_ridges(floatarray &im,floatarray &zero,floatarray &strength,floatarray &angle);
void plus_laplacian(floatarray &result,floatarray &image);
void zero_crossings(bytearray &result,floatarray &image);
void local_minima(bytearray &result,floatarray &image);
void local_maxima(bytearray &result,floatarray &image);
void gradient_based_corners(floatarray &image);
void kitchen_rosenfeld_corners(floatarray &corners,floatarray &image);
void kitchen_rosenfeld_corners2(floatarray &corners,floatarray &image);
void median_filter(bytearray &image, int rx, int ry);


////////////////////////////////////////////////////////////////
// imggauss.h
////////////////////////////////////////////////////////////////

template<class T> void gauss1d(narray<T> &out, narray<T> &in, float sigma);
%template(gauss1d) gauss1d<float>;
%template(gauss1d) gauss1d<byte>;
template<class T> void gauss1d(narray<T> &v, float sigma);
%template(gauss1d) gauss1d<float>;
%template(gauss1d) gauss1d<byte>;
template<class T> void gauss2d(narray<T> &a, float sx, float sy);
%template(gauss2d) gauss2d<float>;
%template(gauss2d) gauss2d<byte>;

////////////////////////////////////////////////////////////////
// imggraymorph.h
////////////////////////////////////////////////////////////////

void complement(bytearray &image);
void difference(bytearray &image, bytearray &image2, int dx, int dy);
int maxdifference(bytearray &image, bytearray &image2, int cx, int cy);
void minshift(bytearray &image, bytearray &image2, int dx, int dy, byte offset=0);
void maxshift(bytearray &image, bytearray &image2, int dx, int dy, byte offset=0);
void gray_erode(bytearray &image, bytearray &mask, int cx, int cy);
void gray_dilate(bytearray &image, bytearray &mask, int cx, int cy);
void gray_open(bytearray &image, bytearray &mask, int cx, int cy);
void gray_close(bytearray &image, bytearray &mask, int cx, int cy);

////////////////////////////////////////////////////////////////
// imglabels.h
////////////////////////////////////////////////////////////////

//%include <iulib/imglabels.h>
void propagate_labels(intarray &image);
void propagate_labels_to(intarray &target,intarray &seed);
void remove_dontcares(intarray &image);
int renumber_labels(intarray &image,int start);
int label_components(intarray &image,bool four_connected=false);
void simple_recolor(intarray &image);
void bounding_boxes(rectarray &result,intarray &image);
int interesting_colors(int x);

////////////////////////////////////////////////////////////////
// imgmap.h
////////////////////////////////////////////////////////////////

template<class T> void rotate_direct_sample(narray<T> &out, narray<T> &in, float angle, float cx, float cy);
%template(rotate_direct_sample) rotate_direct_sample<float>;
%template(rotate_direct_sample) rotate_direct_sample<int>;
%template(rotate_direct_sample) rotate_direct_sample<byte>;

template<class T> void rotate_direct_interpolate(narray<T> &out,narray<T> &in, float angle, float cx, float cy);
%template(rotate_direct_interpolate) rotate_direct_interpolate<float>;
%template(rotate_direct_interpolate) rotate_direct_interpolate<int>;
%template(rotate_direct_interpolate) rotate_direct_interpolate<byte>;

template<class T> void scale_sample(narray<T> &out, narray<T> &in,float sx, float sy);
template<class T> void scale_sample(narray<T> &out,narray<T> &in, int nx,int ny);
%template(scale_sample) scale_sample<float>;
%template(scale_sample) scale_sample<int>;
%template(scale_sample) scale_sample<byte>;

template<class T> void scale_interpolate(narray<T> &out, narray<T> &in,float sx, float sy);
template<class T> void scale_interpolate(narray<T> &out, narray<T> &in,int nx, int ny);
%template(scale_interpolate) scale_interpolate<float>;
%template(scale_interpolate) scale_interpolate<int>;
%template(scale_interpolate) scale_interpolate<byte>;

////////////////////////////////////////////////////////////////
// imgmorph.h
////////////////////////////////////////////////////////////////

void make_binary(bytearray &image);
void check_binary(bytearray &image);
void binary_invert(bytearray &image);
void binary_autoinvert(bytearray &image);
void complement(bytearray &image);
void difference(bytearray &image, bytearray &image2, int dx, int dy);
int maxdifference(bytearray &image, bytearray &image2, int cx=0, int cy=0);
void binary_and(bytearray &image, bytearray &image2, int dx=0, int dy=0);
void binary_or(bytearray &image, bytearray &image2, int dx=0, int dy=0);
void binary_erode_circle(bytearray &image, int r);
void binary_dilate_circle(bytearray &image, int r);
void binary_open_circle(bytearray &image, int r);
void binary_close_circle(bytearray &image, int r);
void binary_erode_rect(bytearray &image, int rw, int rh);
void binary_dilate_rect(bytearray &image, int rw, int rh);
void binary_open_rect(bytearray &image, int rw, int rh);
void binary_close_rect(bytearray &image, int rw, int rh);

////////////////////////////////////////////////////////////////
// imgops.h
////////////////////////////////////////////////////////////////

//%include <iulib/imgops.h>
template<class T, class S> void getd0(narray<T> &image, narray<S> &slice, int index);
%template(getd0) getd0<float,float>;
%template(getd0) getd0<float,byte>;
%template(getd0) getd0<byte,float>;
%template(getd0) getd0<byte,byte>;

template<class T, class S> void getd1(narray<T> &image, narray<S> &slice, int index);
%template(getd1) getd1<float,float>;
%template(getd1) getd1<float,byte>;
%template(getd1) getd1<byte,float>;
%template(getd1) getd1<byte,byte>;

template<class T, class S> void putd0(narray<T> &image, narray<S> &slice, int index);
%template(putd0) putd0<float,float>;
%template(putd0) putd0<float,byte>;
%template(putd0) putd0<byte,float>;
%template(putd0) putd0<byte,byte>;

template<class T, class S> void putd1(narray<T> &image, narray<S> &slice, int index);
%template(putd1) putd1<float,float>;
%template(putd1) putd1<float,byte>;
%template(putd1) putd1<byte,float>;
%template(putd1) putd1<byte,byte>;

float gradx(floatarray &image, int x, int y);
float grady(floatarray &image, int x, int y);
float gradmag(floatarray &image, int x, int y);
float gradang(floatarray &image, int x, int y);

//template<class T>  T &xref(narray<T> &a, int x, int y);
template<class T>  T xref(narray<T> &a, int x, int y);
%template(xref) xref<float>;
%template(xref) xref<int>;
%template(xref) xref<byte>;

template<class T>  T bilin(narray<T> &a, float x, float y);
%template(bilin) bilin<float>;
%template(bilin) bilin<int>;
%template(bilin) bilin<byte>;

template<class T, class V> void addscaled(narray<T> &, narray<T> &, V, int, int);
%template(addscaled) addscaled<float,float>;
%template(addscaled) addscaled<int,float>;
%template(addscaled) addscaled<byte,float>;

template<class T> void tighten(narray<T> &image);
%template(tighten) tighten<float>;
%template(tighten) tighten<int>;
%template(tighten) tighten<byte>;

template<class T> void circ_by(narray<T> &image, int dx, int dy, T value=0);
%template(circ_by) circ_by<float>;
%template(circ_by) circ_by<int>;
%template(circ_by) circ_by<byte>;

template<class T> void shift_by(narray<T> &image, int dx, int dy, T value=0);
%template(shift_by) shift_by<float>;
%template(shift_by) shift_by<int>;
%template(shift_by) shift_by<byte>;

template<class T> void pad_by(narray<T> &image, int px, int py, T value=0);
%template(pad_by) pad_by<float>;
%template(pad_by) pad_by<int>;
%template(pad_by) pad_by<byte>;

template<class T> void erase_boundary(narray<T> &, int, int, T);
%template(erase_boundary) erase_boundary<float>;
%template(erase_boundary) erase_boundary<int>;
%template(erase_boundary) erase_boundary<byte>;

template<class T, class S> void extract_subimage(narray<T> &subimage,narray<S> &image, int x0, int y0, int x1, int y1);
%template(extract_subimage) extract_subimage<float,float>;
%template(extract_subimage) extract_subimage<int,int>;
%template(extract_subimage) extract_subimage<byte,byte>;
%template(extract_subimage) extract_subimage<byte,float>;
%template(extract_subimage) extract_subimage<byte,int>;
%template(extract_subimage) extract_subimage<int,byte>;
%template(extract_subimage) extract_subimage<int,float>;
%template(extract_subimage) extract_subimage<float,int>;
%template(extract_subimage) extract_subimage<float,byte>;

template<class T, class S,class U> void extract_bat(narray<T> &subimage,narray<S> &image, int x0, int y0, int x1, int y1,U dflt);
%template(extract_bat) extract_bat<float,float,float>;
%template(extract_bat) extract_bat<int,int,int>;
%template(extract_bat) extract_bat<byte,byte,int>;
%template(extract_bat) extract_bat<byte,float,float>;
%template(extract_bat) extract_bat<byte,int,int>;
%template(extract_bat) extract_bat<int,byte,int>;
%template(extract_bat) extract_bat<int,float,float>;
%template(extract_bat) extract_bat<float,int,int>;
%template(extract_bat) extract_bat<float,byte,int>;

template<class T> void resize_to(narray<T> &image, int w, int h, T value=0);
%template(resize_to) resize_to<float>;
%template(resize_to) resize_to<int>;
%template(resize_to) resize_to<byte>;

void compose_at(bytearray &image, bytearray &source, int x, int y,int value, int conflict);

template<class T, class U, class V, class W> void ifelse(narray<T> &dest,narray<U> &cond, narray<V> &iftrue, narray<W> &iffalse);
%template(ifelse) ifelse<float,float,float,float>;
%template(ifelse) ifelse<float,byte,float,float>;
%template(ifelse) ifelse<byte,byte,byte,byte>;

void blend(floatarray &dest, floatarray &cond, floatarray &iftrue,floatarray &iffalse);
template<class T> void linearly_transform_intensity(narray<T> &image,float m, float b, float lo, float hi);
%template(linearly_transform_intensity) linearly_transform_intensity<float>;
%template(linearly_transform_intensity) linearly_transform_intensity<int>;
%template(linearly_transform_intensity) linearly_transform_intensity<byte>;

template<class T> void gamma_transform(narray<T> &image, float gamma,float c, float lo, float hi);
%template(gamma_transform) gamma_transform<float>;
%template(gamma_transform) gamma_transform<int>;
%template(gamma_transform) gamma_transform<byte>;

template<class T> void expand_range(narray<T> &image, float lo, float hi);
%template(expand_range) expand_range<float>;
%template(expand_range) expand_range<int>;
%template(expand_range) expand_range<byte>;

////////////////////////////////////////////////////////////////
// imgmisc.h
////////////////////////////////////////////////////////////////

void valleys(intarray &locations,floatarray &v, int minsize=0,int maxsize=1<<30,float sigma=0.0);
void peaks(intarray &locations, floatarray &v, int minsize=0, int maxsize=1<<30, float sigma=0.0);
void hist(floatarray &hist, bytearray &image);
template <class T> void split_rgb(narray<T> &r,narray<T> &g,narray<T> &b,narray<T> &rgb);
%template(split_rgb) split_rgb<byte>;
%template(split_rgb) split_rgb<int>;
%template(split_rgb) split_rgb<float>;
template <class T> void combine_rgb(narray<T> &rgb,narray<T> &r,narray<T> &g,narray<T> &b);
%template(combine_rgb) combine_rgb<byte>;
%template(combine_rgb) combine_rgb<int>;
%template(combine_rgb) combine_rgb<float>;
void unpack_rgb(bytearray &r,bytearray &g,bytearray &b,intarray &rgb);
void pack_rgb(intarray &rgb,bytearray &r,bytearray &g,bytearray &b);
template<class T> void fill_rect(narray<T> &out,int x0,int y0,int x1,int y1,T value);
template<class T> void fill_rect(narray<T> &out,rectangle r,T value);
%template(fill_rect) fill_rect<byte>;
%template(fill_rect) fill_rect<int>;
%template(fill_rect) fill_rect<float>;
template<class T> void math2raster(narray<T> &out, narray<T> &in);
template<class T> void raster2math(narray<T> &out, narray<T> &in);
template<class T> void math2raster(narray<T> &out);
template<class T> void raster2math(narray<T> &out);
%template(math2raster) math2raster<byte>;
%template(math2raster) math2raster<int>;
%template(math2raster) math2raster<float>;
%template(raster2math) raster2math<byte>;
%template(raster2math) raster2math<int>;
%template(raster2math) raster2math<float>;
template<class T> void crop(narray<T> &result, narray<T> &source,int x, int y, int w, int h);
template<class T> void crop(narray<T> &result, narray<T> &source, rectangle r) ;
template<class T> void crop(narray<T> &a, rectangle box) ;
%template(crop) crop<byte>;
%template(crop) crop<int>;
%template(crop) crop<float>;
template<class T> void transpose(narray<T> &a) ;
%template(transpose) transpose<byte>;
%template(transpose) transpose<int>;
%template(transpose) transpose<float>;
template<class T> void replace_values(narray<T> &a, T from, T to) ;
%template(replace_values) replace_values<byte>;
%template(replace_values) replace_values<int>;
%template(replace_values) replace_values<float>;
void binarize_by_threshold(bytearray &image);

////////////////////////////////////////////////////////////////
// imgrescale.h
////////////////////////////////////////////////////////////////
//%include <iulib/imgrescale.h>
void rough_rescale(floatarray &dst, const floatarray &src, int w, int h);
void bicubic_rescale(floatarray &dst, const floatarray &src, int w, int h);
void rescale(floatarray &dst, const floatarray &src, int w, int h);
void rescale(bytearray &dst, const bytearray &src, int w, int h);
void rescale_to_width(floatarray &dst, const floatarray &src, int w);
void rescale_to_width(bytearray &dst, const bytearray &src, int w);
void rescale_to_height(floatarray &dst, const floatarray &src, int h);
void rescale_to_height(bytearray &dst, const bytearray &src, int h);


////////////////////////////////////////////////////////////////
// imgthin.h
////////////////////////////////////////////////////////////////
//%include <iulib/imgthin.h>
void thin(bytearray &uci);


////////////////////////////////////////////////////////////////
// imgtrace.h
////////////////////////////////////////////////////////////////
//%include <iulib/imgtrace.h>

struct IChainTracer {
        virtual void set_image(bytearray &image) = 0;
        virtual void clear() = 0;
        virtual bool get_chain(floatarray &points, bool close=false,int sample=1) = 0;
        virtual bool get_poly(floatarray &points, float maxdist=1.0,bool close=false) = 0;
        virtual ~IChainTracer();
};
%newobject chaintracer_;
%inline
%{
    IChainTracer *chaintracer_(bytearray &image) {
        IChainTracer *result = chaintracer(image);
        return result;
    }
%}
IChainTracer * chaintracer(bytearray &image);


////////////////////////////////////////////////////////////////
// imgbits.h
////////////////////////////////////////////////////////////////
//%include <iulib/imgbits.h>

typedef unsigned int word32;

struct BitImage {
void init();
void clear();
BitImage();
BitImage(int w,int h);
~BitImage();
double megabytes();
void copy(BitImage &other);
int rank();
int dim(int i);
void resize(int w,int h);
word32 *get_line(int i);
bool at(int i,int j);
bool operator()(int i,int j);
void set_bit(int i,int j);
void clear_bit(int i,int j);
void set(int i,int j,bool value);
void fill(bool value);
};

struct IBlit1D {
virtual void blit1d(word32 *dest,int enddestbits,
                    word32 *mask,int endmaskbits,
                    int shift,BlitOp op) = 0;
virtual ~IBlit1D() {}
};

struct IBlit2D {
virtual void blit2d(BitImage &image,BitImage &other,int dx,int dy,
                    BlitOp op,BlitBoundary bop) = 0;
virtual IBlit1D &blit1d() = 0;
virtual int getBlitCount() = 0;
virtual ~IBlit2D() {}
};
IBlit1D *make_Blit1DBitwise();
IBlit1D *make_Blit1DWordwise();
IBlit2D *make_Blit2D(IBlit1D *);

IBlit1D *make_Blit1DBitwiseC();
IBlit1D *make_Blit1DWordwiseC();

void bits_move(BitImage &dest,BitImage &src);
void bits_convert(BitImage &bimage,bytearray &image);
void bits_convert(BitImage &bimage,floatarray &image);
void bits_convert(bytearray &image,BitImage &bimage);
void bits_convert(floatarray &image,BitImage &bimage);
int bits_count_rect(BitImage &image,int x0=0,int y0=0,int x1=32000,int y1=32000);
bool bits_non_empty(BitImage &image);
void bits_set_rect(BitImage &image,int x0=0,int y0=0,int x1=32000,int y1=32000,bool value=false);
void bits_resample_normed(bytearray &image,BitImage &bits,int vis_scale,bool norm=true);
void bits_resample(bytearray &image,BitImage &bits,int vis_scale);
void bits_reduce2_and(BitImage &out,BitImage &image);
void bits_transpose(BitImage &out,BitImage &in);
void bits_transpose(BitImage &image);
void bits_flip_v(BitImage &image);
void bits_flip_h(BitImage &image);
void bits_rotate_rect(BitImage &image,int angle);

void bits_set(BitImage &image,BitImage &other,int dx=0,int dy=0);
void bits_setnot(BitImage &image,BitImage &other,int dx=0,int dy=0);
void bits_and(BitImage &image,BitImage &other,int dx=0,int dy=0);
void bits_or(BitImage &image,BitImage &other,int dx=0,int dy=0);
void bits_andnot(BitImage &image,BitImage &other,int dx=0,int dy=0);
void bits_ornot(BitImage &image,BitImage &other,int dx=0,int dy=0);
void bits_xor(BitImage &image,BitImage &other,int dx=0,int dy=0);
void bits_invert(BitImage &image);

void bits_skew(BitImage &image,float skew,float center=0.0,bool backwards=false);
void bits_rotate(BitImage &image,float angle);

void bits_erode_rect(BitImage &image,int rx,int ry);
void bits_dilate_rect(BitImage &image,int rx,int ry);
void bits_open_rect(BitImage &image,int rx,int ry);
void bits_close_rect(BitImage &image,int rx,int ry);

void bits_erode_mask(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
void bits_dilate_mask(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
void bits_open_mask(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
void bits_close_mask(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
void bits_mask_hitmiss(BitImage &image,BitImage &element,int cx=DFLTC,int cy=DFLTC);
void bits_mask_hitmiss(BitImage &image,BitImage &hit,BitImage &miss,int cx=DFLTC,int cy=DFLTC);

void bits_circ_mask(BitImage &image,int r);
void bits_erode_circ(BitImage &image,int r);
void bits_dilate_circ(BitImage &image,int r);
void bits_open_circ(BitImage &image,int r);
void bits_close_circ(BitImage &image,int r);

void bits_erode_rrect(BitImage &image,int w,int h,double angle);
void bits_dilate_rrect(BitImage &image,int w,int h,double angle);
void bits_open_rrect(BitImage &image,int w,int h,double angle);
void bits_close_rrect(BitImage &image,int w,int h,double angle);

void bits_erode_line(BitImage &image,int r,double angle,int rp);
void bits_dilate_line(BitImage &image,int r,double angle,int rp);
void bits_open_line(BitImage &image,int r,double angle,int rp);
void bits_close_line(BitImage &image,int r,double angle,int rp);

////////////////////////////////////////////////////////////////
// imgrle.h
////////////////////////////////////////////////////////////////

//%include <iulib/imgrle.h>

struct RLERun {
short start,end;
RLERun();
RLERun(short start,short end);
RLERun operator+(int offset) ;
void operator+=(int offset) ;
bool contains(const RLERun other);
bool overlaps(const RLERun &other);
bool before(const RLERun &other);
bool after(const RLERun &other);
};

typedef narray<RLERun> RLELine;

inline void verify_line(RLELine &line,int l=(1<<30));
inline void trim_line(RLELine &line,int l);

// A run-length-encoded image.
struct RLEImage {
bool doesNotAlias(RLEImage &other);
int dim(int d);
RLELine &line(int i);
int nlines();
void take(RLEImage &in);
bool equals(RLEImage &b);
int number_of_runs();
double megabytes();
void resize(int d0,int d1,int prealloc=0) ;
void fill(bool value);
void copy(RLEImage &other);
int at(int x,int y);
void put(int x,int y,bool p);
void verify();
};

// various image processing functions
void rle_convert(RLEImage &out,bytearray &in);
void rle_convert(bytearray &out,RLEImage &in);
void rle_convert(RLEImage &out,BitImage &in);
void rle_convert(BitImage &out,RLEImage &in);

int rle_count_bits(RLEImage &image);
int rle_count_bits(RLEImage &image,int x0,int y0,int x1,int y1);
void rle_runlength_statistics(floatarray &on,floatarray &off,RLEImage &image);
void rle_peak_estimation(intarray &h0,intarray &h1,intarray &v0,intarray &v1,RLEImage &image,float sh=3.0,float sv=3.0);

void rle_shift(RLEImage &image,int d0,int d1);
void rle_transpose(RLEImage &out,RLEImage &in);
void rle_transpose(RLEImage &image);
void rle_rotate_rect(RLEImage &image,int angle);
void rle_skew(RLEImage &image,float skew,float center);
void rle_rotate(RLEImage &image,float angle);
void rle_flip_v(RLEImage &image);


void rle_invert(RLEImage &image);
void rle_and(RLEImage &image,RLEImage &mask,int d0,int d1);
void rle_or(RLEImage &image,RLEImage &mask,int d0,int d1);

void rle_dilate_rect(RLEImage &image,int r0,int r1);
void rle_erode_rect(RLEImage &image,int r0,int r1);
void rle_open_rect(RLEImage &image,int r0,int r1);
void rle_close_rect(RLEImage &image,int r0,int r1);

void rle_circular_mask(RLEImage &image,int r);
void rle_erode_mask(RLEImage &image,RLEImage &mask,int r0,int r1);

int rle_bounding_boxes(rectarray &boxes,RLEImage &image);
void rle_read(RLEImage &,const char *);
void rle_write(const char *,RLEImage &);
void rle_dshow(RLEImage &image,const char *spec);
void rle_debug(RLEImage &image);

////////////////////////////////////////////////////////////////
// dgraphics.h -This is disabled in iulib.
////////////////////////////////////////////////////////////////
void dinit(int w,int h,bool force=false);
bool dactive();
void dclear(int );
//template <class T> void dshown(narray<T> &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshown(floatarray &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshown(intarray &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshown(bytearray &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
//template <class T> void dshow(narray<T> &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshow(floatarray &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshow(bytearray &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshow(intarray &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshow_signed(floatarray &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshow_grid_signed(floatarray &data,int tw,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshowr(intarray &data,const char *spec="",double angle=90,int smooth=1,int rgb=0);
void dshow1d(floatarray &data,const char *spec="");
//void dline(int x0,int y0,int x1,int y1,int color,int w,int h,const char *spec=""); Need to check not working
void dflush();
void dwait();
void dclose();
bool dactivate(bool flag);
const char *dsection_set(const char *section);
void dgraphics_set(const char *s);

%inline
%{
    int return99() {
        return 99;
    }
    const char *return_hello() {
        return "hello";
    }
    float plus1(float x) {
        return x+1;
    }
%}

