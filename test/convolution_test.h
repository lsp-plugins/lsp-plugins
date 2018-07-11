#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <core/dsp.h>

namespace convolution_test
{
    using namespace lsp;

    float convolution[] =
    {
        1.0, -1.0, 0.5, -0.5,
        0.25, -0.25, 0.125, -0.125
    };

    const float signal[] =
    {
        1, 1, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    void dump_data(const float *data, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            if (i > 0)
                printf(", ");
            printf("%.3f", data[i]);
        }
        printf("\n");
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        size_t conv_len     = sizeof(convolution) / sizeof(float);
        size_t signal_len   = sizeof(signal) / sizeof(float);
        size_t out_len      = signal_len;
        float *out          = new float[out_len];

        dsp::fill_zero(out, out_len);
        dsp::reverse(convolution, conv_len);

        printf("Input signal: \n");
        dump_data(signal, signal_len);

        printf("Convolution: \n");
        dump_data(convolution, conv_len);

        ShiftBuffer sb;
        sb.resize(conv_len * 4, conv_len);

//        for (size_t i=conv_len; i<signal_len; ++i)
//        {
        const float *src = sb.tail();
        sb.append(signal, signal_len);
        dsp::convolve(out, src - conv_len + 1, convolution, conv_len, out_len);
        sb.shift(NULL, signal_len);
//        }

        printf("Output signal: \n");
        dump_data(out, out_len);
        delete [] out;

        return 0;
    }
    
}
