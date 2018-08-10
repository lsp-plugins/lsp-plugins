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
        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
        void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
        void fd_packed_direct_fft(float *dst, const float *src, size_t rank);
    }

    namespace sse
    {
        void fastconv_parse(float *dst, const float *src, size_t rank);
        void fastconv_parse_apply(float *dst, float *tmp, const float *c, const float *src, size_t rank);
        void fastconv_apply(float *dst, float *tmp, const float *c1, const float *c2, size_t rank);
    }
}

namespace fft_fastconv_test
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

    void dump_buffer(const char *s, const float *p, size_t count, size_t stride)
    {
        printf("%s", s);
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
        size_t sig_len      = 16;
        size_t sig_rank     = int_log2(sig_len)+1;

        float *sig          = new float[sig_len];
        float *conv         = new float[sig_len];
        float *ff_conv      = new float[sig_len*4];
        float *ff_tmp       = new float[sig_len*4];
        float *ff_sig       = new float[sig_len*4];
        float *out          = new float[sig_len*2];

        // Initialize signal
        dsp::fill_zero(sig, sig_len);
        dsp::copy(sig, sig_re, (sig_len > sg_len) ? sg_len : sig_len);
        dump_buffer("Signal:        ", sig, sig_len, 1);

        // Pre-calculate spectrum
        dsp::fill_zero(ff_conv, sig_len*4);
        dsp::packed_real_to_complex(ff_conv, sig, sig_len);
        native::fd_packed_direct_fft(ff_tmp, ff_conv, sig_rank);
        dump_buffer("Spectrum [RE]: ", ff_tmp, sig_len*2, 2);
        dump_buffer("Spectrum [IM]: ", &ff_tmp[1], sig_len*2, 2);

        native::packed_reverse_fft(ff_tmp, ff_tmp, sig_rank);
        dump_buffer("Signal [RE]:   ", ff_tmp, sig_len*2, 2);
        dump_buffer("Signal [IM]:   ", &ff_tmp[1], sig_len*2, 2);

        // Initialize convolution
        dsp::fill_zero(conv, sig_len);
        conv[0]             = 1.0f;
        conv[sig_len-1]     = 1.0f;
        dump_buffer("Convolution:   ", conv, sig_len, 1);

        sse::fastconv_parse(ff_conv, conv, sig_rank);
        dump_buffer("Parsed:        ", ff_conv, sig_len*4, 1);

        dsp::fill_zero(out, sig_len*2);
        sse::fastconv_parse_apply(out, ff_tmp, ff_conv, sig, sig_rank);
        dump_buffer("Temp:          ", ff_tmp, sig_len*4, 1);
        dump_buffer("Applied:       ", out, sig_len*2, 1);

        sse::fastconv_parse(ff_sig, sig, sig_rank);
        dump_buffer("Parsed:        ", ff_sig, sig_len*4, 1);

        dsp::fill_zero(out, sig_len*2);
        sse::fastconv_apply(out, ff_tmp, ff_sig, ff_conv, sig_rank);
        dump_buffer("Temp:          ", ff_tmp, sig_len*4, 1);
        dump_buffer("Applied:       ", out, sig_len*2, 1);

        // Drop data
        delete [] out;
        delete [] ff_sig;
        delete [] ff_conv;
        delete [] ff_tmp;
        delete [] conv;
        delete [] sig;

        return 0;
    }
    
}
