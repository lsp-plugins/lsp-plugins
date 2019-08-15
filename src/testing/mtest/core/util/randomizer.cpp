/*
 * randomizer.cpp
 *
 *  Created on: 24 авг. 2018 г.
 *      Author: sadko
 */


#include <test/mtest.h>

#include <core/types.h>
#include <core/alloc.h>
#include <data/cvector.h>

#include <core/util/Randomizer.h>

using namespace lsp;

MTEST_BEGIN("core.util", randomizer)

    MTEST_MAIN
    {
        size_t rows     = 32;
        if (argc > 0)
            rows            = atoi(argv[0]);
        if (rows < 4)
            rows            = 32;

        Randomizer rnd;
        rnd.init();

        int *counters   = new int[rows * rows];
        for (size_t i=0; i < rows*rows; ++i)
            counters[i]     = 0;

        for (size_t i=0; i<(rows*rows*1024); ++i)
        {
            size_t idx  = rows * rows * rnd.random(RND_TRIANGLE);
            counters[idx]++;
        }

        float max = 0;
        for (size_t i=0; i < rows*rows; ++i)
            if (max < counters[i])
                max = counters[i];
        max = 1.0f / max;

        for (size_t i=0; i<rows; ++i)
        {
            for (size_t j=0; j<rows; ++j)
                printf("%.3f ", counters[j * rows + i] * max);
            printf("\n");
        }

        printf("Probabilities:\n");
        printf("id;value\n");
        for (size_t i=0; i<rows * rows; ++i)
            printf("%d;%.4f\n", int(i), counters[i] * max);

        delete [] counters;

        printf("\nRandom noise:\n");
        for (size_t i=0; i<rows * rows; ++i)
            printf("%d;%.5f\n", int(i), rnd.random(RND_TRIANGLE) - 0.5f);
    }

MTEST_END






