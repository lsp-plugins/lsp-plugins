#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <core/dsp.h>
#include <core/util/Sidechain.h>

#define SC_BUF_SIZE 1024

namespace sidechain_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        dsp::init();
        Sidechain sc;

        float *out          = new float[SC_BUF_SIZE];
        float *left         = new float[SC_BUF_SIZE];
        float *right        = new float[SC_BUF_SIZE];

        dsp::fill_zero(left, SC_BUF_SIZE);
        dsp::fill_zero(right, SC_BUF_SIZE);
        left[5]     = 1.0f;
        left[6]     = 0.5f;
        left[16]    = -1.0f;

        sc.init(2, 1.0f);
        sc.set_mode(SCM_LPF);
        sc.set_sample_rate(48000);
        sc.set_source(SCS_MIDDLE);
        sc.set_reactivity(0.1f);

        const float *in[2]  = { left, right };
        sc.process(out, in, SC_BUF_SIZE);

        printf("left;right;out;\n");
        for (size_t i=0; i<SC_BUF_SIZE; ++i)
            printf("%f;%f;%f;\n", left[i], right[i], out[i]);

        delete [] left;
        delete [] right;
        delete [] out;

        return 0;
    }

}

#undef SC_BUF_SIZE
