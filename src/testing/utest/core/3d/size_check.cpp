/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 февр. 2019 г.
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



