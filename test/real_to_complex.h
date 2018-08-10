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
        void packed_real_to_complex(float *dst, const float *src, size_t count);
        void packed_complex_to_real(float *dst, const float *src, size_t count);
    }

    namespace sse
    {
        void packed_real_to_complex(float *dst, const float *src, size_t count);
        void packed_complex_to_real(float *dst, const float *src, size_t count);
    }
}

namespace real_to_complex_test
{
    using namespace lsp;

    void init_real(float *dst, size_t len)
    {
        for (size_t i=0; i<len; ++i)
            dst[i] = i+1;
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t sig_len      = 64 - 1;
        float *cplx         = new float[sig_len * 2];
        float *real         = new float[sig_len];

        // Test FFT
        printf("\nTesting Native Real2Complex\n");
        printf("Real: ");
        init_real(real, sig_len);
        test::dump_data(real, sig_len);
        printf("Complex: ");
        native::packed_real_to_complex(cplx, real, sig_len);
        test::dump_data(cplx, sig_len *2);
        printf("Real: ");
        native::packed_complex_to_real(real, cplx, sig_len);
        test::dump_data(real, sig_len);

        printf("\nTesting SSE Real2Complex\n");
        printf("Real: ");
        init_real(real, sig_len);
        test::dump_data(real, sig_len);
        printf("Complex: ");
        sse::packed_real_to_complex(cplx, real, sig_len);
        test::dump_data(cplx, sig_len *2);
        printf("Real: ");
        sse::packed_complex_to_real(real, cplx, sig_len);
        test::dump_data(real, sig_len);

        printf("\n");

        // Drop data
        delete [] cplx;
        delete [] real;

        return 0;
    }
    
}
