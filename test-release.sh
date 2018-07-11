#!/bin/bash

export THREADS=16

do_release() {
    export BUILD_PROFILE=$1
    export CC_FLAGS=-DLSP_NO_EXPERIMENTAL
    
    echo "******************************************************"
    echo "  Building profile=$BUILD_PROFILE"
    echo "******************************************************"

    make clean && make -j$THREADS all VERSION=dbg && make VERSION=dbg dbg_release;
    make clean && make -j$THREADS profile VERSION=dbg && make VERSION=dbg release_profile;
}

echo "******************************************************"
echo "Building RELEASE"
echo "******************************************************"

make unrelease
do_release 'i586'
do_release 'x86_64'
