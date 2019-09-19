/*
 * expression.cpp
 *
 *  Created on: 19 сент. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <core/calc/Expression.h>

using namespace lsp;
using namespace lsp::calc;

UTEST_BEGIN("core.calc", expression)

    void test_int(const char *expr, Resolver *r, ssize_t value)
    {
        LSPString tmp;
        Expression e(r);
        value_t res;

        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_INT);
        UTEST_ASSERT(res.v_int == value);
    }

    UTEST_MAIN
    {
//        test_int("(:a+:b) idiv %c", );
    }

UTEST_END;


