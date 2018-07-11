#!/bin/bash

export THREADS=16

do_release() {
    export CPU_ARCH=$1
    export LD_ARCH=$2
    export CC_ARCH=$3
    
    echo "******************************************************"
    echo "Releasing arch=$CPU_ARCH ld=$LD_ARCH cc=$CC_ARCH";
    echo "******************************************************"

    make clean && make -j$THREADS all && make release;
}

echo "******************************************************"
echo "Building RELEASE"
echo "******************************************************"

do_release 'i586' '-m elf_i386' '-m32';
do_release 'x86_64' '-m elf_x86_64' '-m64';

