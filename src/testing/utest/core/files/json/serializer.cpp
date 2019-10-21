/*
 * serializer.cpp
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: sadko
 */

#include <locale.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <core/stdlib/math.h>
#include <core/files/json/Serializer.h>

using namespace lsp;
using namespace lsp::json;

UTEST_BEGIN("core.files.json", serializer)

    void test_serialize_minimized_json_object()
    {
        using namespace lsp::json;

        LSPString out;
        Serializer s;

        const char *data =
            "{"
                "\"array\":["
                    "123,"
                    "12.34,"
                    "true,"
                    "null,"
                    "\"multiline\\n\\rstring\","
                    "{},"
                    "[]"
                "],"
                "\"object\":{"
                    "\"int\":123,"
                    "\"double\":NaN,"
                    "\"bool\":false,"
                    "\"null\":null,"
                    "\"string\":\"test\\n\","
                    "\"object\":{},"
                    "\"array\":[]"
                "}"
            "}";

        UTEST_ASSERT(s.wrap(&out, NULL) == STATUS_OK);

        UTEST_ASSERT(s.write_comment("comment") == STATUS_INVALID_VALUE);
        UTEST_ASSERT(s.start_object() == STATUS_OK);
            // Try to rite invalid data
            UTEST_ASSERT(s.write_int(123) == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_hex(0x123) == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_double(12.34, "%f") == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_bool(true) == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_null() == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_string("version") == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_comment("comment") == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.start_object() == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.start_array() == STATUS_INVALID_VALUE);

            UTEST_ASSERT(s.write_property("array") == STATUS_OK);
            UTEST_ASSERT(s.start_array() == STATUS_OK);
                UTEST_ASSERT(s.write_int(123) == STATUS_OK);
                UTEST_ASSERT(s.write_hex(0x123) == STATUS_INVALID_VALUE);
                UTEST_ASSERT(s.write_double(12.34, "%.2f") == STATUS_OK);
                UTEST_ASSERT(s.write_bool(true) == STATUS_OK);
                UTEST_ASSERT(s.write_null() == STATUS_OK);
                UTEST_ASSERT(s.write_string("multiline\n\rstring") == STATUS_OK);
                UTEST_ASSERT(s.write_comment("comment") == STATUS_INVALID_VALUE);

                UTEST_ASSERT(s.start_object() == STATUS_OK);
                UTEST_ASSERT(s.end_object() == STATUS_OK);
                UTEST_ASSERT(s.start_array() == STATUS_OK);
                UTEST_ASSERT(s.end_array() == STATUS_OK);
            UTEST_ASSERT(s.end_array() == STATUS_OK);

            UTEST_ASSERT(s.write_property("object") == STATUS_OK);
            UTEST_ASSERT(s.write_comment("comment") == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.start_object() == STATUS_OK);
                UTEST_ASSERT(s.write_property("int") == STATUS_OK);
                UTEST_ASSERT(s.write_comment("comment") == STATUS_INVALID_VALUE);
                UTEST_ASSERT(s.write_hex(0x123) == STATUS_INVALID_VALUE);
                UTEST_ASSERT(s.write_int(123) == STATUS_OK);

                UTEST_ASSERT(s.write_property("double") == STATUS_OK);
                UTEST_ASSERT(s.write_double(NAN) == STATUS_OK);

                UTEST_ASSERT(s.write_property("bool") == STATUS_OK);
                UTEST_ASSERT(s.write_bool(false) == STATUS_OK);

                UTEST_ASSERT(s.write_property("null") == STATUS_OK);
                UTEST_ASSERT(s.write_null() == STATUS_OK);

                UTEST_ASSERT(s.write_property("string") == STATUS_OK);
                UTEST_ASSERT(s.write_string("test\n") == STATUS_OK);

                UTEST_ASSERT(s.write_comment("comment") == STATUS_INVALID_VALUE);

                UTEST_ASSERT(s.write_property("object") == STATUS_OK);
                UTEST_ASSERT(s.start_object() == STATUS_OK);
                UTEST_ASSERT(s.end_object() == STATUS_OK);

                UTEST_ASSERT(s.write_property("array") == STATUS_OK);
                UTEST_ASSERT(s.start_array() == STATUS_OK);
                UTEST_ASSERT(s.end_array() == STATUS_OK);
            UTEST_ASSERT(s.end_object() == STATUS_OK);

            UTEST_ASSERT(s.write_comment("comment") == STATUS_INVALID_VALUE);

        UTEST_ASSERT(s.end_object() == STATUS_OK);
        UTEST_ASSERT(s.write_comment("comment") == STATUS_INVALID_VALUE);
        UTEST_ASSERT(s.close() == STATUS_OK);

        printf("Serialized data: %s\n", out.get_utf8());
        printf("Ethalon data:    %s\n", data);
        UTEST_ASSERT(out.equals_ascii(data));
    }

    void test_serialize_minimized_json5_object()
    {
        using namespace lsp::json;

        LSPString out;
        Serializer s;

        const char *data =
            "/*c1\\U000A*/{"
                "/*/\\U002Ac2*\\U002F*/"
                "array:["
                    "123,/*q0*/"
                    "-0x123,"
                    "Infinity,"
                    "true,"
                    "null,"
                    "\"multiline\\n\\rstring\"/*c3*/,/*q1*/"
                    "{}/*q2*/,"
                    "[]/*q3*/"
                "],"
                "object:/*c4*/{"
                    "\"int\":/*c5*/123/*q6*/,"
                    "hex:/*q7*/0x123,/*q8*/"
                    "\"double\":-Infinity,"
                    "bool:false,"
                    "\"null\":null,"
                    "string:\"test\\n\"/*comment*/,"
                    "object:{/*q10*/},/*q9*/"
                    "array:[/*q12*/]/*q11*/,"
                "}/*c6*/"
            "}/*c7*/";

        serial_flags_t settings;
        settings.version        = JSON_VERSION5;
        settings.identifiers    = true;
        settings.ident          = ' ';
        settings.padding        = 0;
        settings.separator      = false;
        settings.multiline      = false;

        UTEST_ASSERT(s.wrap(&out, &settings) == STATUS_OK);

        UTEST_ASSERT(s.write_comment("c1\n") == STATUS_OK);
        UTEST_ASSERT(s.start_object() == STATUS_OK);
            // Try to rite invalid data
            UTEST_ASSERT(s.write_int(123) == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_hex(0x123) == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_double(12.34, "%f") == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_bool(true) == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_null() == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_string("version") == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_comment("/*c2*/") == STATUS_OK);
            UTEST_ASSERT(s.start_object() == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.start_array() == STATUS_INVALID_VALUE);

            UTEST_ASSERT(s.write_property("array") == STATUS_OK);
            UTEST_ASSERT(s.start_array() == STATUS_OK);
                UTEST_ASSERT(s.write_int(123) == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q0") == STATUS_OK);

                UTEST_ASSERT(s.write_hex(-0x123) == STATUS_OK);
                UTEST_ASSERT(s.write_double(INFINITY) == STATUS_OK);
                UTEST_ASSERT(s.write_bool(true) == STATUS_OK);
                UTEST_ASSERT(s.write_string(NULL, NULL) == STATUS_OK);

                UTEST_ASSERT(s.write_string("multiline\n\rstring") == STATUS_OK);
                UTEST_ASSERT(s.write_comment("c3") == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q1") == STATUS_OK);

                UTEST_ASSERT(s.start_object() == STATUS_OK);
                UTEST_ASSERT(s.end_object() == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q2") == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);

                UTEST_ASSERT(s.start_array() == STATUS_OK);
                UTEST_ASSERT(s.end_array() == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q3") == STATUS_OK);
            UTEST_ASSERT(s.end_array() == STATUS_OK);

            UTEST_ASSERT(s.write_property("object") == STATUS_OK);
            UTEST_ASSERT(s.write_comment("c4") == STATUS_OK);
            UTEST_ASSERT(s.start_object() == STATUS_OK);
                UTEST_ASSERT(s.write_property("int") == STATUS_OK);
                UTEST_ASSERT(s.write_comment("c5") == STATUS_OK);
                UTEST_ASSERT(s.write_int(123) == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q6") == STATUS_OK);

                UTEST_ASSERT(s.write_property("hex") == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q7") == STATUS_OK);
                UTEST_ASSERT(s.write_hex(0x123) == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q8") == STATUS_OK);

                UTEST_ASSERT(s.write_property("double") == STATUS_OK);
                UTEST_ASSERT(s.write_double(-INFINITY) == STATUS_OK);

                UTEST_ASSERT(s.write_property("bool") == STATUS_OK);
                UTEST_ASSERT(s.write_bool(false) == STATUS_OK);

                UTEST_ASSERT(s.write_property("null") == STATUS_OK);
                UTEST_ASSERT(s.write_null() == STATUS_OK);

                UTEST_ASSERT(s.write_property("string") == STATUS_OK);
                UTEST_ASSERT(s.write_string("test\n") == STATUS_OK);

                UTEST_ASSERT(s.write_comment("comment") == STATUS_OK);

                UTEST_ASSERT(s.write_property("object") == STATUS_OK);
                UTEST_ASSERT(s.start_object() == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_INVALID_VALUE);
                UTEST_ASSERT(s.write_comment("q10") == STATUS_OK);
                UTEST_ASSERT(s.end_object() == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q9") == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_INVALID_VALUE);

                UTEST_ASSERT(s.write_property("array") == STATUS_OK);
                UTEST_ASSERT(s.start_array() == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q12") == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_INVALID_VALUE);
                UTEST_ASSERT(s.end_array() == STATUS_OK);
                UTEST_ASSERT(s.write_comment("q11") == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
            UTEST_ASSERT(s.end_object() == STATUS_OK);
            UTEST_ASSERT(s.write_comment("c6") == STATUS_OK);

        UTEST_ASSERT(s.end_object() == STATUS_OK);
        UTEST_ASSERT(s.write_comment("c7") == STATUS_OK);
        UTEST_ASSERT(s.close() == STATUS_OK);

        printf("Serialized data: %s\n", out.get_utf8());
        printf("Ethalon data:    %s\n", data);
        UTEST_ASSERT(out.equals_ascii(data));
    }

    void test_serialize_formatted_json5_object()
    {
        using namespace lsp::json;

        LSPString out;
        Serializer s;

        const char *data =
            "/*\n"
            " * c1\n"
            " */\n"
            "{\n"
            "    /*/\\U002A array *\\U002F*/\n"
            "    array: [\n"
            "        123, /* integer */\n"
            "        -0x123, /* hex */\n"
            "        Infinity, /* double */\n"
            "        true, /* boolean */\n"
            "        null, /* null string */\n"
            "        \"multiline\\n\\rstring\", /* multiline string */\n"
            "        {}, /* empty object */\n"
            "        [], /* empty array */\n"
            "    ], /* end of array */\n"
            "    /*** object ***/\n"
            "    object: {\n"
            "        \"int\": /* key */ 123, /* value */\n"
            "        hex: 0x123,\n"
            "        \"double\": -Infinity,\n"
            "        bool: false,\n"
            "        \"null\": null,\n"
            "        string: /* key */ \"test\\n\" /* value */ /*comment*/,\n"
            "        object: /* key */ {} /* value */,\n"
            "        array: /* key */ [] /* value */\n"
            "    } /* end of object */\n"
            "}\n"
            "/* end of file */";

        serial_flags_t settings;
        settings.version        = JSON_VERSION5;
        settings.identifiers    = true;
        settings.ident          = ' ';
        settings.padding        = 4;
        settings.separator      = true;
        settings.multiline      = true;

        UTEST_ASSERT(s.wrap(&out, &settings) == STATUS_OK);

        UTEST_ASSERT(s.write_comment("\n * c1\n ") == STATUS_OK);
        UTEST_ASSERT(s.writeln() == STATUS_OK);
        UTEST_ASSERT(s.start_object() == STATUS_OK);
            // Try to rite invalid data
            UTEST_ASSERT(s.write_int(123) == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_hex(0x123) == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_double(12.34, "%f") == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_bool(true) == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_null() == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.write_string("version") == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.start_object() == STATUS_INVALID_VALUE);
            UTEST_ASSERT(s.start_array() == STATUS_INVALID_VALUE);

            UTEST_ASSERT(s.writeln() == STATUS_OK);
            UTEST_ASSERT(s.write_comment("/* array */") == STATUS_OK);
            UTEST_ASSERT(s.write_property("array") == STATUS_OK);
            UTEST_ASSERT(s.start_array() == STATUS_OK);
                UTEST_ASSERT(s.write_int(123) == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" integer ") == STATUS_OK);

                UTEST_ASSERT(s.write_hex(-0x123) == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" hex ") == STATUS_OK);

                UTEST_ASSERT(s.write_double(INFINITY) == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" double ") == STATUS_OK);

                UTEST_ASSERT(s.write_bool(true) == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" boolean ") == STATUS_OK);

                UTEST_ASSERT(s.write_string(NULL, NULL) == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" null string ") == STATUS_OK);

                UTEST_ASSERT(s.write_string("multiline\n\rstring") == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" multiline string ") == STATUS_OK);

                UTEST_ASSERT(s.start_object() == STATUS_OK);
                UTEST_ASSERT(s.end_object() == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" empty object ") == STATUS_OK);

                UTEST_ASSERT(s.start_array() == STATUS_OK);
                UTEST_ASSERT(s.end_array() == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" empty array ") == STATUS_OK);
            UTEST_ASSERT(s.end_array() == STATUS_OK);
            UTEST_ASSERT(s.write_comma() == STATUS_OK);
            UTEST_ASSERT(s.write_comment(" end of array ") == STATUS_OK);

            UTEST_ASSERT(s.writeln() == STATUS_OK);
            UTEST_ASSERT(s.write_comment("** object **") == STATUS_OK);
            UTEST_ASSERT(s.write_property("object") == STATUS_OK);
            UTEST_ASSERT(s.start_object() == STATUS_OK);
                UTEST_ASSERT(s.write_property("int") == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" key ") == STATUS_OK);
                UTEST_ASSERT(s.write_int(123) == STATUS_OK);
                UTEST_ASSERT(s.write_comma() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" value ") == STATUS_OK);

                UTEST_ASSERT(s.write_property("hex") == STATUS_OK);
                UTEST_ASSERT(s.write_hex(0x123) == STATUS_OK);

                UTEST_ASSERT(s.write_property("double") == STATUS_OK);
                UTEST_ASSERT(s.write_double(-INFINITY) == STATUS_OK);

                UTEST_ASSERT(s.write_property("bool") == STATUS_OK);
                UTEST_ASSERT(s.write_bool(false) == STATUS_OK);

                UTEST_ASSERT(s.write_property("null") == STATUS_OK);
                UTEST_ASSERT(s.write_null() == STATUS_OK);

                UTEST_ASSERT(s.write_property("string") == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" key ") == STATUS_OK);
                UTEST_ASSERT(s.write_string("test\n") == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" value ") == STATUS_OK);
                UTEST_ASSERT(s.write_comment("comment") == STATUS_OK);

                UTEST_ASSERT(s.write_property("object") == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" key ") == STATUS_OK);
                UTEST_ASSERT(s.start_object() == STATUS_OK);
                UTEST_ASSERT(s.end_object() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" value ") == STATUS_OK);

                UTEST_ASSERT(s.write_property("array") == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" key ") == STATUS_OK);
                UTEST_ASSERT(s.start_array() == STATUS_OK);
                UTEST_ASSERT(s.end_array() == STATUS_OK);
                UTEST_ASSERT(s.write_comment(" value ") == STATUS_OK);
            UTEST_ASSERT(s.end_object() == STATUS_OK);
            UTEST_ASSERT(s.write_comment(" end of object ") == STATUS_OK);

        UTEST_ASSERT(s.end_object() == STATUS_OK);
        UTEST_ASSERT(s.writeln() == STATUS_OK);
        UTEST_ASSERT(s.write_comment(" end of file ") == STATUS_OK);
        UTEST_ASSERT(s.close() == STATUS_OK);

        printf("Serialized data:\n%s\n", out.get_utf8());
        printf("Ethalon data:\n%s\n", data);
        UTEST_ASSERT(out.equals_ascii(data));
    }

    UTEST_MAIN
    {
        printf("Testing serialization of minimized json...\n");
        test_serialize_minimized_json_object();
        printf("Testing serialization of minimized json5...\n");
        test_serialize_minimized_json5_object();
        printf("Testing serialization of formatted json5...\n");
        test_serialize_formatted_json5_object();
    }

UTEST_END
