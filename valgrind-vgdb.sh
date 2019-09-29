#!/bin/bash

valgrind \
    --vgdb-error=0 \
    --vgdb=yes \
    --tool=memcheck \
    --leak-check=full \
    --suppressions=.valgrind-profile.supp \
    .test/lsp-plugins-test mtest --debug --nofork standalone --args limiter_stereo

