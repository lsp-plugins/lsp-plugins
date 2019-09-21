/*
 * expression.cpp
 *
 *  Created on: 19 сент. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <core/units.h>
#include <core/calc/Expression.h>
#include <core/calc/Variables.h>

using namespace lsp;
using namespace lsp::calc;

UTEST_BEGIN("core.calc", expression)

    void test_float(const char *expr, Resolver *r, double value, float tol = 0.001)
    {
        LSPString tmp;
        Expression e(r);
        value_t res;

        printf("Evaluating expression: %s\n", expr);
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_FLOAT);
        UTEST_ASSERT_MSG(float_equals_relative(res.v_float, value, tol),
                "%s: result (%f) != expected (%f)", expr, double(res.v_float), value);
    }

    void test_int(const char *expr, Resolver *r, ssize_t value)
    {
        LSPString tmp;
        Expression e(r);
        value_t res;

        printf("Evaluating expression: %s\n", expr);
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_INT);
        UTEST_ASSERT_MSG(res.v_int == value,
                "%s: result (%ld) != expected (%ld)", expr, long(res.v_int), long(value));
    }

    void init_vars(Variables &v)
    {
        UTEST_ASSERT(v.set_int("ia", 1) == STATUS_OK);
        UTEST_ASSERT(v.set_int("ib", 3) == STATUS_OK);
        UTEST_ASSERT(v.set_int("ic", 5) == STATUS_OK);
        UTEST_ASSERT(v.set_int("id", 7) == STATUS_OK);
        UTEST_ASSERT(v.set_int("ie", 10) == STATUS_OK);
    }

    UTEST_MAIN
    {
        Variables v;
        init_vars(v);

//        test_float("12 db", &v, GAIN_AMP_P_12_DB);
//        test_float("-12 db", &v, GAIN_AMP_M_12_DB);
        test_float("db 12", &v, GAIN_AMP_P_12_DB);
        test_float("db -12", &v, GAIN_AMP_M_12_DB);

        test_int("(:ia+:ic) idiv :ib", &v, 2);
    }

UTEST_END;


