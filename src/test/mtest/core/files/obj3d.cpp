/*
 * obj3d.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */


#include <test/mtest.h>
#include <core/files/3d/ObjFileParser.h>

using namespace lsp;

static const char *FILE_NAME = "test_data/3d/test-model.obj";

MTEST_BEGIN("core.files", obj3d)

    MTEST_MAIN
    {
        const char *path = FILE_NAME;
        if (argc > 0)
            path = argv[0];

        IFileHandler3D hdl;
        MTEST_ASSERT(ObjFileParser::parse(path, &hdl) == STATUS_OK);
    }

MTEST_END





