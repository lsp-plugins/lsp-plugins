#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>

namespace native
{
    void conv_direct_fft(float *dst, const float *src, size_t rank);
}

namespace sse
{
    void conv_direct_fft(float *dst, const float *src, size_t rank);
}

namespace fft_test3
{
    using namespace lsp;

    const float sig_re[] =
    {
        1,
        -2,
        3,
        -4,
        5,
        -6,
        7,
        -8,
        9,
        -10,
        11,
        -12,
        13,
        -14,
        15,
        -16
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

    void dump_buffer(const float *p, size_t count, size_t stride)
    {
        for (size_t i=0; i<count; ++i)
        {
            if (i > 0)
                printf(", ");
            printf("%.3f", *p);
            p += stride;
        }
        printf("\n");
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t sig_len      = 32;
        float *sig          = new float[sig_len];
        float *spc          = new float[sig_len * 2];

        size_t sig_rank     = int_log2(sig_len);


        dsp::fill_zero(sig, sig_len);
        dsp::copy(sig, sig_re, sizeof(sig_re)/sizeof(float));

        printf("Signal   [RE]: ");
        dump_buffer(sig, sig_len, 1);

        // Test Convolution FFT
        printf("\nTesting Native convolution FFT\n");
        native::conv_direct_fft(spc, sig, sig_rank);

        printf("Spectrum [RE]: ");
        dump_buffer(spc, sig_len, 1);
        printf("Spectrum [IM]: ");
        dump_buffer(&spc[sig_len], sig_len, 1);

        dsp::packed_reverse_fft(spc, spc, sig_rank);

        printf("Signal   [RE]: ");
        dump_buffer(spc, sig_len, 2);
        printf("Signal   [IM]: ");
        dump_buffer(&spc[1], sig_len, 2);

        // Test Convolution FFT
        printf("\nTesting SSE convolution FFT\n");
        sse::conv_direct_fft(spc, sig, sig_rank);

        printf("Spectrum [RE]: ");
        dump_buffer(spc, sig_len, 1);
        printf("Spectrum [IM]: ");
        dump_buffer(&spc[sig_len], sig_len, 1);

        dsp::packed_reverse_fft(spc, spc, sig_rank);

        printf("Signal   [RE]: ");
        dump_buffer(spc, sig_len, 2);
        printf("Signal   [IM]: ");
        dump_buffer(&spc[1], sig_len, 2);

        // Drop data
        delete [] spc;
        delete [] sig;

        return 0;
    }
    
}
