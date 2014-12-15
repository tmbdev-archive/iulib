// -*- C++ -*-

#ifndef h5eigen__
#define h5eigen__

#include <memory>
#include <string>
#include <map>
#include "H5Cpp.h"
#include "narray.h"

namespace h5eigen {
using namespace H5;
using namespace std;
using namespace colib;

struct HDF5 {
    shared_ptr<H5File> h5;
    void open(const char *name,bool rw=false) {
        if(rw) {
            h5.reset(new H5File(name,H5F_ACC_TRUNC));
        } else {
            h5.reset(new H5File(name,H5F_ACC_RDONLY));
        }
    }
    ~HDF5() {
        h5->close();
    }
    H5::PredType pred_type(int) { return PredType::NATIVE_INT; }
    H5::PredType pred_type(float) { return PredType::NATIVE_FLOAT; }
    H5::PredType pred_type(double) { return PredType::NATIVE_DOUBLE; }
    template <class T>
    void get(T &a,const char *name) {
        DataSet dataset = h5->openDataSet(name);
        DataSpace space = dataset.getSpace();
        hsize_t offset[] = {0,0,0,0,0,0};
        hsize_t count[] = {0,0,0,0,0,0};
        int rank = space.getSimpleExtentDims(count);
        if(rank==1) a.resize(count[0]);
        else if(rank==2) a.resize(count[0],count[1]);
        else if(rank==3) a.resize(count[0],count[1],count[2]);
        else if(rank==4) a.resize(count[0],count[1],count[2],count[4]);
        else throw "unsupported rank";
        space.selectHyperslab(H5S_SELECT_SET, count, offset);
        DataSpace mem(rank, count);
        mem.selectHyperslab(H5S_SELECT_SET, count, offset);
        dataset.read(&a(0,0),pred_type(a(0,0)), mem, space);
    }
    template <class T>
    void put(T &a,const char *name) {
        int rank = a.rank();
        DSetCreatPropList plist; // setFillValue, etc.
        hsize_t dim[] = {rows,cols,0,0,0,0};
        for(int i=0;i<rank;i++) dim[i] = a.dim(i);
        DataSpace fspace(rank,dim);
        DataSet dataset = h5->createDataSet(name, pred_type(a[0]), fspace, plist);
        hsize_t start[] = {0,0,0,0,0,0};
        hsize_t count[] = {0,0,0,0,0,0};
        for(int i=0;i<rank;i++) count[i] = a.dim(i);
        DataSpace mspace(rank,dim);
        fspace.selectHyperslab(H5S_SELECT_SET, count, start);
        mspace.selectHyperslab(H5S_SELECT_SET, count, start);
        dataset.write(&a[0],pred_type(a[0]), mspace, fspace);
    }
};
}

#endif
