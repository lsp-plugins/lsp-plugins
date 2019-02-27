/*
 * Randomizer.cpp
 *
 *  Created on: 23 марта 2016 г.
 *      Author: sadko
 */

#include <core/util/Randomizer.h>
#include <time.h>
#include <math.h>

#define RAND_RANGE          2.32830643654e-10 /* 1 / (1 << 32) */
#define RAND_LAMBDA         M_E * M_SQRT2

#define RAND_T              0.5f

namespace lsp
{

    const uint32_t Randomizer::vMul1[] =
    {
        0x43ca16c1, 0x451222f3, 0x465e0183, 0x47f27263,
        0x4212ffe9, 0x4433f6ad, 0x40f31425, 0x412318bb,
        0x48f39cbf, 0x49b18a45, 0x4d341bbf, 0x4e93a169,
        0x4bacd5e5, 0x4c55e139, 0x4f11db4d, 0x4a901f8b
    };

    const uint32_t Randomizer::vMul2[] =
    {
        0x4c37c68f, 0x4d59b853, 0x4ef1d1e9, 0x4fe16c01,
        0x40fc2271, 0x44e335c1, 0x450fc1bb, 0x48cc3d07,
        0x493737a9, 0x4182e63f, 0x42198197, 0x43fc5611,
        0x4ac116eb, 0x4b0faf0d, 0x46777db9, 0x4730a64d
    };

    const uint32_t Randomizer::vAdders[] =
    {
        0x000551ff, 0x000633f5, 0x00011fcf, 0x00021b81,
        0x00075af1, 0x00080be5, 0x000330a7, 0x00040d0b,
        0x000c2521, 0x000dd113, 0x0009eea5, 0x000ae007,
        0x00092df5, 0x000b42bd, 0x000e1b15, 0x000f054d
    };

    void Randomizer::init(uint32_t seed)
    {
        for (size_t i=0; i<4; ++i)
        {
            uint32_t reseed     = (seed << (i * 8)) | (seed >> ((sizeof(uint32_t) - i) * 8));

            vRandom[i].vAdd     = vAdders[reseed & 0x0f];
            vRandom[i].vMul1    = vMul1[(reseed >> 4) & 0x0f];
            vRandom[i].vMul2    = vMul2[(reseed >> 8) & 0x0f];
            vRandom[i].vLast    = reseed ^ (seed >> 4);
        }

        nBufID      = 0;
    }

    void Randomizer::init()
    {
#if defined(PLATFORM_WINDOWS)
        FILETIME clock;
        GetSystemTimeAsFileTime(&clock);
        init(clock.dwHighDateTime ^ clock.dwLowDateTime);
#else
        struct timespec ts;

        if (clock_gettime(CLOCK_REALTIME, &ts) == 0)
            init(ts.tv_sec ^ ts.tv_nsec);
        else
            init(time(NULL));
#endif /* PLATFORM_WINDOWS */
    }

    float Randomizer::random(random_function_t func)
    {
        // Generate linear random number
        randgen_t *rg   = &vRandom[nBufID];
        nBufID          = (nBufID + 1) & 0x03;
        rg->vLast       = (rg->vMul1 * rg->vLast) + ((rg->vMul2 * rg->vLast) >> 16) + rg->vAdd;
        float rv        = rg->vLast * RAND_RANGE;

        // Now we can analyze algorithm
        switch (func)
        {
            case RND_EXP:
                return (expf(RAND_LAMBDA * rv) - 1.0f) / (expf(RAND_LAMBDA) - 1.0f);

            case RND_TRIANGLE:
                return (rv <= 0.5f) ?
                    M_SQRT2 * RAND_T * sqrtf(rv) :
                    2.0f*RAND_T - sqrtf(4.0f - 2.0f*(1.0f + rv)) * RAND_T;
//                    rv*rv/(2.0f * RAND_T * RAND_T) :
//                    rv*(2.0f/RAND_T) - rv*rv/(2.0f * RAND_T * RAND_T) - 1.0f;

            default:
                return rv;
        }
    }
} /* namespace lsp */
