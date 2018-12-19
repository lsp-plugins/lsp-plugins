#!/bin/bash

. scripts/bash/set_vars.sh

echo "******************************************************"
echo "  Building profile=$BUILD_PROFILE"
echo "******************************************************"

$MAKE clean && $MAKE -j$THREADS all
