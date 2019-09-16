#!/bin/bash -e

# Build the udis86 object files
cd udis86
mkdir -p builddir
./autogen.sh
./configure --prefix=`pwd`/builddir
make -j`grep processor /proc/cpuinfo | wc -l` # make based on cpu cores
# No need for install since we only want the object files
# make install

# Copy the object files from udis module
cd ..
mkdir -p build/udis86
cp -v udis86/libudis86/.libs/*.o build/udis86/

# Build the rest api
cd c-rest-api
make -j`grep processor /proc/cpuinfo | wc -l` # make based on cpu cores

#Copy the object file from c-rest api
cd ..
mkdir -p build/c-rest-api
cp -v c-rest-api/build/restapi.o build/c-rest-api/
