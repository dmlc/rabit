#!/usr/bin/env bash

sudo apt-get update -q
sudo apt-get install -y -q libopenmpi-dev openmpi-bin

export LD_PRELOAD=/usr/lib/openmpi/lib/libmpi.so
mpiexec --version
mpicxx -showme:incdirs
mpicxx -showme:libdirs
mpicxx -showme:compile
mpicxx -showme:link
mpiexec -np 2 hostname
