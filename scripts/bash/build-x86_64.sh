#!/bin/sh

. scripts/bash/set_vars.sh

export ARCH=x86_64

echo "******************************************************"
echo "  Building profile=$ARCH"
echo "******************************************************"

$MAKE clean && $MAKE BUILD_PROFILE=$ARCH -j$THREADS all
