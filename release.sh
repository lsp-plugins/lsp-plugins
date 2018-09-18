#!/bin/bash

export THREADS=16

do_release() {
    export BUILD_PROFILE=$1
    export CC_FLAGS=-DLSP_NO_EXPERIMENTAL
    
    echo "******************************************************"
    echo "  Building profile=$BUILD_PROFILE"
    echo "******************************************************"

    make clean && make -j$THREADS all && make release;
    make clean && make -j$THREADS profile && make release_profile;
}

echo "******************************************************"
echo "Building RELEASE"
echo "******************************************************"

ARCH=`uname -m`

make unrelease
if [[ "$ARCH" =~ ^x86_64 ]]; then
	do_release 'i586'
	do_release 'x86_64'
elif [[ "$ARCH" =~ ^i[3-6]86 ]]; then
	do_release 'i586'
elif [[ "$ARCH" =~ ^armv7.* ]]; then
	do_release 'armv7a'
fi

