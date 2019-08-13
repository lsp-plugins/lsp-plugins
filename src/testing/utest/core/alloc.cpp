/*
 * alloc.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
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




