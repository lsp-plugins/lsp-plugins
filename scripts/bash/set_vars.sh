#!/bin/bash

SYSTEM=`uname -s`
ARCH=`uname -m`
THREADS=1

# Update platform
if [[ "$SYSTEM" =~ BSD ]]; then
    PLATFORM=BSD
    MAKE=gmake
    THREADS=`sysctl -a | grep 'hw.ncpu' | awk  '{print $2}'`
elif [[ "$SYSTEM" =~ Linux ]]; then
    PLATFORM=Linux
    SYSTEM=Linux
    MAKE=make
    THREADS=`grep -c ^processor /proc/cpuinfo`
else
    PLATFORM=Unknown
    SYSTEM=Unknown
    MAKE=make
fi

# Update architecture
if [[ ( "$ARCH" =~ ^x86_64 ) || ( "$ARCH" =~ ^amd64 ) ]]; then
    ARCH=x86_64
elif [[ "$ARCH" =~ ^i[3-6]86 ]]; then
    ARCH=i586
elif [[ "$ARCH" =~ ^armv7.* ]]; then
    ARCH=armv7a
else
    ARCH=native
fi
