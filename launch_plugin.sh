#!/bin/bash

if [ $# -eq 0 ]; then
    echo "Usage: $0 <plugin_name> (like para_equalizer_x32_stereo)"
    exit 1
fi

.build/host/lsp-plugin-fw/lsp-plugins-test mtest standalone --args $1 --external-resources
