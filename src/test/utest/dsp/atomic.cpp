/*
 * atomic.cpp
 *
 *  Created on: 25 февр. 2019 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <test/utest.h>
#include <dsp/atomic.h>

using namespace lsp;

UTEST_BEGIN("dsp", atomic)

    void test_cas()
    {
        size_t i;
        atomic_t value = 0;

        printf("Testing atomic_cas...\n");

        for (i=0; i<1000; ++i)
            if (atomic_cas(&value, 0, 1))
                break;
        UTEST_ASSERT(i < 1000);

        for (i=0; i<1000; ++i)
            if (atomic_cas(&value, 0, 1))
                break;
        UTEST_ASSERT(i >= 1000);

        for (i=0; i<1000; ++i)
            if (atomic_cas(&value, 1, 2))
                break;
        UTEST_ASSERT(i < 1000);
        UTEST_ASSERT(value == 2);
    }

    void test_swap()
    {
        atomic_t value1 = 0, value2 = 1;
        printf("Testing atomic_swap...\n");

        atomic_swap(&value1, 2);
        atomic_swap(&value2, 3);

        UTEST_ASSERT(value1 == 2);
        UTEST_ASSERT(value2 == 3);

        atomic_swap(&value1, value2);

        UTEST_ASSERT(value1 == 3);
    }

    void test_add()
    {
        atomic_t value = 0;
        printf("Testing atomic_add...\n");

        for (size_t i=0; i<1000; ++i)
            UTEST_ASSERT(atomic_add(&value, 1) == atomic_t(i)); // i == previous value
        UTEST_ASSERT(value == 1000);
    }

    UTEST_MAIN
    {
        test_cas();
        test_swap();
        test_add();
    }
UTEST_END;
