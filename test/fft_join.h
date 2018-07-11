/*
 * fft_join.h
 *
 *  Created on: 29 дек. 2016 г.
 *      Author: sadko
 */

#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>

namespace fft_join_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t sig_rank     = 4;
        size_t sig_len      = (1 << sig_rank);
        size_t sig_half     = sig_len >> 1;

        float *sig_re       = new float[sig_len];
        float *sig_im       = new float[sig_len];
        float *spc_re       = new float[sig_len];
        float *spc_im       = new float[sig_len];

        // Test complex conversions
        printf("\nInitial Data\n");
        dsp::fill_zero(sig_re, sig_len);
        dsp::fill_zero(sig_im, sig_len);
        sig_re[0] = 1.0f;
        sig_re[1] = 2.0f;
        sig_re[2] = 3.0f;
        sig_re[3] = 4.0f;
        sig_re[sig_half+0] = -1.0f;
        sig_re[sig_half+1] = -2.0f;
        sig_re[sig_half+2] = -3.0f;
        sig_re[sig_half+3] = -4.0f;

        printf("Signal   [RE]: ");
        test::dump_data(sig_re, sig_len);
        printf("Signal   [IM]: ");
        test::dump_data(sig_im, sig_len);

        // Fourier transform
        printf("\nIssuing Fourier Transform of rank=%d\n", int(sig_rank));
        dsp::direct_fft(spc_re, spc_im, sig_re, sig_im, sig_rank);

        printf("Spectrum [RE]: ");
        test::dump_data(spc_re, sig_len);
        printf("Spectrum [IM]: ");
        test::dump_data(spc_im, sig_len);

        printf("\nIssuing Reverse Fourier Transform of rank=%d\n", int(sig_rank));
        dsp::reverse_fft(spc_re, spc_im, spc_re, spc_im, sig_rank);

        printf("Signal [RE]: ");
        test::dump_data(spc_re, sig_len);
        printf("Signal [IM]: ");
        test::dump_data(spc_im, sig_len);

        // Alternative transform
        printf("\nIssuing 2x Fourier Transform of rank=%d\n", int(sig_rank-1));
        dsp::direct_fft(spc_re, spc_im, sig_re, sig_im, sig_rank-1);
        dsp::direct_fft(&spc_re[sig_half], &spc_im[sig_half], &sig_re[sig_half], &sig_im[sig_half], sig_rank-1);

        printf("Pre-spectrum [RE]: ");
        test::dump_data(spc_re, sig_len);
        printf("Pre-spectrum [IM]: ");
        test::dump_data(spc_im, sig_len);

        // Alternative transform
//        printf("\nIssuing Join of 2x Fourier Transforms of rank=%d\n", int(sig_rank-1));
//        dsp::join_fft(spc_re, spc_im, spc_re, spc_im, sig_rank-1);

        printf("Spectrum [RE]: ");
        test::dump_data(spc_re, sig_len);
        printf("Spectrum [IM]: ");
        test::dump_data(spc_im, sig_len);

        printf("\nIssuing Reverse Fourier Transform of rank=%d\n", int(sig_rank));
        dsp::reverse_fft(spc_re, spc_im, spc_re, spc_im, sig_rank);

        printf("Signal [RE]: ");
        test::dump_data(spc_re, sig_len);
        printf("Signal [IM]: ");
        test::dump_data(spc_im, sig_len);

        delete [] spc_im;
        delete [] spc_re;
        delete [] sig_im;
        delete [] sig_re;

        return 0;
    }
}

