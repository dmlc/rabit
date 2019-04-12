#!/bin/bash

# main script of travis
if [ ${TASK} == "lint" ]; then
    make lint || exit -1
fi

if [ ${TASK} == "doc" ]; then
    make doc 2>log.txt
    (cat log.txt| grep -v ENABLE_PREPROCESSING |grep -v "unsupported tag" |grep warning) && exit -1
fi

if [ ${TASK} == "build" ]; then
    make all || exit -1
fi

if [ ${TASK} == "mpi-build" ]; then
    if [ -f mpich/lib/libmpich.so ]; then
        echo "libmpich.so found -- nothing to build."
    else
        echo "Downloading mpich source."
        wget http://www.mpich.org/static/downloads/3.2/mpich-3.2.tar.gz
        tar xfz mpich-3.2.tar.gz
        rm mpich-3.2.tar.gz*
        echo "configuring and building mpich."
        cd mpich-3.2
        #CC=gcc CXX=g++ CFLAGS=-m64 CXXFLAGS=-m64 FFLAGS=-m64
        ./configure \
                --prefix=`pwd`/../mpich \
                --enable-static=false \
                --enable-alloca=true \
                --disable-long-double \
                --enable-threads=single \
                --enable-fortran=no \
                --enable-fast=all \
                --enable-g=none \
                --enable-timing=none \
                --enable-cxx
        make -j4
        make install
        cd -
    fi
    cd test
    make mpi && make speed_test.mpi || exit -1
fi

if [ ${TASK} == "test" ]; then
    cd test
    make all || exit -1
    ../scripts/travis_runtest.sh || exit -1
fi

if [ ${TASK} == "cmake-build" ]; then
    mkdir build
    cd build
    cmake ..
    make all || exit -1
fi