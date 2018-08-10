#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <time.h>
#include <math.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/util/Oversampler.h>

#define RTEST_BUF_SIZE  0x100

namespace oversampler_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        dsp::init();

        // Create oversampler
        Oversampler os;
        os.init();
        os.set_mode(OM_LANCZOS_8X2);
        os.set_sample_rate(48000);
        if (os.modified())
            os.update_settings();

        size_t times        = os.get_oversampling();
        float *out          = new float[RTEST_BUF_SIZE];
        float *in           = new float[RTEST_BUF_SIZE * times];
        float kf            = 0.05f;

        for (size_t i=0; i<RTEST_BUF_SIZE*times; ++i)
        {
            in[i]           = (size_t(i * kf / times) & 1) ? -1.0f : 1.0f;
        }

        printf("Input data: ");
        test::dump_data(in, RTEST_BUF_SIZE*times);

        os.downsample(out, in, RTEST_BUF_SIZE);

        printf("Output data: \n");
//        test::dump_data(out, RTEST_BUF_SIZE);
        for (size_t i=0; i<RTEST_BUF_SIZE; ++i)
            printf("%.3f\n", out[i]);

        delete [] out;
        delete [] in;

        return 0;
    }

}

#undef RTEST_BUF_SIZE
