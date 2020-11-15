/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 авг. 2018 г.
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

using namespace lsp;

UTEST_BEGIN("core", alloc)
    UTEST_MAIN
    {
    #if defined(LSP_DEBUG) && defined(LSP_MEMORY_PROFILING)
        printf("Validating normal malloc...");
        uint8_t *data   = reinterpret_cast<uint8_t *>(lsp_malloc(0x20));
        UTEST_ASSERT(data != NULL);
        UTEST_ASSERT(lsp_validate(data));
        UTEST_ASSERT(lsp_free(data));

        printf("Validating alloc/realloc/free...");
        data    = reinterpret_cast<uint8_t *>(lsp_malloc(0x20));
        UTEST_ASSERT(data != NULL);
        UTEST_ASSERT(lsp_validate(data));
        data    = reinterpret_cast<uint8_t *>(lsp_realloc(data, 0x40));
        UTEST_ASSERT(data != NULL);
        UTEST_ASSERT(lsp_validate(data));
        UTEST_ASSERT(lsp_free(data));

        printf("Corrupting buffer at the head...");
        data    = reinterpret_cast<uint8_t *>(lsp_malloc(0x20));
        UTEST_ASSERT(data != NULL);
        UTEST_ASSERT(lsp_validate(data));
        data[-1]    = 0xff;
        UTEST_ASSERT(!lsp_free(data));

        printf("Corrupting buffer at the tail...");
        data    = reinterpret_cast<uint8_t *>(lsp_malloc(0x20));
        UTEST_ASSERT(data != NULL);
        UTEST_ASSERT(lsp_validate(data));
        data[0x21]  = 0xff;
        UTEST_ASSERT(!lsp_free(data));
    #endif
    }
UTEST_END;




