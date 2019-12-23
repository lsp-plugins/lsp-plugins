/*
 * tokenizer.cpp
 *
 *  Created on: 15 окт. 2019 г.
 *      Author: sadko
 */

#include <locale.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <core/stdlib/math.h>
#include <core/io/InStringSequence.h>
#include <core/files/json/token.h>
#include <core/files/json/Tokenizer.h>

using namespace lsp;
using namespace lsp::json;

UTEST_BEGIN("core.files.json", tokenizer)

    void ck_token(Tokenizer &t, const char *s, token_t token)
    {
        printf("  checking token: %s\n", s);
        token_t tok = t.get_token(true);
        UTEST_ASSERT_MSG(tok == token, "Error testing token: %s", s);
        LSPString tmp;
        UTEST_ASSERT(tmp.set_utf8(s));
        UTEST_ASSERT_MSG(t.text_value()->equals(&tmp), "Error testing token: %s", s);
    }

    void ck_int(Tokenizer &t, ssize_t value)
    {
        printf("  checking integer: %d\n", int(value));
        token_t tok = t.get_token(true);
        UTEST_ASSERT_MSG((tok == JT_DECIMAL) || (tok == JT_HEXADECIMAL), "Error testing token: not decimal/hexadecimal value");
        UTEST_ASSERT_MSG(t.int_value() == value, "Error testing token: %d != %d", int(t.int_value()), int(value));
    }

    void ck_float(Tokenizer &t, double value)
    {
        printf("  checking float: %f\n", double(value));
        token_t tok = t.get_token(true);
        UTEST_ASSERT_MSG(tok == JT_DOUBLE, "Error testing token: not double value");
        UTEST_ASSERT_MSG(float_equals_relative(t.float_value(), value), "Error testing token: %f != %f", t.float_value(), value);
    }

    void ck_invalid(const char *s, token_t token)
    {
        printf("  checking invalid token: %s\n", s);

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(s) == STATUS_OK);

        Tokenizer t(&sq);
        token_t tok = t.get_token(true);
        UTEST_ASSERT(tok == token);
    }

    void test_basic_tokens()
    {
        static const char *tokens =
                " [ ] { } : , 'sq_string' \"dq_string\" "
                " identifier true false null break "
                " Infinity NaN "
                " //comment\n\r /*Multiline\n\rcomment*/ ";

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);
        Tokenizer t(&sq);

        ck_token(t, "[", JT_LQ_BRACE);
        ck_token(t, "]", JT_RQ_BRACE);
        ck_token(t, "{", JT_LC_BRACE);
        ck_token(t, "}", JT_RC_BRACE);
        ck_token(t, ":", JT_COLON);
        ck_token(t, ",", JT_COMMA);
        ck_token(t, "sq_string", JT_SQ_STRING);
        ck_token(t, "dq_string", JT_DQ_STRING);
        ck_token(t, "identifier", JT_IDENTIFIER);
        ck_token(t, "true", JT_TRUE);
        ck_token(t, "false", JT_FALSE);
        ck_token(t, "null", JT_NULL);
        ck_token(t, "break", JT_RESERVED);
        ck_token(t, "Infinity", JT_DOUBLE);
        ck_token(t, "NaN", JT_DOUBLE);
        ck_token(t, "comment", JT_SL_COMMENT);
        ck_token(t, "Multiline\ncomment", JT_ML_COMMENT);

        UTEST_ASSERT(t.get_token(true) == JT_EOF);
    }

    void test_string_tokens()
    {
        static const char *tokens =
            "'Lorem ipsum dolor sit amet, \\\n\rconsectetur adipiscing elit.'\n\r"
            "'\\n\\t\\r\\f\"\\\"\\v\\b\\f'\n\r"
            "'\\x20\\X41\\u000a\\U000D\\u042F\\uD83C\\uDFBC'\n\r"
            "'\\x20\\X41\\u000a\\U000D\\u042F\\uD83C\\uDFBCx'\n\r"
            "\"\\A\\C/\\D\\C\"\n\r"
            "\"\\0\"\n\r";

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);
        Tokenizer t(&sq);

        ck_token(t, "Lorem ipsum dolor sit amet, consectetur adipiscing elit.", JT_SQ_STRING);
        ck_token(t, "\n\t\r\f\"\"\v\b\f", JT_SQ_STRING);
        ck_token(t, " A\n\rЯ\xF0\x9F\x8E\xBC", JT_SQ_STRING);
        ck_token(t, " A\n\rЯ\xF0\x9F\x8E\xBCx", JT_SQ_STRING);
        ck_token(t, "AC/DC", JT_DQ_STRING);

        UTEST_ASSERT(t.get_token(true) == JT_DQ_STRING);
        UTEST_ASSERT(t.text_value()->length() == 1);
        UTEST_ASSERT(t.text_value()->char_at(0) == '\0');

        UTEST_ASSERT(t.get_token(true) == JT_EOF);
    }

    void test_identifier_tokens()
    {
        static const char *tokens =
            "Identifier_001\n\r"
            //"Идентификатор_001\n\r"
            "Identifier\u0041\u0042\n\r"
            "Identifier\u0041\u0042C\n\r"
            "$valid_identifier$\n\r"
            "\\u042F8";

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);
        Tokenizer t(&sq);

        char *locale = ::setlocale(LC_ALL, NULL);
        ::setlocale(LC_ALL, "ru_RU.UTF-8");
        ck_token(t, "Identifier_001", JT_IDENTIFIER);
        //ck_token(t, "Идентификатор_001", JT_IDENTIFIER); // Not valid on systems that do not support russian locales
        ck_token(t, "IdentifierAB", JT_IDENTIFIER);
        ck_token(t, "IdentifierABC", JT_IDENTIFIER);
        ck_token(t, "$valid_identifier$", JT_IDENTIFIER);
        ck_token(t, "Я8", JT_IDENTIFIER);

        UTEST_ASSERT(t.get_token(true) == JT_EOF);

        ::setlocale(LC_ALL, locale);
    }

    void test_numeric_tokens()
    {
        static const char *tokens =
            "0\n\r"
            "10\n\r"
            "+20\n\r"
            "-20\n\r"
            "0x10\n\r"
            "+0x20\n\r"
            "-0x20\n\r"
            "123.\n\r"
            ".123\n\r"
            "12.34\n\r"
            "12.34E+1\n\r"
            "12.34e-1\n\r"
            "1e-1\n\r"
            "2e+1\n\r"
            "+Infinity\n\r"
            "-Infinity\n\r"
            "+NaN\n\r"
            "-NaN\n\r"
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);
        Tokenizer t(&sq);

        ck_int(t, 0);
        ck_int(t, 10);
        ck_int(t, +20);
        ck_int(t, -20);
        ck_int(t, 0x10);
        ck_int(t, 0x20);
        ck_int(t, -0x20);
        ck_float(t, 123.0);
        ck_float(t, 0.123);
        ck_float(t, 12.34);
        ck_float(t, 12.34E+1);
        ck_float(t, 12.34E-1);
        ck_float(t, 1E-1);
        ck_float(t, 2E+1);

        UTEST_ASSERT(t.get_token(true) == JT_DOUBLE);
        UTEST_ASSERT(isinf(t.float_value()) && (t.float_value() > 0.0));
        UTEST_ASSERT(t.get_token(true) == JT_DOUBLE);
        UTEST_ASSERT(isinf(t.float_value()) && (t.float_value() < 0.0));
        UTEST_ASSERT(t.get_token(true) == JT_DOUBLE);
        UTEST_ASSERT(isnan(t.float_value()));
        UTEST_ASSERT(t.get_token(true) == JT_DOUBLE);
        UTEST_ASSERT(isnan(t.float_value()));

        UTEST_ASSERT(t.get_token(true) == JT_EOF);
    }

    void test_unicode_comments()
    {
        static const char *tokens =
            "// \u0041\u0042\n\r"
            "/* \u0041\u0042 */\n\r"
            "//\\uD83C\\uDFBC\n\r"
            "/*\\uD83C\\uDFBC*/\n\r"
            "//\\uD83C\\uDFBCX\n\r"
            "/*\\uD83C\\uDFBCX*/\n\r"
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);
        Tokenizer t(&sq);
        ck_token(t, " AB", JT_SL_COMMENT);
        ck_token(t, " AB ", JT_ML_COMMENT);
        ck_token(t, "\xF0\x9F\x8E\xBC", JT_SL_COMMENT);
        ck_token(t, "\xF0\x9F\x8E\xBC", JT_ML_COMMENT);
        ck_token(t, "\xF0\x9F\x8E\xBCX", JT_SL_COMMENT);
        ck_token(t, "\xF0\x9F\x8E\xBCX", JT_ML_COMMENT);
    }

    void test_invalid_tokens()
    {
        ck_invalid("\"", JT_ERROR);
        ck_invalid("\'", JT_ERROR);
        ck_invalid("\\", JT_ERROR);
        ck_invalid("\"\\\"", JT_ERROR);
        ck_invalid("'\\x1'", JT_ERROR);
        ck_invalid("'\\u1'", JT_ERROR);
        ck_invalid("'\\u12'", JT_ERROR);
        ck_invalid("'\\u123'", JT_ERROR);
        ck_invalid("\\u123", JT_ERROR);
        ck_invalid("\\u12", JT_ERROR);
        ck_invalid("'\n\r'", JT_ERROR);
        ck_invalid("+ 1", JT_UNKNOWN);
        ck_invalid("- 1", JT_UNKNOWN);
        ck_invalid(".", JT_UNKNOWN);
        ck_invalid("12identifier", JT_ERROR);
        ck_invalid("/* test comment", JT_ERROR);
    }

    UTEST_MAIN
    {
        printf("Testing basic tokens...\n");
        test_basic_tokens();
        printf("Testing string tokens...\n");
        test_string_tokens();
        printf("Testing identifier tokens...\n");
        test_identifier_tokens();
        printf("Testing numeric tokens...\n");
        test_numeric_tokens();
        printf("Testing unicode comments...\n");
        test_unicode_comments();
        printf("Testing invalid tokens...\n");
        test_invalid_tokens();
    }

UTEST_END
