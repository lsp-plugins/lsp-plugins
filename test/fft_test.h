#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>

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

        size_t sig_len      = 16384;//sizeof(signal) / sizeof(float);
        float *sig_re       = new float[sig_len];
        float *sig_im       = new float[sig_len];
        float *spc_re       = new float[sig_len];
        float *spc_im       = new float[sig_len];
        size_t sig_rank     = int_log2(sig_len);

        // Test complex conversions
        printf("\nTesting CVT\n");
        for (size_t i=0; i<sig_len; ++i)
        {
            float w     = (2 * M_PI * i * 1000) / sig_len;
            spc_re[i]   = 1.0f;
            spc_im[i]   = w;
            sig_re[i]   = cosf(w);
            sig_im[i]   = 0;//sinf(w);
        }

//        printf("Signal   [RE]: ");
//        dump_data(sig_re, sig_len);
//        printf("Signal   [IM]: ");
//        dump_data(sig_im, sig_len);
//        printf("Signal  [MOD]: ");
//        dump_data(spc_re, sig_len);
//        printf("Signal  [ARG]: ");
//        dump_data(spc_im, sig_len);
//
//        dsp::complex_cvt2modarg(sig_re, sig_im, sig_re, sig_im, sig_len);
//        dsp::complex_cvt2reim(spc_re, spc_im, spc_re, spc_im, sig_len);
//
//        printf("Cvt     [MOD]: ");
//        dump_data(sig_re, sig_len);
//        printf("Cvt     [ARG]: ");
//        dump_data(sig_im, sig_len);
//        printf("Cvt      [RE]: ");
//        dump_data(spc_re, sig_len);
//        printf("Cvt      [IM]: ");
//        dump_data(spc_im, sig_len);

        // Test FFT
        printf("\nTesting FFT\n");
//        dsp::copy(sig_re, signal, sig_len);
//        dsp::fill_zero(sig_im, sig_len);
//        dsp::copy(sig_im, signal_im, sig_len);

        printf("Signal   [RE]: ");
        test::dump_data(sig_re, sig_len);
        printf("Signal   [IM]: ");
        test::dump_data(sig_im, sig_len);

        dsp::direct_fft(spc_re, spc_im, sig_re, sig_im, sig_rank);
//        dsp::copy(spc_re, sig_re, sig_len);
//        dsp::copy(spc_im, sig_im, sig_len);
//        dsp::direct_fft(spc_re, spc_im, spc_re, spc_im, sig_rank);

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
