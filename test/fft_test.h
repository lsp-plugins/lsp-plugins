#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>

namespace fft_test
{
    using namespace lsp;

    const float signal[] =
    {
//        2, 4, 6, 8,
//        2, 4, 6, 8
        1, 2, 3, 4,
        4, 5, 6, 7,
        8, 9, 10, 11,
        12, 13, 14, 15,
        1, -1, 1, -1,
        1, -1, 1, -1,
        1, -1, 1, -1,
        1, -1, 1, -1
//        1, -1, 1, -1, 1, -1, 1, -1,
//        2, 2, 2, 2, 2, 2, 2, 2,
//        1, 2, 3, 4, 5, 4, 3, 2,
//        1, 2, 3, 4, 5, 4, 3, 2,
//        1, -1, 1, -1, 1, -1, 1, -1,
//        2, 2, 2, 2, 2, 2, 2, 2,
//        1, 2, 3, 4, 5, 4, 3, 2,
//        1, 2, 3, 4, 5, 4, 3, 2
    };

    const float signal_im[] =
    {
        0.1, 0.2, 0.3, 0.4,
        0.1, 0.2, 0.3, 0.4,
//        50, 60, 70, 80
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
//        1, -1, 3, 4,
//        3, 3, -1, 1
    };

    inline size_t int_log2(size_t n)
    {
        size_t x = 1, r=0;
        while (n > x)
        {
            x <<= 1;
            r++;
        }
        return r;
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t sig_len      = 32; //65536;//sizeof(signal) / sizeof(float);
        float *sig_re       = new float[sig_len];
        float *sig_im       = new float[sig_len];
        float *spc_re       = new float[sig_len];
        float *spc_im       = new float[sig_len];
        size_t sig_rank     = int_log2(sig_len);

        // Test complex conversions
        printf("\nTesting CVT\n");
        dsp::fill_zero(sig_re, sig_len);
        dsp::fill_zero(sig_im, sig_len);
        sig_re[0] = 1.0f;
        sig_re[1] = 1.1f;
        sig_re[2] = 1.3f;
        sig_re[3] = 1.6f;
        sig_re[4] = 2.0f;
        sig_re[5] = 2.5f;
        sig_re[6] = 3.1f;
        sig_re[7] = 3.8f;

        sig_im[0] = -0.0f;
        sig_im[1] = -0.1f;
        sig_im[2] = -0.3f;
        sig_im[3] = -0.6f;
        sig_im[4] = -0.0f;
        sig_im[5] = -0.5f;
        sig_im[6] = -0.1f;
        sig_im[7] = -0.8f;

        // Test FFT
        printf("\nTesting FFT\n");

        printf("Signal   [RE]: ");
        test::dump_data(sig_re, sig_len);
        printf("Signal   [IM]: ");
        test::dump_data(sig_im, sig_len);

        dsp::direct_fft(spc_re, spc_im, sig_re, sig_im, sig_rank);

        printf("Spectrum [RE]: ");
        test::dump_data(spc_re, sig_len);
        printf("Spectrum [IM]: ");
        test::dump_data(spc_im, sig_len);

        dsp::reverse_fft(sig_re, sig_im, spc_re, spc_im, sig_rank);

        printf("Signal   [RE]: ");
        test::dump_data(sig_re, sig_len);
        printf("Signal   [IM]: ");
        test::dump_data(sig_im, sig_len);

        delete [] spc_im;
        delete [] spc_re;
        delete [] sig_im;
        delete [] sig_re;

        return 0;
    }
    
}
