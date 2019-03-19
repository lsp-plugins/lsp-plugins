#!/bin/bash

. scripts/bash/set_vars.sh

export THREADS=$((THREADS/2))
export ARCH=aarch64

echo "******************************************************"
echo "  Building profile=$ARCH"
echo "******************************************************"

$MAKE clean && $MAKE BUILD_PROFILE=$ARCH -j$THREADS all

