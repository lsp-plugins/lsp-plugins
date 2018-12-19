#!/bin/bash

. scripts/bash/set_vars.sh

export THREADS=$((THREADS/2))
export ARCH=armv7a

echo "******************************************************"
echo "  Building profile=$ARCH"
echo "******************************************************"

$MAKE clean && $MAKE BUILD_PROFILE=$ARCH -j$THREADS all

