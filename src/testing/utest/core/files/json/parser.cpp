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
        UTEST_ASSERT(p.get_next(&ev) == STATUS_OK);
        UTEST_ASSERT_MSG(ev.type == type, "Unexpected event type: 0x%02x (%s)", ev.type, json_event_name(ev.type));
    }

    void check_event(json::Parser &p, int value)
    {
        json::event_t ev;
        printf("  Checking event: 0x%02x (%s) -> %d\n", json::JE_INTEGER, json_event_name(json::JE_INTEGER), int(value));
        UTEST_ASSERT(p.get_next(&ev) == STATUS_OK);
        UTEST_ASSERT_MSG(ev.type == json::JE_INTEGER, "Unexpected event type: 0x%02x (%s)",
                ev.type, json_event_name(ev.type));
        UTEST_ASSERT_MSG(ev.iValue == value,"Unexpected event value: 0x%02x (%s) -> %d",
                ev.type, json_event_name(ev.type), int(ev.iValue));
    }

    void check_event(json::Parser &p, double value)
    {
        json::event_t ev;
        printf("  Checking event: 0x%02x (%s) -> %f\n", json::JE_DOUBLE, json_event_name(json::JE_DOUBLE), double(value));
        UTEST_ASSERT(p.get_next(&ev) == STATUS_OK);
        UTEST_ASSERT_MSG(ev.type == json::JE_DOUBLE, "Unexpected event type: 0x%02x (%s)",
                ev.type, json_event_name(ev.type));
        UTEST_ASSERT_MSG(float_equals_relative(ev.fValue, value), "Unexpected event value: 0x%02x (%s) -> %f",
                ev.type, json_event_name(ev.type), double(ev.fValue));
    }

    void check_event(json::Parser &p, bool value)
    {
        json::event_t ev;
        printf("  Checking event: 0x%02x (%s) -> %s\n", json::JE_BOOL, json_event_name(json::JE_BOOL), (value) ? "true" : "false");
        UTEST_ASSERT(p.get_next(&ev) == STATUS_OK);
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
        UTEST_ASSERT(p.get_next(&ev) == STATUS_OK);
        UTEST_ASSERT_MSG(ev.type == type, "Unexpected event type: 0x%02x (%s)",
                ev.type, json_event_name(ev.type));
        UTEST_ASSERT_MSG(tmp.equals(&ev.sValue), "Unexpected event value: 0x%02x (%s) -> %s",
                ev.type, json_event_name(ev.type), ev.sValue.get_utf8());
    }

    void test_read_valid_json()
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
                            "\"value\""
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
        UTEST_ASSERT(p.open(data, JSON_LEGACY, "UTF-8") == STATUS_OK);
        check_event(p, JE_OBJECT_START);
            check_event(p, JE_PROPERTY, "version"); check_event(p, 1.0);
            check_event(p, JE_PROPERTY, "array"); check_event(p, JE_ARRAY_START);
                check_event(p, JE_STRING, "string");
                check_event(p, true);
                check_event(p, false);
                check_event(p, JE_ARRAY_START);
                    check_event(p, 1234);
                    check_event(p, JE_STRING, "value");
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
        UTEST_ASSERT(p.get_next(&ev) == STATUS_EOF);
    }

    UTEST_MAIN
    {
        printf("Testing valid json read...\n");
        test_read_valid_json();
    }

UTEST_END






