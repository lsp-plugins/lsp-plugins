#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>

namespace root_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        dsp::init();

        for (int i=-8; i <= 8; ++i)
        {
            float x = dsp::ipowf(2.0f, i);
            lsp_trace("2^%d = %f", i, x);
        }

        float root = dsp::irootf(M_PI, 3);
        lsp_trace("Root(3) of PI = %f, the power is %f, the PI is %f", root, root*root*root, M_PI);

        root = dsp::irootf(M_PI, 4);
        lsp_trace("Root(4) of PI = %f, the power is %f, the PI is %f", root, root*root*root*root, M_PI);

        root = dsp::irootf(M_PI, 5);
        lsp_trace("Root(5) of PI = %f, the power is %f, the PI is %f", root, root*root*root*root*root, M_PI);

        return 0;
    }
    
}
