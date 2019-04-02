/*
 * size_check.cpp
 *
 *  Created on: 24 февр. 2019 г.
 *      Author: sadko
 */

#include <core/alloc.h>
#include <test/utest.h>

#include <core/3d/common.h>

using namespace lsp;

#define CHECK_SIZE(type, multiple) \
    printf("sizeof(" #type ") = 0x%x\n", int(sizeof(type))); \
    if (sizeof(type) % multiple) \
        UTEST_FAIL_MSG("sizeof(" #type ") = 0x%x but should be multiple of %d bytes", int(sizeof(type)), int(multiple))

UTEST_BEGIN("core.3d", size_check)
    UTEST_MAIN
    {
        // All these structures should be multiple of 16-byte
        CHECK_SIZE(rt_split_t, 0x10);
        CHECK_SIZE(rt_triangle_t, 0x10);
        CHECK_SIZE(rt_material_t, 0x10);
        CHECK_SIZE(rt_view_t, 0x10);

        CHECK_SIZE(rtm_vertex_t, 0x10);
        CHECK_SIZE(rtm_edge_t, 0x10);
        CHECK_SIZE(rtm_triangle_t, 0x10);
    }
UTEST_END;



