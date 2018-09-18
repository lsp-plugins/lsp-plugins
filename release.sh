#!/bin/bash

do_release() {
    export BUILD_PROFILE=$1
    export CC_FLAGS=-DLSP_NO_EXPERIMENTAL
    
    echo "******************************************************"
    echo "  Building profile=$BUILD_PROFILE, threads=$THREADS"
    echo "******************************************************"

    make clean && make -j$THREADS all && make release;
    make clean && make -j$THREADS profile && make release_profile;
}

ARCH=`uname -m`
THREADS=`grep -c ^processor /proc/cpuinfo`

echo "******************************************************"
echo "Building RELEASE"
echo "******************************************************"

make unrelease
if [[ "$ARCH" =~ ^x86_64 ]]; then
	export THREADS=$((THREADS*2)) # For x86_64 architecture we use twice greater number of threads
	do_release 'i586'
	do_release 'x86_64'
elif [[ "$ARCH" =~ ^i[3-6]86 ]]; then
	export THREADS # For IA32 architecture we won't volatile number of threads
	do_release 'i586'
elif [[ "$ARCH" =~ ^armv7.* ]]; then
	export THREADS=$((THREADS/2)) # Raspberry may overheat, we use twice lower number of threads
	do_release 'armv7a'
fi

