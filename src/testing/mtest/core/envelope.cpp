/*
 * envelope.cpp
 *
 *  Created on: 24 авг. 2018 г.
 *      Author: sadko
 */


#include <test/mtest.h>

#include <core/types.h>
#include <core/alloc.h>
#include <data/cvector.h>

#include <core/envelope.h>

using namespace lsp;

MTEST_BEGIN("core", envelope)

    MTEST_MAIN
    {
        float *buf      = NULL;
        float *envelopes[envelope::TOTAL];

        size_t points   = 1024;
        if (argc > 0)
            points          = atoi(argv[0]);
        if (points < 10)
            points          = 10;

        buf             = new float[points * envelope::TOTAL];
        MTEST_ASSERT(buf != NULL);

        float *ptr      = buf;
        for (size_t i=0; i<envelope::TOTAL; ++i)
        {
            envelope::noise(ptr, points, envelope::envelope_t(i + envelope::FIRST));

            envelopes[i]    = ptr;
            ptr            += points;
        }

        // Print header
        printf("Index;Frequency;");
        for (size_t i=0; i<envelope::TOTAL; ++i)
            printf("%s;%s(dB);", envelope::envelopes[i], envelope::envelopes[i]);
        printf("\n");

        // Print items
        float kf = float(SPEC_FREQ_MAX) / float(points);
        for (size_t i=0; i<points; ++i)
        {
            printf("%d;%.3f;", int(i), i *  kf);
            for (size_t j=0; j< envelope::TOTAL; ++j)
                printf("%.7f;%.2f;", envelopes[j][i], 20 * log10(envelopes[j][i]));
            printf("\n");
        }

        delete [] buf;
    }

MTEST_END



