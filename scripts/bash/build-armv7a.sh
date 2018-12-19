#!/bin/bash

. scripts/bash/set_vars.sh

export THREADS=$((THREADS/2))

echo "******************************************************"
echo "  Building profile=$BUILD_PROFILE"
echo "******************************************************"

$MAKE clean && $MAKE -j$THREADS all

