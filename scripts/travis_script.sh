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

if [ ${TASK} == "xgb-java-tests" ]; then
    git clone --recursive https://github.com/chenqin/xgboost ../xgboost
    rm -rf ../xgboost/rabit
    ln -s ../rabit ../xgboost/rabit
    cd ../xgboost
    source tests/travis/setup.sh

    set -e
    cd jvm-packages
    mvn -q clean install -DskipTests -Dmaven.test.skip
    mvn -q test || exit -1
fi
