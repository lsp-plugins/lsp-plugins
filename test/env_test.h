#include <stdlib.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stddef.h>

#include <core/types.h>
#include <dsp/dsp.h>
#include <core/windows.h>
#include <core/envelope.h>

#define BUF_ITEMS 2400

namespace env_test
{
    using namespace lsp;

    int test(int argc, const char **argv)
    {
        dsp::init();

        float *buf      = NULL;
        float *envelopes[envelope::TOTAL];

        size_t count    = BUF_ITEMS * envelope::TOTAL;
        buf             = new float[count];
        if (buf == NULL)
            return -1;

        float *ptr      = buf;
        for (size_t i=0; i<envelope::TOTAL; ++i)
        {
            envelope::noise(ptr, BUF_ITEMS, envelope::envelope_t(i + envelope::FIRST));

            envelopes[i]    = ptr;
            ptr            += BUF_ITEMS;
        }

        // Print header
        printf("Index;Frequency;");
        for (size_t i=0; i<envelope::TOTAL; ++i)
            printf("%s;%s(dB);", envelope::envelopes[i], envelope::envelopes[i]);
        printf("\n");

        // Print items
        float kf = float(SPEC_FREQ_MAX) / float(BUF_ITEMS);
        for (size_t i=0; i<BUF_ITEMS; ++i)
        {
            printf("%d;%.3f;", int(i), i *  kf);
            for (size_t j=0; j< envelope::TOTAL; ++j)
                printf("%.7f;%.2f;", envelopes[j][i], 20 * log10(envelopes[j][i]));
            printf("\n");
        }

        delete [] buf;

        return 0;
    }
    
}

#undef BUF_ITEMS

