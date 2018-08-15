#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/util/Convolver.h>

#ifndef CONVOLVER_RANK_SMALLEST
    #define UNDEF_CONVOLVER_RANK_SMALLEST
    #define CONVOLVER_RANK_SMALLEST CONVOLVER_RANK_FFT_SMALL
#endif

#define TEST_BUF_SIZE   1024

namespace convolver_test
{
    using namespace lsp;

    void perform_test(size_t conv_len, size_t rank)
    {
        // Initialize convolution
        float *convolution  = new float[conv_len];
        dsp::fill_zero(convolution, conv_len);
        for (size_t i=0; i<(conv_len>>(CONVOLVER_RANK_SMALLEST-1)); i++)
            convolution[i<<(CONVOLVER_RANK_SMALLEST-1)]    = i + 1;

        printf("Convolution:\n  ");
        test::dump_data(convolution, conv_len);

        // Initialize convolver
        Convolver c;
        c.init(convolution, conv_len, rank, 0.0f);

        // Prepare input data
        float *in       = new float[TEST_BUF_SIZE];
        float *out      = new float[TEST_BUF_SIZE];
        dsp::fill_zero(in, TEST_BUF_SIZE);
        in[0]           = 1.0f;
        in[2]           = 1.0f;
        in[9]           = 1.0f;
        in[15]          = -1.0f;
        if (conv_len < TEST_BUF_SIZE)
            in[TEST_BUF_SIZE - conv_len] = 1.0f;

        // Dump data
        printf("Input data:\n  ");
        test::dump_data(in, TEST_BUF_SIZE);

        c.process(out, in, TEST_BUF_SIZE);

        printf("Output data:\n  ");
        test::dump_data(out, TEST_BUF_SIZE);

        // Drop all data
        c.destroy();
        delete [] in;
        delete [] out;
        delete [] convolution;
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        perform_test(240, 7);

        return 0;
    }
    
}

#ifdef UNDEF_CONVOLVER_RANK_SMALLEST
    #undef CONVOLVER_RANK_SMALLEST
    #undef UNDEF_CONVOLVER_RANK_SMALLEST
#endif

#undef TEST_BUF_SIZE
