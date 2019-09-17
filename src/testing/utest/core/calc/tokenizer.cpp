/*
 * tokenizer.cpp
 *
 *  Created on: 17 сен. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
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

    void test_sign_tokens()
    {
        static const char *tokens =
                "( ) [ ] & && | || ! ^ ^^ ~ + - * ** / % < > <= >= != <> = == <=> ? : ;";

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens) == STATUS_OK);

        Tokenizer t(&sq);

        ck_token(t, "(", TT_LBRACE);
        ck_token(t, ")", TT_RBRACE);
        ck_token(t, "[", TT_LQBRACE);
        ck_token(t, "]", TT_RQBRACE);
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
                "lt nge gt nle le ngt ge nlt ne eq cmp "
                "ilt inge igt inle ile ingt ige inlt ine ie ieq "
                "ex db "
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens) == STATUS_OK);

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

        ck_token(t, "add", TT_ADD);
        ck_token(t, "sub", TT_SUB);
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

        ck_token(t, "ex", TT_EX);
        ck_token(t, "db", TT_DB);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }

    void test_identifier_tokens()
    {
        static const char *tokens =
                ": :_ :A :abc :aBc :abc123 :i123:xyz"
            ;

        io::InStringSequence sq;
        UTEST_ASSERT(sq.wrap(tokens) == STATUS_OK);

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


    UTEST_MAIN
    {
        printf("Testing basic sign tokens...\n");
        test_sign_tokens();
        printf("Testing basic text tokens...\n");
        test_text_tokens();
        printf("Testing identifier tokens...\n");
        test_identifier_tokens();
    }

UTEST_END


