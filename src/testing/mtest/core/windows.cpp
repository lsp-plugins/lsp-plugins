/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 28 авг. 2018 г.
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

#include <test/mtest.h>

#include <core/types.h>
#include <core/alloc.h>
#include <data/cvector.h>

#include <core/windows.h>

using namespace lsp;

MTEST_BEGIN("core", windows)

    MTEST_MAIN
    {
        float *buf      = NULL;
        float *windows[windows::TOTAL];

        size_t points   = 2400;
        if (argc > 0)
            points          = atoi(argv[0]);
        if (points < 10)
            points          = 10;

        size_t count    = points * windows::TOTAL;
        buf             = new float[count];
        MTEST_ASSERT(buf != NULL);

        float *ptr      = buf;
        for (size_t i=0; i<windows::TOTAL; ++i)
        {
            windows::window(ptr, points, windows::window_t(i + windows::FIRST));

            windows[i]      = ptr;
            ptr            += points;
        }

        // Print header
        printf("Index;");
        for (size_t i=0; i<windows::TOTAL; ++i)
            printf("%s;", windows::windows[i]);
        printf("\n");

        // Print items
        for (size_t i=0; i<points; ++i)
        {
            printf("%d;", int(i));
            for (size_t j=0; j< windows::TOTAL; ++j)
                printf("%.5f;", windows[j][i]);
            printf("\n");
        }

        delete [] buf;
    }

MTEST_END



