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

    void ck_basic(Tokenizer &t, const char *s, token_t token)
    {
        printf("Checking token: %s\n", s);
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

        ck_basic(t, "(", TT_LBRACE);
        ck_basic(t, ")", TT_RBRACE);
        ck_basic(t, "[", TT_LQBRACE);
        ck_basic(t, "]", TT_RQBRACE);
        ck_basic(t, "&", TT_AND);
        ck_basic(t, "&&", TT_AND);
        ck_basic(t, "|", TT_OR);
        ck_basic(t, "||", TT_OR);
        ck_basic(t, "!", TT_NOT);
        ck_basic(t, "^", TT_XOR);
        ck_basic(t, "^^", TT_XOR);
        ck_basic(t, "~", TT_BNOT);
        ck_basic(t, "+", TT_ADD);
        ck_basic(t, "-", TT_SUB);
        ck_basic(t, "*", TT_MUL);
        ck_basic(t, "**", TT_POW);
        ck_basic(t, "/", TT_DIV);
        ck_basic(t, "%", TT_IMOD);
        ck_basic(t, "<", TT_LESS);
        ck_basic(t, ">", TT_GREATER);
        ck_basic(t, "<=", TT_LESS_EQ);
        ck_basic(t, ">=", TT_GREATER_EQ);
        ck_basic(t, "!=", TT_NOT_EQ);
        ck_basic(t, "<>", TT_NOT_EQ);
        ck_basic(t, "=", TT_EQ);
        ck_basic(t, "==", TT_EQ);
        ck_basic(t, "<=>", TT_CMP);
        ck_basic(t, "?", TT_QUESTION);
        ck_basic(t, ":", TT_COLON);
        ck_basic(t, ";", TT_SEMICOLON);

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

        ck_basic(t, "true", TT_TRUE);
        ck_basic(t, "false", TT_FALSE);
        ck_basic(t, "null", TT_NULL);
        ck_basic(t, "undef", TT_UNDEF);

        ck_basic(t, "and", TT_AND);
        ck_basic(t, "or", TT_OR);
        ck_basic(t, "not", TT_NOT);
        ck_basic(t, "xor", TT_XOR);

        ck_basic(t, "band", TT_BAND);
        ck_basic(t, "bor", TT_BOR);
        ck_basic(t, "bnot", TT_BNOT);
        ck_basic(t, "bxor", TT_BXOR);

        ck_basic(t, "add", TT_ADD);
        ck_basic(t, "sub", TT_SUB);
        ck_basic(t, "mul", TT_MUL);
        ck_basic(t, "pow", TT_POW);
        ck_basic(t, "div", TT_DIV);
        ck_basic(t, "fmod", TT_FMOD);

        ck_basic(t, "iadd", TT_IADD);
        ck_basic(t, "isub", TT_ISUB);
        ck_basic(t, "imul", TT_IMUL);
        ck_basic(t, "idiv", TT_IDIV);
        ck_basic(t, "imod", TT_IMOD);
        ck_basic(t, "mod", TT_IMOD);

        ck_basic(t, "lt", TT_LESS);
        ck_basic(t, "nge", TT_LESS);
        ck_basic(t, "gt", TT_GREATER);
        ck_basic(t, "nle", TT_GREATER);
        ck_basic(t, "le", TT_LESS_EQ);
        ck_basic(t, "ngt", TT_LESS_EQ);
        ck_basic(t, "ge", TT_GREATER_EQ);
        ck_basic(t, "nlt", TT_GREATER_EQ);
        ck_basic(t, "ne", TT_NOT_EQ);
        ck_basic(t, "eq", TT_EQ);
        ck_basic(t, "cmp", TT_CMP);

        ck_basic(t, "ilt", TT_ILESS);
        ck_basic(t, "inge", TT_ILESS);
        ck_basic(t, "igt", TT_IGREATER);
        ck_basic(t, "inle", TT_IGREATER);
        ck_basic(t, "ile", TT_ILESS_EQ);
        ck_basic(t, "ingt", TT_ILESS_EQ);
        ck_basic(t, "ige", TT_IGREATER_EQ);
        ck_basic(t, "inlt", TT_IGREATER_EQ);
        ck_basic(t, "ine", TT_INOT_EQ);
        ck_basic(t, "ie", TT_IEQ);
        ck_basic(t, "ieq", TT_IEQ);

        ck_basic(t, "ex", TT_EX);
        ck_basic(t, "db", TT_DB);

        UTEST_ASSERT(t.get_token(TF_GET) == TT_EOF);
    }


    UTEST_MAIN
    {
        printf("Testing basic sign tokens...\n");
        test_sign_tokens();
        printf("Testing basic text tokens...\n");
        test_text_tokens();
    }

UTEST_END


