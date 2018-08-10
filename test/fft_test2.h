#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>

namespace lsp
{
    namespace native
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void packed_direct_fft(float *dst, const float *src, size_t rank);
        void packed_reverse_fft(float *dst, const float *src, size_t rank);
    }

    namespace sse
    {
        void direct_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void reverse_fft(float *dst_re, float *dst_im, const float *src_re, const float *src_im, size_t rank);
        void packed_direct_fft(float *dst, const float *src, size_t rank);
        void packed_reverse_fft(float *dst, const float *src, size_t rank);
    }
}

namespace fft_test2
{
    using namespace lsp;

    const float sig_re[] =
    {
        1.0,
        1.1,
        1.2,
        1.3,
        1.4,
        1.5,
        1.6,
        1.7
    };

    const float sig_im[] =
    {
        -1.0,
        -0.75,
        -0.5,
        -0.25,
        0.0,
        0.25,
        0.5,
        0.75
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

    void init_signal(float *re, float *im, size_t stride)
    {
        for (size_t i=0; i<sizeof(sig_re)/sizeof(float); ++i)
        {
            *re     = sig_re[i];
            re     += stride;
        }
        for (size_t i=0; i<sizeof(sig_im)/sizeof(float); ++i)
        {
            *im     = sig_im[i];
            im     += stride;
        }
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t sig_len      = 32;
        float *sig          = new float[sig_len * 2];
        float *spc          = new float[sig_len * 2];

        size_t sig_rank     = int_log2(sig_len);

        // Test FFT
        printf("\nTesting FFT\n");
        dsp::fill_zero(sig, sig_len*2);
        init_signal(sig, &sig[sig_len], 1);

        printf("Signal   [RE]: ");
        dump_buffer(sig, sig_len, 1);
        printf("Signal   [IM]: ");
        dump_buffer(&sig[sig_len], sig_len, 1);

        native::direct_fft(spc, &spc[sig_len], sig, &sig[sig_len], sig_rank);

        printf("Spectrum [RE]: ");
        dump_buffer(spc, sig_len, 1);
        printf("Spectrum [IM]: ");
        dump_buffer(&spc[sig_len], sig_len, 1);

        native::reverse_fft(sig, &sig[sig_len], spc, &spc[sig_len], sig_rank);

        printf("Signal   [RE]: ");
        dump_buffer(sig, sig_len, 1);
        printf("Signal   [IM]: ");
        dump_buffer(&sig[sig_len], sig_len, 1);

        // Test Packed Native FFT
        printf("\nTesting Packed Native FFT\n");
        dsp::fill_zero(sig, sig_len*2);
        init_signal(sig, &sig[1], 2);

        printf("Signal   [RE]: ");
        dump_buffer(sig, sig_len, 2);
        printf("Signal   [IM]: ");
        dump_buffer(&sig[1], sig_len, 2);

        native::packed_direct_fft(spc, sig, sig_rank);

        printf("Spectrum [RE]: ");
        dump_buffer(spc, sig_len, 2);
        printf("Spectrum [IM]: ");
        dump_buffer(&spc[1], sig_len, 2);

        native::packed_reverse_fft(sig, spc, sig_rank);

        printf("Signal   [RE]: ");
        dump_buffer(sig, sig_len, 2);
        printf("Signal   [IM]: ");
        dump_buffer(&sig[1], sig_len, 2);

        // Test Packed SSE FFT
        printf("\nTesting Packed SSE FFT\n");
        dsp::fill_zero(sig, sig_len*2);
        init_signal(sig, &sig[1], 2);

        printf("Signal   [RE]: ");
        dump_buffer(sig, sig_len, 2);
        printf("Signal   [IM]: ");
        dump_buffer(&sig[1], sig_len, 2);

        sse::packed_direct_fft(spc, sig, sig_rank);

        printf("Spectrum [RE]: ");
        dump_buffer(spc, sig_len, 2);
        printf("Spectrum [IM]: ");
        dump_buffer(&spc[1], sig_len, 2);

        sse::packed_reverse_fft(sig, spc, sig_rank);

        printf("Signal   [RE]: ");
        dump_buffer(sig, sig_len, 2);
        printf("Signal   [IM]: ");
        dump_buffer(&sig[1], sig_len, 2);

        // Drop data
        delete [] spc;
        delete [] sig;

        return 0;
    }
    
}
