/*
 * tokenizer.cpp
 *
 *  Created on: 17 сен. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <core/calc/token.h>
#include <core/calc/Tokenizer.h>
#include <core/io/InStringSequence.h>

using namespace lsp;
using namespace lsp::calc;

UTEST_BEGIN("core.calc", tokenizer)

    void ck_token(Tokenizer &t, const char *s, token_t token)
    {
        printf("  checking token: %s\n", s);
        token_t tok = t.get_token(TF_GET);
        UTEST_ASSERT_MSG(tok == token, "Error testing token: %s", s);
        UTEST_ASSERT_MSG(t.text_value()->equals_ascii(s), "Error testing token: %s", s);
    }

    void ck_int(Tokenizer &t, ssize_t value)
    {
        printf("  checking integer: %d\n", int(value));
        token_t tok = t.get_token(TF_GET);
        UTEST_ASSERT_MSG(tok == TT_IVALUE, "Error testing token: not IVALUE");
        UTEST_ASSERT_MSG(t.int_value() == value, "Error testing token: %d != %d", int(t.int_value()), int(value));
    }

    void ck_float(Tokenizer &t, double value)
    {
        printf("  checking float: %f\n", double(value));
        token_t tok = t.get_token(TF_GET);
        UTEST_ASSERT_MSG(tok == TT_FVALUE, "Error testing token: not FVALUE");
        UTEST_ASSERT_MSG(float_equals_relative(t.float_value(), value), "Error testing token: %f != %f", t.float_value(), value);
    }

    void ck_invalid(const char *s, token_t token)
    {
        printf("  checking invalid token: %s\n", s);

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(s, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);
        token_t tok = t.get_token(TF_GET);
        UTEST_ASSERT(tok == token);
    }

    void test_sign_tokens()
    {
        static const char *tokens =
                "( ) [ ] { } & && | || ! ^ ^^ ~ + - * ** / % < > <= >= != <> = == <=> ? : ;";

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "(", TT_LBRACE);
        ck_token(t, ")", TT_RBRACE);
        ck_token(t, "[", TT_LQBRACE);
        ck_token(t, "]", TT_RQBRACE);
        ck_token(t, "{", TT_LCBRACE);
        ck_token(t, "}", TT_RCBRACE);
        ck_token(t, "&", TT_AND);
        ck_token(t, "&&", TT_AND);
        ck_token(t, "|", TT_OR);
        ck_token(t, "||", TT_OR);
        ck_token(t, "!", TT_NOT);
        ck_token(t, "^", TT_XOR);
        ck_token(t, "^^", TT_XOR);
        ck_token(t, "~", TT_BNOT);
        ck_token(t, "+", TT_ADD);
        ck_token(t, "-", TT_SUB);
        ck_token(t, "*", TT_MUL);
        ck_token(t, "**", TT_POW);
        ck_token(t, "/", TT_DIV);
        ck_token(t, "%", TT_IMOD);
        ck_token(t, "<", TT_LESS);
        ck_token(t, ">", TT_GREATER);
        ck_token(t, "<=", TT_LESS_EQ);
        ck_token(t, ">=", TT_GREATER_EQ);
        ck_token(t, "!=", TT_NOT_EQ);
        ck_token(t, "<>", TT_NOT_EQ);
        ck_token(t, "=", TT_EQ);
        ck_token(t, "==", TT_EQ);
        ck_token(t, "<=>", TT_CMP);
        ck_token(t, "?", TT_QUESTION);
        ck_token(t, ":", TT_COLON);
        ck_token(t, ";", TT_SEMICOLON);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_text_tokens()
    {
        static const char *tokens =
                "true false null undef "
                "and or not xor "
                "band bor bnot bxor "
                "add sub mul pow div fmod "
                "iadd isub imul idiv imod mod "
                "sc scat sr srep sl slen uc supr lc slwr srev "
                "lt nge gt nle le ngt ge nlt ne eq cmp icmp "
                "ilt inge igt inle ile ingt ige inlt ine ie ieq "
                "int float fp bool str "
                "ex db "
                "bareword "
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "true", TT_TRUE);
        ck_token(t, "false", TT_FALSE);
        ck_token(t, "null", TT_NULL);
        ck_token(t, "undef", TT_UNDEF);

        ck_token(t, "and", TT_AND);
        ck_token(t, "or", TT_OR);
        ck_token(t, "not", TT_NOT);
        ck_token(t, "xor", TT_XOR);

        ck_token(t, "band", TT_BAND);
        ck_token(t, "bor", TT_BOR);
        ck_token(t, "bnot", TT_BNOT);
        ck_token(t, "bxor", TT_BXOR);

        ck_token(t, "add", TT_ADDSYM);
        ck_token(t, "sub", TT_SUBSYM);
        ck_token(t, "mul", TT_MUL);
        ck_token(t, "pow", TT_POW);
        ck_token(t, "div", TT_DIV);
        ck_token(t, "fmod", TT_FMOD);

        ck_token(t, "iadd", TT_IADD);
        ck_token(t, "isub", TT_ISUB);
        ck_token(t, "imul", TT_IMUL);
        ck_token(t, "idiv", TT_IDIV);
        ck_token(t, "imod", TT_IMOD);
        ck_token(t, "mod", TT_IMOD);

        ck_token(t, "sc", TT_SCAT);
        ck_token(t, "scat", TT_SCAT);
        ck_token(t, "sr", TT_SREP);
        ck_token(t, "srep", TT_SREP);
        ck_token(t, "sl", TT_SLEN);
        ck_token(t, "slen", TT_SLEN);
        ck_token(t, "uc", TT_SUPR);
        ck_token(t, "supr", TT_SUPR);
        ck_token(t, "lc", TT_SLWR);
        ck_token(t, "slwr", TT_SLWR);
        ck_token(t, "srev", TT_SREV);

        ck_token(t, "lt", TT_LESS);
        ck_token(t, "nge", TT_LESS);
        ck_token(t, "gt", TT_GREATER);
        ck_token(t, "nle", TT_GREATER);
        ck_token(t, "le", TT_LESS_EQ);
        ck_token(t, "ngt", TT_LESS_EQ);
        ck_token(t, "ge", TT_GREATER_EQ);
        ck_token(t, "nlt", TT_GREATER_EQ);
        ck_token(t, "ne", TT_NOT_EQ);
        ck_token(t, "eq", TT_EQ);
        ck_token(t, "cmp", TT_CMP);
        ck_token(t, "icmp", TT_ICMP);

        ck_token(t, "ilt", TT_ILESS);
        ck_token(t, "inge", TT_ILESS);
        ck_token(t, "igt", TT_IGREATER);
        ck_token(t, "inle", TT_IGREATER);
        ck_token(t, "ile", TT_ILESS_EQ);
        ck_token(t, "ingt", TT_ILESS_EQ);
        ck_token(t, "ige", TT_IGREATER_EQ);
        ck_token(t, "inlt", TT_IGREATER_EQ);
        ck_token(t, "ine", TT_INOT_EQ);
        ck_token(t, "ie", TT_IEQ);
        ck_token(t, "ieq", TT_IEQ);

        ck_token(t, "int", TT_INT);
        ck_token(t, "float", TT_FLOAT);
        ck_token(t, "fp", TT_FLOAT);
        ck_token(t, "bool", TT_BOOL);
        ck_token(t, "str", TT_STR);

        ck_token(t, "ex", TT_EX);
        ck_token(t, "db", TT_DB);
        ck_token(t, "bareword", TT_BAREWORD);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_identifier_tokens()
    {
        static const char *tokens =
                ": :_ :A :abc :aBc :abc123 :i123:xyz"
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, ":", TT_COLON);
        ck_token(t, "_", TT_IDENTIFIER);
        ck_token(t, "A", TT_IDENTIFIER);
        ck_token(t, "abc", TT_IDENTIFIER);
        ck_token(t, "aBc", TT_IDENTIFIER);
        ck_token(t, "abc123", TT_IDENTIFIER);
        ck_token(t, "i123", TT_IDENTIFIER);
        ck_token(t, "xyz", TT_IDENTIFIER);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_numeric_tokens()
    {
        static const char *tokens =
                " "
                "+ - 0 0.0 +1 -1 +1.0 -1.0 +1.123 -1.321 "
                "0b0101 0b0101.0 0b0101.11 "
                "0o17 0o7.0 0o7.7 "
                "0d129 0d9.0 0d9.9 "
                "0x1f 0x1f.0 0x1f.1 "
                "1.e 2.0e .3e 4.5e 6.7e1 8.9e+1 1.0e-1 .2e+1 .3e-1 "
                "0b101.0e-10 -0b101.0e+10 "
                "0x5.0p-2 -0x5.0p+2 "
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "+", TT_ADD);
        ck_token(t, "-", TT_SUB);
        ck_int(t, 0);
        ck_float(t, 0.0);
        ck_int(t, +1);
        ck_int(t, -1);
        ck_float(t, +1.0);
        ck_float(t, -1.0);
        ck_float(t, +1.123);
        ck_float(t, -1.321);

        ck_int(t, 5);
        ck_float(t, 5.0);
        ck_float(t, 5.75);

        ck_int(t, 15);
        ck_float(t, 7.0);
        ck_float(t, 7.875);

        ck_int(t, 129);
        ck_float(t, 9.0);
        ck_float(t, 9.9);

        ck_int(t, 0x1f);
        ck_float(t, 31.0);
        ck_float(t, 31.0625);

        ck_float(t, 1.0);
        ck_float(t, 2.0);
        ck_float(t, 0.3);
        ck_float(t, 4.5);
        ck_float(t, 67.0);
        ck_float(t, 89.0);
        ck_float(t, 0.1);
        ck_float(t, 2.0);
        ck_float(t, 0.03);

        ck_float(t, 1.25);
        ck_float(t, -20.0);

        ck_float(t, 0.01953125);
        ck_float(t, -1280.0);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_string_tokens()
    {
        static const char *tokens =
                "'' 1 '\\n' 2 '\\r\\t' 3 "
                "'123 ' '456 ' '789' 4 'end \\protector'5 '\\\':xc\\\''";
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "", TT_STRING);
        ck_int(t, 1);
        ck_token(t, "\n", TT_STRING);
        ck_int(t, 2);
        ck_token(t, "\r\t", TT_STRING);
        ck_int(t, 3);
        ck_token(t, "123 456 789", TT_STRING);
        ck_int(t, 4);
        ck_token(t, "end \\protector", TT_STRING);
        ck_int(t, 5);
        ck_token(t, "':xc'", TT_STRING);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_invalid_tokens()
    {
        ck_invalid(".", TT_UNKNOWN);
        ck_invalid(".e+", TT_UNKNOWN);
        ck_invalid("\'", TT_ERROR);
        ck_invalid("\'\' \'", TT_ERROR);
    }

    void test_expression_tokens()
    {
        static const char *tokens =
            "((:a eq :b) or (:a eq :c+:d[ssel])) * 10 + (:b ine :c) ? 1 db : 2.0 db";

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens, "UTF-8") == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "(", TT_LBRACE);
            ck_token(t, "(", TT_LBRACE);
                ck_token(t, "a", TT_IDENTIFIER);
                ck_token(t, "eq", TT_EQ);
                ck_token(t, "b", TT_IDENTIFIER);
            ck_token(t, ")", TT_RBRACE);

            ck_token(t, "or", TT_OR);

            ck_token(t, "(", TT_LBRACE);
                ck_token(t, "a", TT_IDENTIFIER);
                ck_token(t, "eq", TT_EQ);
                ck_token(t, "c", TT_IDENTIFIER);
                ck_token(t, "+", TT_ADD);
                ck_token(t, "d", TT_IDENTIFIER);
                ck_token(t, "[", TT_LQBRACE);
                    ck_token(t, "ssel", TT_BAREWORD);
                ck_token(t, "]", TT_RQBRACE);
            ck_token(t, ")", TT_RBRACE);
        ck_token(t, ")", TT_RBRACE);

        ck_token(t, "*", TT_MUL);
        ck_int(t, 10);
        ck_token(t, "+", TT_ADD);

        ck_token(t, "(", TT_LBRACE);
            ck_token(t, "b", TT_IDENTIFIER);
            ck_token(t, "ine", TT_INOT_EQ);
            ck_token(t, "c", TT_IDENTIFIER);
        ck_token(t, ")", TT_RBRACE);

        ck_token(t, "?", TT_QUESTION);
        ck_int(t, 1);
        ck_token(t, "db", TT_DB);
        ck_token(t, ":", TT_COLON);
        ck_float(t, 2.0);
        ck_token(t, "db", TT_DB);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    UTEST_MAIN
    {
        printf("Testing basic sign tokens...\n");
        test_sign_tokens();
        printf("Testing basic text tokens...\n");
        test_text_tokens();
        printf("Testing identifier tokens...\n");
        test_identifier_tokens();
        printf("Testing numeric tokens...\n");
        test_numeric_tokens();
        printf("Testing string tokens...\n");
        test_string_tokens();
        printf("Testing invalid tokens...\n");
        test_invalid_tokens();

        printf("Testing expression tokens...\n");
        test_expression_tokens();
    }

UTEST_END


