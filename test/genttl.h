#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/fade.h>

namespace lsp
{
    void gen_ttl(const char *path);
}

namespace genttl_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        dsp::init();
        gen_ttl("tmp");
        return 0;
    }
    
}
