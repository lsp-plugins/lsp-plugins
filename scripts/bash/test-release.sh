#!/bin/bash

. scripts/bash/set_vars.sh

do_release() {
    export BUILD_PROFILE=$1
    export CC_FLAGS=-DLSP_NO_EXPERIMENTAL

    echo "******************************************************"
    echo "  Building profile=$BUILD_PROFILE, threads=$THREADS"
    echo "******************************************************"

    $MAKE clean && $MAKE -j$THREADS all VERSION=dbg && $MAKE VERSION=dbg dbg_release;
    $MAKE clean && $MAKE -j$THREADS profile VERSION=dbg && $MAKE VERSION=dbg release_profile;
}

echo "******************************************************"
echo "Building RELEASE"
echo "******************************************************"

# Clean previously built release
$MAKE unrelease

# Perform a release
if [[ "$ARCH" == "x86_64" ]]; then
    export THREADS=$((THREADS*2)) # For x86_64 architecture we use twice greater number of threads
    if [[ "$PLATFORM" == "Linux" ]]; then # Cross-compiling for 32-bit architecture
        do_release 'i586'
    fi;
    do_release 'x86_64'
elif [[ "$ARCH" == "i586" ]]; then
    export THREADS # For IA32 architecture we won't volatile number of threads
    do_release 'i586'
elif [[ "$ARCH" == "armv7a" ]]; then
    export THREADS=$((THREADS/2)) # Raspberry may overheat, we use twice lower number of threads
    do_release 'armv7a'
else
    echo "Don't know how to build release"
fi
