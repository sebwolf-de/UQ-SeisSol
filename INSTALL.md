# Install the UQ-Wrapper on the LRZ linux cluster

## Prerequisites
I like to build all my libraries in `~/opt` but this is personal preference. 
If you build in a different place, replace the path.

## Build MUQ
Download muq library: http://muq.mit.edu/ and extract to `~/opt/muq2`
MUQ depends on Eigen3, HDF5 and Boost and some other libraries. MUQ can build all these libraries itself, but it takes a lot of time.
HDF5 and boost are alredy installed on the linux cluster and can be loaded as modules.

```bash
module load intel/19.0.5
module load intel-mpi/2019.8.254
module load netcdf-hdf5-all/4.6_hdf5-1.8-intel19-impi
module load boost/1.61.0-intel19-impi

CC=mpiicc CXX=mpiicpc cmake .. -DCMAKE_INSTALL_PREFIX=~/opt 
make -j20
make install
```

## Build UQ-SeisSol

Go to your UQ-SeisSol main directory.

```bash
mkdir build
cd build
CC=mpiicc CXX=mpiicpc cmake -DMUQ_DIR=~/opt/CMake/MUQ ..
make -j20
```
