#!/bin/bash

valgrind \
    --vgdb-error=0 \
    --vgdb=yes \
    --tool=memcheck \
    --suppressions=.valgrind-profile.supp \
    .build/lsp-plugins-profile impulse_reverb_stereo

