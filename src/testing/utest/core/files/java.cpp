/*
 * java.cpp
 *
 *  Created on: 30 авг. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/files/java/const.h>
#include <core/files/java/Handles.h>
#include <core/files/java/ObjectStream.h>

using namespace lsp;

UTEST_BEGIN("core.files", java)

#pragma pack(push, 1)
    union tptype
    {
        java::byte_t        v_byte;
        java::short_t       v_short;
        java::int_t         v_int;
        java::long_t        v_long;
        java::char_t        v_char;
        java::double_t      v_double;
        java::float_t       v_float;
        java::bool_t        v_bool;
        java::Object       *v_object;
    };
#pragma pack(pop)

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
        UTEST_ASSERT(arr->length() == 2);

        const java::Object **vobj = arr->get_objects();
        UTEST_ASSERT(vobj[0] != NULL);
        UTEST_ASSERT(vobj[1] != NULL);

        // Check item[0]
        const java::Object *obj = vobj[0];
        UTEST_ASSERT(::strcmp(obj->class_name(), "test.TestObject") == 0);

        tptype x;
        UTEST_ASSERT(obj->get_bool("xbool", &x.v_bool) == STATUS_OK);
        UTEST_ASSERT(bool(x.v_bool) == false);
        UTEST_ASSERT(obj->get_byte("xbyte", &x.v_byte) == STATUS_OK);
        UTEST_ASSERT(x.v_byte == java::byte_t(0x5a));
        UTEST_ASSERT(obj->get_char("xchar", &x.v_char) == STATUS_OK);
        UTEST_ASSERT(x.v_char == 'X');
        UTEST_ASSERT(obj->get_double("xdouble", &x.v_double) == STATUS_OK);
        UTEST_ASSERT(x.v_double == 440.0);
        UTEST_ASSERT(obj->get_float("xfloat", &x.v_float) == STATUS_OK);
        UTEST_ASSERT(x.v_float == 48000.0f);
        UTEST_ASSERT(obj->get_int("xint", &x.v_int) == STATUS_OK);
        UTEST_ASSERT(x.v_int == 0x10203040);
        UTEST_ASSERT(obj->get_long("xlong", &x.v_long) == STATUS_OK);
        UTEST_ASSERT(x.v_long == java::long_t(0x1122334455667788L));
        UTEST_ASSERT(obj->get_short("xshort", &x.v_short) == STATUS_OK);
        UTEST_ASSERT(x.v_short == 0x55aa);

        const java::RawArray *narr = NULL;
        UTEST_ASSERT(obj->get_array("iarray", &narr) == STATUS_OK);
        UTEST_ASSERT(narr != NULL);
        UTEST_ASSERT(narr->length() == 8);

        const java::int_t* ints = narr->get_ints();
        for (size_t i=0; i<8; ++i)
            UTEST_ASSERT(ints[i] == java::int_t(i+1));

        UTEST_ASSERT(obj->get_string("iref", &tmp) == STATUS_OK);
        UTEST_ASSERT(tmp.equals_ascii("Referenced string"));
        UTEST_ASSERT(obj->get_object("inull", NULL) == STATUS_NULL);
        UTEST_ASSERT(obj->get_enum("xenum", &tmp) == STATUS_OK);
        UTEST_ASSERT(tmp.equals_ascii("ONE"));

        // Check item[1]
        obj = vobj[1];
        UTEST_ASSERT(::strcmp(obj->class_name(), "test.TestObject") == 0);

        UTEST_ASSERT(obj->get_bool("xbool", &x.v_bool) == STATUS_OK);
        UTEST_ASSERT(bool(x.v_bool) == true);
        UTEST_ASSERT(obj->get_byte("xbyte", &x.v_byte) == STATUS_OK);
        UTEST_ASSERT(x.v_byte == java::byte_t(0xa5));
        UTEST_ASSERT(obj->get_char("xchar", &x.v_char) == STATUS_OK);
        UTEST_ASSERT(x.v_char == 'Y');
        UTEST_ASSERT(obj->get_double("xdouble", &x.v_double) == STATUS_OK);
        UTEST_ASSERT(x.v_double == 880.0);
        UTEST_ASSERT(obj->get_float("xfloat", &x.v_float) == STATUS_OK);
        UTEST_ASSERT(x.v_float == 41000.0f);
        UTEST_ASSERT(obj->get_int("xint", &x.v_int) == STATUS_OK);
        UTEST_ASSERT(x.v_int == 0x20304050);
        UTEST_ASSERT(obj->get_long("xlong", &x.v_long) == STATUS_OK);
        UTEST_ASSERT(x.v_long == java::long_t(0x5566778811223344L));
        UTEST_ASSERT(obj->get_short("xshort", &x.v_short) == STATUS_OK);
        UTEST_ASSERT(x.v_short == java::short_t(0xaa55));

        UTEST_ASSERT(obj->get_array("iarray", &narr) == STATUS_OK);
        UTEST_ASSERT(narr != NULL);
        UTEST_ASSERT(narr->length() == 4);

        ints = narr->get_ints();
        for (size_t i=0; i<4; ++i)
            UTEST_ASSERT(ints[i] == java::int_t(5-i));

        UTEST_ASSERT(obj->get_array("iref", &narr) == STATUS_OK);
        UTEST_ASSERT(narr != NULL);
        UTEST_ASSERT(narr->length() == 6);

        UTEST_ASSERT(obj->get_enum("inull", &tmp) == STATUS_OK);
        UTEST_ASSERT(tmp.equals_ascii("FOUR"));
        UTEST_ASSERT(obj->get_enum("xenum", &tmp) == STATUS_OK);
        UTEST_ASSERT(tmp.equals_ascii("TWO"));

        // Close file
        UTEST_ASSERT(os.close() == STATUS_OK);
        handles.flush();
    }

    void read_rew_file()
    {
        // Open file
        java::Handles handles;
        java::ObjectStream os(&handles);
        UTEST_ASSERT(os.open("res/test/rew/Test11.req") == STATUS_OK);
        UTEST_ASSERT(os.version() == 5);

        java::String *str = NULL;
        java::Object *array = NULL;
        uint32_t value = 0;
        LSPString tmp;

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
        UTEST_ASSERT(value == 19);

        UTEST_ASSERT(os.current_token() == java::JST_STRING);
        UTEST_ASSERT(os.read_string(&str) == STATUS_OK);
        UTEST_ASSERT(str->string()->equals_ascii("Notes:test notes"));
        str = NULL;

        UTEST_ASSERT(os.current_token() == java::JST_BLOCK_DATA);
        UTEST_ASSERT(os.read_int(&value) == STATUS_OK);
        UTEST_ASSERT(value == 1);

        UTEST_ASSERT(os.current_token() == java::JST_ARRAY);
        UTEST_ASSERT(os.read_object(&array) == STATUS_OK);

        UTEST_ASSERT(array->to_string(&tmp) == STATUS_OK);
        printf("Read array: %s", tmp.get_utf8());

        // Close file
        UTEST_ASSERT(os.close() == STATUS_OK);
        handles.flush();
    }

    UTEST_MAIN
    {
        printf("Testing JDK8 file...\n");
        read_jdk8_file();

        printf("Testing REW file...\n");
        read_rew_file();
    }

UTEST_END


