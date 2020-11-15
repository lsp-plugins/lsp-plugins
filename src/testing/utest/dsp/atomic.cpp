/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 25 февр. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
