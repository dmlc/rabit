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

if [ ${TASK} == "xgb-cmake" ]; then
    git clone --recursive https://github.com/chenqin/xgboost ../xgboost
    rm -rf ../xgboost/rabit
    ln -s ../rabit ../xgboost/rabit
    cd ../xgboost
    set -e
    # Build gtest via cmake
    wget -nc https://github.com/google/googletest/archive/release-1.7.0.zip
    unzip -n release-1.7.0.zip
    mv googletest-release-1.7.0 gtest && cd gtest
    cmake . && make
    mkdir lib && mv libgtest.a lib
    cd ..
    rm -rf release-1.7.0.zip

    # Build/test
    rm -rf build
    mkdir build && cd build
    PLUGINS="-DPLUGIN_LZ4=ON -DPLUGIN_DENSE_PARSER=ON"
    cmake .. -DGOOGLE_TEST=ON -DGTEST_ROOT=$PWD/../gtest/ ${PLUGINS}
    make
    ./testxgboost
    cd ..
    rm -rf build
fi

if [ ${TASK} == "xgb-java-tests" ]; then
    git clone --recursive https://github.com/chenqin/xgboost ../xgboost
    rm -rf ../xgboost/rabit
    ln -s ../rabit ../xgboost/rabit
    echo "MAVEN_OPTS='-Xmx2g -XX:MaxPermSize=1024m -XX:ReservedCodeCacheSize=512m -Dorg.slf4j.simpleLogger.defaultLogLevel=error'" > ~/.mavenrc
    cd ../xgboost
    set -e
    cd jvm-packages
    mvn -q clean install -DskipTests -Dmaven.test.skip
    mvn -q test || exit -1
fi
