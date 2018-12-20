#!/bin/bash

. scripts/bash/set_vars.sh

export ARCH=i586

echo "******************************************************"
echo "  Building profile=$ARCH"
echo "******************************************************"

$MAKE clean && $MAKE BUILD_PROFILE=$ARCH -j$THREADS all
