/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 1 апр. 2019 г.
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

#include <test/utest.h>
#include <core/io/Path.h>
#include <core/files/Model3DFile.h>

using namespace lsp;

UTEST_BEGIN("core.files", objfile)

    UTEST_MAIN
    {
        io::Path path;
        path.set("res/test/3d/parse/parse.obj");

        Scene3D s;
        status_t res = Model3DFile::load(&s, &path, true);
        UTEST_ASSERT_MSG(res == STATUS_OK, "Error loading scene from file %s", path.as_native());

        UTEST_ASSERT(s.num_objects() == 2);
        UTEST_ASSERT(s.num_normals() == 1);
        UTEST_ASSERT(s.num_vertexes() == 4096);

        Object3D *obj = s.object(0);
        UTEST_ASSERT(!strcmp(obj->get_name(), "Circle 0 \\  with comment"));
        UTEST_ASSERT(obj->num_triangles() >= 2046);

        obj = s.object(1);
        UTEST_ASSERT(!strcmp(obj->get_name(), "Circle #1 with # sharps \\and line split"));
        UTEST_ASSERT(obj->num_triangles() >= 2046);

        s.destroy();
    }

UTEST_END;


