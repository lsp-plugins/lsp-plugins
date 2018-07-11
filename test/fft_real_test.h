#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>

namespace lsp
{
    namespace native
    {
        void conv_direct_fft(float *dst, const float *src, size_t rank);
    }

    namespace sse
    {
        void conv_direct_fft(float *dst, const float *src, size_t rank);
    }
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

    void do_fft(float *rex, float *imx, size_t n)
    {
        size_t nh = n/2 - 1;
        for (size_t i=0; i<=nh; ++i)
        {
            rex[i]  = rex[2*i];
            imx[i]  = imx[2*i+1];
        }

        n   /= 2;

        // CALL FFT for rex and imx
        dsp::direct_fft(rex, imx, rex, imx, int_log2(n));
        n   *= 2;

        size_t nm1  = n - 1;
        size_t nd2  = n/2;
        size_t n4   = n/4 - 1;
        for (size_t i=1; i<n4; ++i)
        {
            size_t im   = nd2 - i;
            size_t ip2  = i + nd2;
            size_t ipm  = im + nd2;
            rex[ip2]    = (imx[i] + imx[im])/2;
            rex[ipm]    = rex[ip2];
            imx[ip2]    = -(rex[i]-rex[im])/2;
            imx[ipm]    = -imx[ip2];
            rex[i]      = (rex[i] + rex[im])/2;
            rex[im]     = rex[i];
            imx[i]      = (imx[i] - imx[im])/2;
            imx[im]     = -imx[i];
        }

        rex[n*3/4]  = imx[n/4];
        rex[nd2]    = imx[0];
        imx[n*3/4]  = 0;
        imx[nd2]    = 0;
        imx[n/4]    = 0;
        imx[0]      = 0;
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
