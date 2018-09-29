/*
 * convolver.cpp
 *
 *  Created on: 14 сент. 2018 г.
 *      Author: sadko
 */

#include <dsp/dsp.h>
#include <test/ptest.h>
#include <core/util/Convolver.h>

#define MIN_RANK        8
#define MAX_RANK        16
#define STEP_SIZE       128

#define MIN_LENGTH      (1 << MIN_RANK)
#define LEN_STEPS       3
#define LEN_SHIFT       4
#define MAX_LENGTH      (MIN_LENGTH << (LEN_STEPS * LEN_SHIFT))

using namespace dsp;
using namespace lsp;

//-----------------------------------------------------------------------------
// Performance test for equalizer module
PTEST_BEGIN("core.util", convolver, 10, 500)

    void call(float *out, const float *in, const float *conv, size_t count, size_t rank)
    {
        char buf[80];
        sprintf(buf, "length=%d, rank=%d", int(count), int(rank));
        printf("Testing convolver %s ...\n", buf);

        Convolver c;
        c.init(conv,  count, rank, 0.0f);

        PTEST_LOOP(buf,
                c.process(out, in, STEP_SIZE);
        );

        c.destroy();
    }

    PTEST_MAIN
    {
        uint8_t *data   = NULL;
        float *in       = alloc_aligned<float>(data, MAX_LENGTH + STEP_SIZE*4, 64);
        float *out      = &in[STEP_SIZE];
        float *backup   = &out[STEP_SIZE];
        float *conv     = &backup[STEP_SIZE*2];

        for (size_t i=0; i < (MAX_LENGTH + STEP_SIZE*4); ++i)
            in[i]           = float(rand()) / RAND_MAX;

        #define CALL(...)  { \
            dsp::copy(in, backup, STEP_SIZE*2); \
            call(__VA_ARGS__); \
        }

        for (size_t i=0, len=MIN_LENGTH; i<=LEN_STEPS; i++, len <<= LEN_SHIFT)
        {
            for (size_t rank=MIN_RANK; rank <= MAX_RANK; ++rank)
                CALL(out, in, conv, len, rank);

            PTEST_SEPARATOR;
        }

        free_aligned(data);
    }
PTEST_END



