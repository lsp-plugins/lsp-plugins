/*
 * equalizer.cpp
 *
 *  Created on: 24 авг. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/filters/Equalizer.h>

#define MIN_RANK 5
#define MAX_RANK 12

using namespace dsp;
using namespace lsp;

//-----------------------------------------------------------------------------
// Performance test for equalizer module
PTEST_BEGIN("core.filters", equalizer, 5, 1000)

    void call(float *out, const float *in, size_t slope, size_t count)
    {
        char buf[80];
        sprintf(buf, "slope=%d, samples=%d", int(slope), int(count));
        printf("Testing equalizer of 16 channels, %s ...\n", buf);

        filter_params_t lopass, hipass;
        lopass.fFreq    = 1000;
        lopass.fGain    = 1.0f;
        lopass.fQuality = 0.707;
        lopass.nSlope   = slope;
        lopass.nType    = FLT_BT_BWC_LOPASS;
        hipass          = lopass;
        hipass.nType    = FLT_BT_BWC_HIPASS;

        Equalizer eq;
        eq.init(16, 13);
        eq.set_sample_rate(48000);
        eq.set_mode(EQM_IIR);

        for (size_t i=0; i<16; i+=2)
        {
            eq.set_params(i*2, &lopass);
            eq.set_params(i*2+1, &lopass);
        }

        PTEST_LOOP(buf,
                eq.process(out, in, count);
        );

        eq.destroy();
    }

    PTEST_MAIN
    {
        size_t buf_size = 1 << MAX_RANK;
        uint8_t *data   = NULL;
        float *in       = alloc_aligned<float>(data, buf_size *6, 64);
        float *out      = &in[buf_size];

        for (size_t i=0; i < (1 << (MAX_RANK + 1)); ++i)
            in[i]           = float(rand()) / RAND_MAX;

        for (size_t i=MIN_RANK; i <= MAX_RANK; ++i)
        {
            size_t count = 1 << i;

            call(out, in, 2, count);
            call(out, in, 4, count);
            call(out, in, 6, count);
            call(out, in, 8, count);

            PTEST_SEPARATOR;
            printf("\n");
        }

        free_aligned(data);
    }
PTEST_END




