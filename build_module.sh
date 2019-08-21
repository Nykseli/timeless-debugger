#!/bin/bash -e

# Build the udis86 object files
cd udis86
mkdir -p builddir
./autogen.sh
./configure --prefix=`pwd`/builddir
make -j4
# No need for install since we only want the object files
# make install

# Copy the object files from udis module
cd ..
mkdir -p build/udis86
cp -v udis86/libudis86/.libs/*.o build/udis86/
