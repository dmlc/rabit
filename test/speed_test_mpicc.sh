#!/bin/sh

make clean
make speed_test.mpicc
mpiexec -np 2 ./speed_test.mpicc 2 2

