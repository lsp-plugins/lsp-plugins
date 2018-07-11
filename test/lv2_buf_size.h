#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/fade.h>
#include <metadata/metadata.h>
#include <plugins/plugins.h>
#include <container/lv2/extensions.h>

namespace lv2bufsize_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        const port_t *ports = multisampler_x12::metadata.ports;

        lsp_trace("input  ports size=%lx", lv2_all_port_sizes(ports, true, false));
        lsp_trace("output ports size=%lx", lv2_all_port_sizes(ports, false, true));

        return 0;
    }
    
}
