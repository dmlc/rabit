#!/usr/bin/env bash

if [ ${TRAVIS_OS_NAME} == "linux" ]; then
    # sudo apt-get update -q
    sudo apt-get install -y -q libopenmpi-dev openmpi-bin

    export LD_PRELOAD=/usr/lib/openmpi/lib/libmpi.so
fi

if [ ${TRAVIS_OS_NAME} == "osx" ]; then
    curl -OL http://raw.github.com/craigcitro/r-travis/master/scripts/travis-tool.sh
    chmod 755 ./travis-tool.sh
    ./travis-tool.sh bootstrap
    ruby -e "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/master/install)"
    rm -rf /usr/local/include/c++
    rm -rf /usr/local/include/gfortran
    brew install gcc
    brew install open-mpi
fi

mpiexec --version
mpicxx -showme:incdirs
mpicxx -showme:libdirs
mpicxx -showme:compile
mpicxx -showme:link
mpiexec -np 2 hostname
