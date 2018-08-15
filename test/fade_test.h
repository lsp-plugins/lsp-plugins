#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/fade.h>

namespace fade_test
{
    using namespace lsp;

    void dump_data(const float *data, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            if (i > 0)
                printf(", ");
            printf("%.3f", data[i]);
        }
        printf("\n");
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t sig_len      = 64;
        float *sig          = new float[sig_len];

        // Test fade-out
        printf("\nTesting Fade-in and Fade-out\n");

        dsp::fill(sig, 1.0f, sig_len);

        printf("Signal: ");
        dump_data(sig, sig_len);

        fade_out(sig, sig, sig_len >> 1, sig_len);
        printf("Fade-out signal: ");
        dump_data(sig, sig_len);

        fade_in(sig, sig, sig_len >> 1, sig_len);
        printf("Fade-in signal: ");
        dump_data(sig, sig_len);

        delete [] sig;

        return 0;
    }
    
}
