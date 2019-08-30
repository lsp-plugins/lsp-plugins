/*
 * java.cpp
 *
 *  Created on: 30 авг. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/files/java/Handles.h>
#include <core/files/java/ObjectStream.h>

using namespace lsp;

UTEST_BEGIN("core.files", java)

    void read_test_file(java::ObjectStream *os)
    {
        java::String *str = NULL;
        uint32_t value = 0;

        UTEST_ASSERT(os->current_token() == java::JST_STRING);
        UTEST_ASSERT(os->read_string(&str) == STATUS_OK);
        UTEST_ASSERT(str->string()->equals_ascii("TMreq Filters File:Equaliser:Generic"));
        UTEST_ASSERT(str->handle() == 0);
        UTEST_ASSERT(str->references() == 2);
        str->release();
        str = NULL;

        UTEST_ASSERT(os->current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os->read_int(&value) == STATUS_OK);
        UTEST_ASSERT(value == 5);

        UTEST_ASSERT(os->current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os->read_int(&value) == STATUS_OK);
        UTEST_ASSERT(value == 18);

        UTEST_ASSERT(os->current_token() == java::JST_STRING);
        UTEST_ASSERT(os->read_string(&str) == STATUS_OK);
        UTEST_ASSERT(str->string()->equals_ascii("Notes:"));
        UTEST_ASSERT(str->handle() == 1);
        UTEST_ASSERT(str->references() == 2);
        str->release();
        str = NULL;

        UTEST_ASSERT(os->current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os->read_int(&value) == STATUS_OK);
        UTEST_ASSERT(value == 1);

        UTEST_ASSERT(os->current_token() == java::JST_ARRAY);
    }

    UTEST_MAIN
    {
        java::Handles handles;
        java::ObjectStream os(&handles);

        UTEST_ASSERT(os.open("res/test/rew/Test1.req") == STATUS_OK);
        UTEST_ASSERT(os.version() == 5);

        read_test_file(&os);

        UTEST_ASSERT(os.close() == STATUS_OK);
    }

UTEST_END


