#!/bin/bash

export THREADS=2
export BUILD_PROFILE=armv7a
export CC_FLAGS=-DLSP_NO_EXPERIMENTAL

echo "******************************************************"
echo "  Building profile=$BUILD_PROFILE"
echo "******************************************************"

make clean && make -j$THREADS all
