#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/util/Crossover.h>

namespace crossover_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        dsp::init();

        Crossover cv;
        cv.init(8, 4);

        return 0;
    }
    
}
