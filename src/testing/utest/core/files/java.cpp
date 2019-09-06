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

    void read_jdk8_file()
    {
        // Open file
        java::Handles handles;
        java::ObjectStream os(&handles);
        UTEST_ASSERT(os.open("res/test/java/test-jdk8.bin") == STATUS_OK);
        UTEST_ASSERT(os.version() == 5);

        uint8_t u8;
        uint32_t u32;
        LSPString lstr;
        LSPString tmp;
        java::String *jstr = NULL;
        java::Object *obj = NULL;
        java::RawArray *arr = NULL;

        // Read block data
        UTEST_ASSERT(os.current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os.read_int(&u32) == STATUS_OK);
        UTEST_ASSERT(u32 == 0x11332244);
        UTEST_ASSERT(os.current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os.read_byte(&u8) == STATUS_OK);
        UTEST_ASSERT(u8 == 0xce);
        UTEST_ASSERT(os.current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os.read_utf(&lstr) == STATUS_OK);
        UTEST_ASSERT(lstr.equals_ascii("writeUTF string"));
        UTEST_ASSERT(os.current_token() == java::JST_STRING);
        UTEST_ASSERT(os.read_string(&jstr) == STATUS_OK);
        UTEST_ASSERT(jstr->string()->equals_ascii("writeObject string"));
        UTEST_ASSERT(jstr->to_string(&tmp) == STATUS_OK);
        printf("Read string: %s", tmp.get_utf8());
        jstr = NULL;
        tmp.clear();

        // Read array
        UTEST_ASSERT(os.current_token() == java::JST_ARRAY);
        UTEST_ASSERT(os.read_array(&arr) == STATUS_OK);
        UTEST_ASSERT(arr->to_string(&tmp) == STATUS_OK);
        printf("Read array: %s", tmp.get_utf8());

        // Close file
        UTEST_ASSERT(os.close() == STATUS_OK);
        handles.flush();
    }

    void read_rew_file()
    {
        // Open file
        java::Handles handles;
        java::ObjectStream os(&handles);
        UTEST_ASSERT(os.open("res/test/rew/Test1.req") == STATUS_OK);
        UTEST_ASSERT(os.version() == 5);

        java::String *str = NULL;
        java::Object *array = NULL;
        uint32_t value = 0;

        // Perform read
        UTEST_ASSERT(os.current_token() == java::JST_STRING);
        UTEST_ASSERT(os.read_string(&str) == STATUS_OK);
        UTEST_ASSERT(str->string()->equals_ascii("TMreq Filters File:Equaliser:Generic"));
        str = NULL;

        UTEST_ASSERT(os.current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os.read_int(&value) == STATUS_OK);
        UTEST_ASSERT(value == 5);

        UTEST_ASSERT(os.current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os.read_int(&value) == STATUS_OK);
        UTEST_ASSERT(value == 18);

        UTEST_ASSERT(os.current_token() == java::JST_STRING);
        UTEST_ASSERT(os.read_string(&str) == STATUS_OK);
        UTEST_ASSERT(str->string()->equals_ascii("Notes:"));
        str = NULL;

        UTEST_ASSERT(os.current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os.read_int(&value) == STATUS_OK);
        UTEST_ASSERT(value == 1);

        UTEST_ASSERT(os.current_token() == java::JST_ARRAY);
        UTEST_ASSERT(os.read_object(&array) == STATUS_OK);

        // Close file
        UTEST_ASSERT(os.close() == STATUS_OK);
        handles.flush();
    }

    UTEST_MAIN
    {
        printf("Testing JDK8 file...\n");
        read_jdk8_file();

//        printf("Testing REW file...\n");
//        read_rew_file();
    }

UTEST_END


