/*
 * parser.cpp
 *
 *  Created on: 16 окт. 2019 г.
 *      Author: sadko
 */

#include <locale.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <core/stdlib/math.h>
#include <core/files/json/Parser.h>

using namespace lsp;

static const char *json_event_name(json::event_type_t type)
{
    #define DECODE(x) case json::x: return #x;
    switch (type)
    {
        DECODE(JE_OBJECT_START)
        DECODE(JE_OBJECT_END)
        DECODE(JE_ARRAY_START)
        DECODE(JE_ARRAY_END)
        DECODE(JE_PROPERTY)
        DECODE(JE_STRING)
        DECODE(JE_INTEGER)
        DECODE(JE_DOUBLE)
        DECODE(JE_BOOL)
        DECODE(JE_NULL)
        default: break;
    }

    #undef DECODE
    return "JE_UNKNOWN";
}

UTEST_BEGIN("core.files.json", parser)

    void check_event(json::Parser &p, json::event_type_t type)
    {
        json::event_t ev;
        printf("  Checking event: 0x%02x (%s)\n", type, json_event_name(type));
        UTEST_ASSERT(p.read_next(&ev) == STATUS_OK);
        UTEST_ASSERT_MSG(ev.type == type, "Unexpected event type: 0x%02x (%s)", ev.type, json_event_name(ev.type));
    }

    void check_event(json::Parser &p, int value)
    {
        json::event_t ev;
        printf("  Checking event: 0x%02x (%s) -> %d\n", json::JE_INTEGER, json_event_name(json::JE_INTEGER), int(value));
        UTEST_ASSERT(p.read_next(&ev) == STATUS_OK);
        UTEST_ASSERT_MSG(ev.type == json::JE_INTEGER, "Unexpected event type: 0x%02x (%s)",
                ev.type, json_event_name(ev.type));
        UTEST_ASSERT_MSG(ev.iValue == value,"Unexpected event value: 0x%02x (%s) -> %d",
                ev.type, json_event_name(ev.type), int(ev.iValue));
    }

    void check_event(json::Parser &p, double value)
    {
        json::event_t ev;
        printf("  Checking event: 0x%02x (%s) -> %f\n", json::JE_DOUBLE, json_event_name(json::JE_DOUBLE), double(value));
        UTEST_ASSERT(p.read_next(&ev) == STATUS_OK);
        UTEST_ASSERT_MSG(ev.type == json::JE_DOUBLE, "Unexpected event type: 0x%02x (%s)",
                ev.type, json_event_name(ev.type));
        UTEST_ASSERT_MSG(float_equals_relative(ev.fValue, value), "Unexpected event value: 0x%02x (%s) -> %f",
                ev.type, json_event_name(ev.type), double(ev.fValue));
    }

    void check_event(json::Parser &p, bool value)
    {
        json::event_t ev;
        printf("  Checking event: 0x%02x (%s) -> %s\n", json::JE_BOOL, json_event_name(json::JE_BOOL), (value) ? "true" : "false");
        UTEST_ASSERT(p.read_next(&ev) == STATUS_OK);
        UTEST_ASSERT_MSG(ev.type == json::JE_BOOL, "Unexpected event type: 0x%02x (%s)",
                ev.type, json_event_name(ev.type));
        UTEST_ASSERT_MSG(ev.bValue == value, "Unexpected event value: 0x%02x (%s) -> %s",
                ev.type, json_event_name(ev.type), (ev.fValue) ? "true" : "false");
    }

    void check_event(json::Parser &p, json::event_type_t type, const char *text)
    {
        json::event_t ev;
        LSPString tmp;
        UTEST_ASSERT(tmp.set_utf8(text));
        printf("  Checking event: 0x%02x (%s) -> %s\n", type, json_event_name(type), tmp.get_utf8());
        UTEST_ASSERT(p.read_next(&ev) == STATUS_OK);
        UTEST_ASSERT_MSG(ev.type == type, "Unexpected event type: 0x%02x (%s)",
                ev.type, json_event_name(ev.type));
        UTEST_ASSERT_MSG(tmp.equals(&ev.sValue), "Unexpected event value: 0x%02x (%s) -> %s",
                ev.type, json_event_name(ev.type), ev.sValue.get_utf8());
    }

    void test_read_valid_events_json()
    {
        using namespace lsp::json;

        static const char *data =
                "{"
                    "\"version\": 1.0,"
                    "\"array\": ["
                        "\"string\","
                        "true,"
                        "false,"
                        "["
                            "1234,"
                            "\"value\","
                            "{}"
                        "]"
                    "],"
                    "\"object\": {"
                        "\"bvalue\": true,"
                        "\"ivalue\": 1024,"
                        "\"fvalue\": 440.0,"
                        "\"null\": null"
                    "}"
                "}";

        Parser p;
        UTEST_ASSERT(p.wrap(data, JSON_LEGACY, "UTF-8") == STATUS_OK);
        check_event(p, JE_OBJECT_START);
            check_event(p, JE_PROPERTY, "version"); check_event(p, 1.0);
            check_event(p, JE_PROPERTY, "array"); check_event(p, JE_ARRAY_START);
                check_event(p, JE_STRING, "string");
                check_event(p, true);
                check_event(p, false);
                check_event(p, JE_ARRAY_START);
                    check_event(p, 1234);
                    check_event(p, JE_STRING, "value");
                    check_event(p, JE_OBJECT_START);
                    check_event(p, JE_OBJECT_END);
                check_event(p, JE_ARRAY_END);
            check_event(p, JE_ARRAY_END);
            check_event(p, JE_PROPERTY, "object"); check_event(p, JE_OBJECT_START);
                check_event(p, JE_PROPERTY, "bvalue"); check_event(p, true);
                check_event(p, JE_PROPERTY, "ivalue"); check_event(p, 1024);
                check_event(p, JE_PROPERTY, "fvalue"); check_event(p, 440.0);
                check_event(p, JE_PROPERTY, "null"); check_event(p, JE_NULL);
            check_event(p, JE_OBJECT_END);
        check_event(p, JE_OBJECT_END);

        event_t ev;
        UTEST_ASSERT(p.read_next(&ev) == STATUS_EOF);
    }

    void test_read_valid_events_json5()
    {
        using namespace lsp::json;

        static const char *data =
                "/*some comment here */{"
                    "\"version\": 5.0, // test of JSON5\n"
                    "array: ["
                        "\"string\","
                        "true,"
                        "false,"
                        "12.34,"
                        "["
                            "0x1234,"
                            "\"value\","
                            "{},"
                        "]"
                    "],\n"
                    "// object test\n"
                    "\"object\": {"
                        "bvalue: true,"
                        "ivalue: 1024,"
                        "fvalue: 440.0,"
                        "svalue: \"string value\","
                        "\"null\": null,"
                    "},"
                    "\"extra\": ["
                        "0x10,"
                        "1.6,"
                        "false,"
                        "\"read_string\","
                        "null,"
                        "null,"
                        "null,"
                        "null,"
                    "]"
                "}\n"
                "// Comment after end";

        bool bv;
        ssize_t iv;
        double fv;
        LSPString sv;

        Parser p;
        UTEST_ASSERT(p.wrap(data, JSON_VERSION5, "UTF-8") == STATUS_OK);
        check_event(p, JE_OBJECT_START);
            check_event(p, JE_PROPERTY, "version"); check_event(p, 5.0);
            check_event(p, JE_PROPERTY, "array"); check_event(p, JE_ARRAY_START);
                check_event(p, JE_STRING, "string");
                check_event(p, true);
                UTEST_ASSERT(p.get_bool(NULL) == STATUS_OK);
                check_event(p, false);
                check_event(p, 12.34);
                UTEST_ASSERT(p.get_double(NULL) == STATUS_OK);
                check_event(p, JE_ARRAY_START);
                    check_event(p, 0x1234);
                    UTEST_ASSERT(p.get_int(NULL) == STATUS_OK);
                    check_event(p, JE_STRING, "value");
                    UTEST_ASSERT(p.get_string(NULL) == STATUS_OK);
                    check_event(p, JE_OBJECT_START);
                    check_event(p, JE_OBJECT_END);
                check_event(p, JE_ARRAY_END);
            check_event(p, JE_ARRAY_END);
            check_event(p, JE_PROPERTY, "object"); check_event(p, JE_OBJECT_START);
                check_event(p, JE_PROPERTY, "bvalue"); check_event(p, true);
                check_event(p, JE_PROPERTY, "ivalue"); check_event(p, 1024);
                check_event(p, JE_PROPERTY, "fvalue"); check_event(p, 440.0);
                check_event(p, JE_PROPERTY, "svalue"); check_event(p, JE_STRING, "string value");
                check_event(p, JE_PROPERTY, "null"); check_event(p, JE_NULL);
                UTEST_ASSERT(p.get_string(NULL) == STATUS_NULL);
                UTEST_ASSERT(p.get_int(NULL) == STATUS_NULL);
                UTEST_ASSERT(p.get_double(NULL) == STATUS_NULL);
                UTEST_ASSERT(p.get_bool(NULL) == STATUS_NULL);
            check_event(p, JE_OBJECT_END);
            check_event(p, JE_PROPERTY, "extra"); check_event(p, JE_ARRAY_START);
                UTEST_ASSERT(p.read_int(&iv) == STATUS_OK);
                UTEST_ASSERT(iv == 0x10);
                UTEST_ASSERT(p.read_double(&fv) == STATUS_OK);
                UTEST_ASSERT(float_equals_relative(fv, 1.6));
                UTEST_ASSERT(p.read_bool(&bv) == STATUS_OK);
                UTEST_ASSERT(bv == false);
                UTEST_ASSERT(p.read_string(&sv) == STATUS_OK);
                UTEST_ASSERT(sv.equals_ascii("read_string"));

                UTEST_ASSERT(p.read_int(&iv) == STATUS_NULL);
                UTEST_ASSERT(p.read_double(&fv) == STATUS_NULL);
                UTEST_ASSERT(p.read_bool(&bv) == STATUS_NULL);
                UTEST_ASSERT(p.read_string(&sv) == STATUS_NULL);
            check_event(p, JE_ARRAY_END);
        check_event(p, JE_OBJECT_END);

        event_t ev;
        UTEST_ASSERT(p.read_next(&ev) == STATUS_EOF);
    }

    status_t read_fully(const char *text, json::json_version_t version)
    {
        using namespace lsp::json;

        Parser p;
        event_t ev;
        status_t res;

        UTEST_ASSERT(p.wrap(text, version, "UTF-8") == STATUS_OK);

        while ((res = p.read_next(&ev)) == STATUS_OK) {
            printf("    Read event: 0x%02x (%s)\n", ev.type, json_event_name(ev.type));
        } // Nothing

        if (res == STATUS_EOF)
            res = p.close();
        else
            p.close();

        return res;
    }

    void test_read_invalid_json()
    {
        static const char *list[] = {
            "{",
            "}",
            "{a",
            "{\"a\"",
            "{\"a\"\"b\"",
            "{\"a\":",
            "{\"a\":b",
            "{\"a\":\"b\"",
            "{\"a\":\"b\"]",
            "{\"a\":\"b\",",
            "{\"a\":\"b\",}",
            "{\"a\":\"b\" \"c\":\"d\"}",
            "{a:\"b\"}",
            "{\"a\",\"b\"}",

            "[",
            "]",
            "[a",
            "[\"a\"",
            "[\"a\" \"b\"",
            "[\"a\",",
            "[\"a\",}",
            "[\"a\",]",
            "[\"a\", \"b\" \"c\"]",
            "[\"a\":\"b\"]",

            "0x123",
            "NULL",
            "a",
            "\"a\", \"b\"",
            "\"a\":\"b\"",
            "{},[]",
            "{}[]",
            "[]{}",
            "1, 2",

            NULL
        };

        for (const char **ptr = list; *ptr != NULL; ++ptr)
        {
            printf("  Testing: %s\n", *ptr);
            UTEST_ASSERT(read_fully(*ptr, json::JSON_LEGACY) != STATUS_OK);
        }
    }

    void test_read_invalid_json5()
    {
        static const char *list[] = {
            "{",
            "}",
            "{a",
            "{\"a\"",
            "{\"a\"\"b\"",
            "{\"a\":",
            "{\"a\":b",
            "{\"a\":\"b\"",
            "{\"a\":\"b\"]",
            "{\"a\":\"b\",",
            "{\"a\":\"b\" \"c\":\"d\"}",
            "{\"a\",\"b\"}",

            "[",
            "]",
            "[a",
            "[\"a\"",
            "[\"a\" \"b\"",
            "[\"a\",",
            "[\"a\",}",
            "[\"a\", \"b\" \"c\"]",
            "[\"a\":\"b\"]",

            "NULL",
            "a",
            "0x123,",
            "1,",
            "\"a\", \"b\"",
            "\"a\":\"b\"",
            "{},[]",
            "{}[]",
            "[]{}",
            "1, 2",

            NULL
        };

        for (const char **ptr = list; *ptr != NULL; ++ptr)
        {
            printf("  Testing: %s\n", *ptr);
            UTEST_ASSERT(read_fully(*ptr, json::JSON_VERSION5) != STATUS_OK);
        }
    }

    void test_read_valid_json()
    {
        static const char *list[] = {
            "1",
            "1.0",
            "null",
            "\"a\"",
            "[]",
            "{}",

            "[1]",
            "[1.0]",
            "[null]",
            "[\"a\"]",
            "[[]]",
            "[{}]",
            "[\"a\", 1]",
            "[\"a\", 1.0]",
            "[\"a\", null]",
            "[\"a\", \"b\"]",
            "[\"a\", []]",
            "[\"a\", {}]",

            "{\"a\":1}",
            "{\"a\":1.0}",
            "{\"a\":null}",
            "{\"a\":\"b\"}",
            "{\"a\":[]}",
            "{\"a\":{}}",
            "{\"a\":\"b\",\"c\":1}",
            "{\"a\":\"b\",\"c\":1.0}",
            "{\"a\":\"b\",\"c\":null}",
            "{\"a\":\"b\",\"c\":\"d\"}",
            "{\"a\":\"b\",\"c\":[]}",
            "{\"a\":\"b\",\"c\":{}}",

            "[{\"\":\"\"}]",
            NULL
        };

        for (const char **ptr = list; *ptr != NULL; ++ptr)
        {
            printf("  Testing: %s\n", *ptr);
            UTEST_ASSERT(read_fully(*ptr, json::JSON_LEGACY) == STATUS_OK);
        }
    }

    void test_read_valid_json5()
    {
        static const char *list[] = {
            "//a",
            "/*a*/",

            "1",
            "0x10",
            "1.0",
            "null",
            "\"a\"",
            "[]",
            "{}",

            "/*a*/1",
            "/*a*/0x10",
            "/*a*/1.0",
            "/*a*/null",
            "/*a*/\"a\"",
            "/*a*/[]",
            "/*a*/{}",

            "1//a",
            "0x10//a",
            "1.0//a",
            "null//a",
            "\"a\"//a",
            "[]//a",
            "{}//a",
            "[/*a*/]",
            "{/*a*/}",

            "[1]",
            "[0x10]",
            "[1.0]",
            "[null]",
            "[\"a\"]",
            "[[]]",
            "[{}]",
            "[1,]",
            "[0x10,]",
            "[1.0,]",
            "[null,]",
            "[\"a\",]",
            "[[],]",
            "[{},]",
            "[\"a\", 1]",
            "[\"a\", 0x10]",
            "[\"a\", 1.0]",
            "[\"a\", null]",
            "[\"a\", \"b\"]",
            "[\"a\", []]",
            "[\"a\", {}]",

            "{\"a\":1}",
            "{\"a\":0x10}",
            "{\"a\":1.0}",
            "{\"a\":null}",
            "{\"a\":\"b\"}",
            "{\"a\":[]}",
            "{\"a\":{}}",

            "{\"a\":1,}",
            "{\"a\":0x10,}",
            "{\"a\":1.0,}",
            "{\"a\":null,}",
            "{\"a\":\"b\",}",
            "{\"a\":[],}",
            "{\"a\":{},}",

            "{a:1}",
            "{a:0x10}",
            "{a:1.0}",
            "{a:null}",
            "{a:\"b\"}",
            "{a:[]}",
            "{a:{}}",

            "{a:1,}",
            "{a:0x10,}",
            "{a:1.0,}",
            "{a:null,}",
            "{a:\"b\",}",
            "{a:[],}",
            "{a:{},}",

            "{\"a\":\"b\",\"c\":1}",
            "{\"a\":\"b\",\"c\":0x10}",
            "{\"a\":\"b\",\"c\":1.0}",
            "{\"a\":\"b\",\"c\":null}",
            "{\"a\":\"b\",\"c\":\"d\"}",
            "{\"a\":\"b\",\"c\":[]}",
            "{\"a\":\"b\",\"c\":{}}",

            NULL
        };

        for (const char **ptr = list; *ptr != NULL; ++ptr)
        {
            printf("  Testing: %s\n", *ptr);
            UTEST_ASSERT(read_fully(*ptr, json::JSON_VERSION5) == STATUS_OK);
        }
    }

    void test_skip_json()
    {
        using namespace lsp::json;

        static const char *data =
                "{"
                    "\"version\": 1.0,"
                    "\"array\": ["
                        "\"string\","
                        "true,"
                        "false,"
                        "["
                            "1234,"
                            "\"value\","
                            "{}"
                        "]"
                    "],"
                    "\"object\": {"
                        "\"bvalue\": true,"
                        "\"ivalue\": 1024,"
                        "\"fvalue\": 440.0,"
                        "\"null\": null"
                    "}"
                "}";

        Parser p;
        UTEST_ASSERT(p.wrap(data, JSON_LEGACY, "UTF-8") == STATUS_OK);
        check_event(p, JE_OBJECT_START);
            UTEST_ASSERT(p.skip_next() == STATUS_OK);   // "version": 1.0
            check_event(p, JE_PROPERTY, "array"); check_event(p, JE_ARRAY_START);
                UTEST_ASSERT(p.skip_next() == STATUS_OK);   // "string"
                check_event(p, true);
                check_event(p, false);
                UTEST_ASSERT(p.skip_next() == STATUS_OK);   // [ ... ]
            check_event(p, JE_ARRAY_END);
            check_event(p, JE_PROPERTY, "object");
            UTEST_ASSERT(p.skip_current() == STATUS_OK); // { ... }
        check_event(p, JE_OBJECT_END);

        event_t ev;
        UTEST_ASSERT(p.read_next(&ev) == STATUS_EOF);
    }

    UTEST_MAIN
    {
        printf("Testing invalid json read...\n");
        test_read_invalid_json();
        printf("Testing invalid json5 read...\n");
        test_read_invalid_json5();

        printf("Testing valid json read...\n");
        test_read_valid_json();
        printf("Testing valid json5 read...\n");
        test_read_valid_json5();

        printf("Testing valid json events read...\n");
        test_read_valid_events_json();
        printf("Testing valid json5 events read...\n");
        test_read_valid_events_json5();

        printf("Testing skip method for json...\n");
        test_skip_json();
    }

UTEST_END
