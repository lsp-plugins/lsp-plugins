#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>

namespace native
{
    void packed_direct_fft(float *dst, const float *src, size_t rank);
    void fd_packed_direct_fft(float *dst, const float *src, size_t rank);
}

namespace fft_tdomain_test
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

        size_t sg_len       = sizeof(sig_re)/sizeof(float);
        size_t sig_len      = 8;
        float *sig          = new float[sig_len * 2];
        float *fft          = new float[sig_len * 2];

        size_t sig_rank     = int_log2(sig_len);

        dsp::fill_zero(sig, sig_len * 2);
        if (sg_len > (sig_len * 2))
            sg_len = sig_len * 2;

        dsp::copy(sig, sig_re, sg_len);

        printf("Signal     [RE]: ");
        dump_buffer(sig, sig_len, 2);
        printf("Signal     [IM]: ");
        dump_buffer(&sig[1], sig_len, 2);

        native::packed_direct_fft(fft, sig, sig_rank);

        printf("\nTD FFT     [RE]: ");
        dump_buffer(fft, sig_len, 2);
        printf("TD FFT     [IM]: ");
        dump_buffer(&fft[1], sig_len, 2);

        native::fd_packed_direct_fft(fft, sig, sig_rank);

        printf("\nFD FFT     [RE]: ");
        dump_buffer(fft, sig_len, 2);
        printf("FD FFT     [IM]: ");
        dump_buffer(&fft[1], sig_len, 2);

        // Drop data
        delete [] sig;
        delete [] fft;

        return 0;
    }
    
}
