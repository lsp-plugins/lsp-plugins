#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>

namespace fft_filter_test
{
    using namespace lsp;

    const float signal[] =
    {
        1, -1, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    const float signal_im[] =
    {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    const float fft_image[] =
    {
        1, 1, 1, 1,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 1, 1, 1
    };

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t sig_len      = 16;
        float *sig_re       = new float[sig_len];
        float *sig_im       = new float[sig_len];
        float *spc_re       = new float[sig_len];
        float *spc_im       = new float[sig_len];
        size_t sig_rank     = 4;

        dsp::copy(sig_re, signal, sig_len);
        dsp::copy(sig_im, signal_im, sig_len);

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

        dsp::copy(spc_re, fft_image, sig_len);
        dsp::copy(spc_im, signal_im, sig_len);

        printf("Spectrum [RE]: ");
        test::dump_data(spc_re, sig_len);
        printf("Spectrum [IM]: ");
        test::dump_data(spc_im, sig_len);

        dsp::reverse_fft(sig_re, sig_im, spc_re, spc_im, sig_rank);

        printf("Signal   [RE]: \n");
        for (size_t i=0; i<sig_len; ++i)
            printf("%f\n", sig_re[i]);

        delete [] spc_im;
        delete [] spc_re;
        delete [] sig_im;
        delete [] sig_re;

        return 0;
    }
    
}
