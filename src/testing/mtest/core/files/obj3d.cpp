/*
 * obj3d.cpp
 *
 *  Created on: 29 авг. 2018 г.
 *      Author: sadko
 */


#include <core/files/3d/Parser.h>
#include <test/mtest.h>

using namespace lsp;
using namespace lsp::obj;

static const char *FILE_NAME = "test_data/3d/test-model.obj";

MTEST_BEGIN("core.files", obj3d)

    MTEST_MAIN
    {
        const char *path = FILE_NAME;
        if (argc > 0)
            path = argv[0];

        IObjHandler hdl;
        MTEST_ASSERT(Parser::parse(path, &hdl) == STATUS_OK);
    }

MTEST_END





