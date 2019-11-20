/*
 * player.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/FloatBuffer.h>
#include <dsp/dsp.h>
#include <core/sampling/SamplePlayer.h>

#define SAMPLE_LENGTH       8

static const float samples[4][SAMPLE_LENGTH] =
{
    { 1, -1, 1, -1, 1, 1, -1, -1 },
    { 1, 2, 3, 5, 7, 11, 13, 17 },
    { 4, 3, 2, 1, 1, 2, 3, 4 },
    { 1, 2, 3, 2, 2, 3, 2, 1 }
};

using namespace lsp;

UTEST_BEGIN("core.sampling", player)
    UTEST_MAIN
    {
        SamplePlayer sp;
        sp.init(4, 5);

        FloatBuffer src(0x100);
        src.fill_zero();
        FloatBuffer dst1(src);
        FloatBuffer dst2(src.size());

        // Initialize samples
        for (size_t i=0; i<4; ++i)
        {
            Sample *s = new Sample;
            s->init(1, SAMPLE_LENGTH, SAMPLE_LENGTH);
            dsp::copy(s->getBuffer(0), samples[i], SAMPLE_LENGTH);
            UTEST_ASSERT(sp.bind(i, s));
        }

        // Trigger playing
        float *dptr = dst1;
        dsp::copy(dptr, src, src.size());
        for (size_t i=0; i<4; ++i)
        {
            dsp::fmadd_k3(&dptr[(i+1) * 11], samples[i], (i+1)*1.1f, SAMPLE_LENGTH);
            UTEST_ASSERT(sp.play(i, 0, (i+1) * 1.1f, (i+1) * 11));
        }

        // Call processing
        dptr = dst2;
        float *sptr = src;
        for (size_t processed = 0; processed <= src.size(); processed += 16)
        {
            size_t n = src.size() - processed;
            if (n > 16)
                n = 16;
            sp.process(&dptr[processed], &sptr[processed], n);
        }

        // Destroy player
        sp.destroy(true);

        // Check state
        UTEST_ASSERT_MSG(src.valid(), "Source buffer corrupted");
        UTEST_ASSERT_MSG(dst1.valid(), "Destination buffer 1 corrupted");
        UTEST_ASSERT_MSG(dst2.valid(), "Destination buffer 2 corrupted");

        // Compare buffers
        if (!dst1.equals_absolute(dst2, 1e-5))
        {
            src.dump("src");
            dst1.dump("dst1");
            dst2.dump("dst2");
        }
    }
UTEST_END;







