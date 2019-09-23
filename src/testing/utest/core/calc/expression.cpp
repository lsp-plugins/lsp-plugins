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

        printf("Evaluating expression: %s -> %f\n", expr, value);
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

        printf("Evaluating expression: %s -> %ld\n", expr, long(value));
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_INT);
        UTEST_ASSERT_MSG(res.v_int == value,
                "%s: result (%ld) != expected (%ld)", expr, long(res.v_int), long(value));
    }

    void test_bool(const char *expr, Resolver *r, bool value)
    {
        LSPString tmp;
        Expression e(r);
        value_t res;

        printf("Evaluating expression: %s -> %s\n", expr, (value) ? "true" : "false");
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_BOOL);
        UTEST_ASSERT_MSG(res.v_bool == value,
                "%s: result (%s) != expected (%s)", expr, (res.v_bool) ? "true" : "false", (value) ? "true" : "false");
    }

    void init_vars(Variables &v)
    {
        UTEST_ASSERT(v.set_int("ia", 1) == STATUS_OK);
        UTEST_ASSERT(v.set_int("ib", 3) == STATUS_OK);
        UTEST_ASSERT(v.set_int("ic", 5) == STATUS_OK);
        UTEST_ASSERT(v.set_int("id", 7) == STATUS_OK);
        UTEST_ASSERT(v.set_int("ie", 10) == STATUS_OK);

        UTEST_ASSERT(v.set_bool("ba", true) == STATUS_OK);
        UTEST_ASSERT(v.set_bool("bb", false) == STATUS_OK);
        UTEST_ASSERT(v.set_bool("bc", true) == STATUS_OK);
        UTEST_ASSERT(v.set_bool("bd", false) == STATUS_OK);

        UTEST_ASSERT(v.set_float("fa", 1) == STATUS_OK);
        UTEST_ASSERT(v.set_float("fb", 0.3) == STATUS_OK);
        UTEST_ASSERT(v.set_float("fc", 0.5) == STATUS_OK);
        UTEST_ASSERT(v.set_float("fd", 0.7) == STATUS_OK);
        UTEST_ASSERT(v.set_float("fe", 0.01) == STATUS_OK);

        UTEST_ASSERT(v.set_null("za") == STATUS_OK);
        UTEST_ASSERT(v.set_null("zb") == STATUS_OK);

        UTEST_ASSERT(v.set_int("v_0_0", 1234) == STATUS_OK);
        UTEST_ASSERT(v.set_float("v_0_1", 1.234) == STATUS_OK);
        UTEST_ASSERT(v.set_bool("v_1_0", true) == STATUS_OK);
        UTEST_ASSERT(v.set_string("v_1_1", "test") == STATUS_OK);
    }

    UTEST_MAIN
    {
        Variables v;
        init_vars(v);

        test_float("12 db", &v, GAIN_AMP_P_12_DB);
        test_float("-12 db", &v, GAIN_AMP_M_12_DB);
        test_float("db 12", &v, GAIN_AMP_P_12_DB);
        test_float("db -12", &v, GAIN_AMP_M_12_DB);
        test_float(":fa + :fb/:fc - :fe", &v, 1.59);
        test_float(":ic ** :ib", &v, 125.0f);

        test_int("0b1011_0010", &v, 0xb2);
        test_int("0o1_1", &v, 9);
        test_int("0d12_34", &v, 1234);
        test_int("0x12_34", &v, 0x1234);
        test_int("+6 + -3 - --2", &v, 1);
        test_int("(:ia+:ic) idiv :ib", &v, 2);
        test_int(":ia * :ic + :id idiv :ib", &v, 7);
        test_int(":ie bxor 0x3", &v, 9);
        test_int(":ie bor :ic", &v, 15);
        test_int(":ie band 0xC", &v, 8);
        test_int("~:ia band 0xf", &v, 0x0e);
        test_int(":ia icmp null", &v, 1);
        test_int("null cmp :ia", &v, -1);
        test_int("null <=> undef", &v, 1);
        test_int("undef <=> :za", &v, -1);

        test_bool(":ia*:ib < :fc / :fe", &v, true);
        test_bool(":ia*:ic > :fa / :fb", &v, true);
        test_bool(":bb || :ba && :bd ^^ :bc", &v, true);
        test_bool("(:bb || :bd) || !(:ba eq :bc)", &v, false);
        test_bool("-:ia + :ib - :ic ieq -(:ie - :id)", &v, true);
        test_bool("'true' eq true", &v, true);
        test_bool("'TRUE' ne true", &v, true);
        test_bool("'TRUE' ieq true", &v, true);
        test_bool("'0x100' ieq 0x100", &v, true);
        test_bool("'0x100' != 0x100", &v, true);
        test_bool("ex :ia", &v, true);
        test_bool("ex :fz", &v, false);
        test_bool(":za ieq :zb", &v, true);
        test_bool(":ia >= :za", &v, true);
        test_bool(":ia + :za == undef", &v, true);
        test_bool("(:v[0][0] ieq 1234) and (:v[bb][ia] = 1.234)", &v, true);
        test_bool("(:v[:fa][:ia-:fd]) && (:v[1][:bc] = 'test')", &v, true);
    }

UTEST_END;


