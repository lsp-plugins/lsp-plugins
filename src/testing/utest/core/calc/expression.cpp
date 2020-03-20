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
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> %f\n", expr, value);
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_FLOAT);
        UTEST_ASSERT_MSG(float_equals_relative(res.v_float, value, tol),
                "%s: result (%f) != expected (%f)", expr, double(res.v_float), value);
        destroy_value(&res);
    }

    void test_int(const char *expr, Resolver *r, ssize_t value)
    {
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> %ld\n", expr, long(value));
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_INT);
        UTEST_ASSERT_MSG(res.v_int == value,
                "%s: result (%ld) != expected (%ld)", expr, long(res.v_int), long(value));
        destroy_value(&res);
    }

    void test_bool(const char *expr, Resolver *r, bool value)
    {
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> %s\n", expr, (value) ? "true" : "false");
        UTEST_ASSERT_MSG(e.parse(expr, NULL, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_BOOL);
        UTEST_ASSERT_MSG(res.v_bool == value,
                "%s: result (%s) != expected (%s)", expr, (res.v_bool) ? "true" : "false", (value) ? "true" : "false");
        destroy_value(&res);
    }

    void test_string(const char *expr, Resolver *r, const char *value)
    {
        LSPString tmp;
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> '%s'\n", expr, value);
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(&tmp, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_STRING);
        UTEST_ASSERT(tmp.set_utf8(value) == true);
        UTEST_ASSERT_MSG(tmp.equals(res.v_str),
                "%s: result ('%s') != expected ('%s')", expr, res.v_str->get_utf8(), tmp.get_utf8());
        destroy_value(&res);
    }

    void test_substitution(const char *expr, Resolver *r, const char *value)
    {
        LSPString tmp;
        Expression e(r);
        value_t res;
        init_value(&res);

        printf("Evaluating expression: %s -> '%s'\n", expr, value);
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(&tmp, Expression::FLAG_STRING) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.evaluate(&res) == STATUS_OK);
        UTEST_ASSERT(cast_string(&res) == STATUS_OK);
        UTEST_ASSERT(res.type == VT_STRING);
        UTEST_ASSERT(tmp.set_utf8(value) == true);
        UTEST_ASSERT_MSG(tmp.equals(res.v_str),
                "%s: result ('%s') != expected ('%s')", expr, res.v_str->get_utf8(), tmp.get_utf8());
        destroy_value(&res);
    }

    void test_dependencies(Resolver *r)
    {
        LSPString tmp;
        Expression e(r);

        static const char *expr = "(:v[:fa][:ia-:fd]) && (:v[1][:bc] = 'test') || (:za + :zb == undef)";

        printf("Testing dependencies for expression\n");
        UTEST_ASSERT(tmp.set_utf8(expr) == true);
        UTEST_ASSERT_MSG(e.parse(&tmp, Expression::FLAG_NONE) == STATUS_OK, "Error parsing expression: %s", expr);
        UTEST_ASSERT(e.has_dependency("v"));
        UTEST_ASSERT(e.has_dependency("fa"));
        UTEST_ASSERT(e.has_dependency("ia"));
        UTEST_ASSERT(e.has_dependency("fd"));
        UTEST_ASSERT(e.has_dependency("bc"));
        UTEST_ASSERT(e.has_dependency("za"));
        UTEST_ASSERT(e.has_dependency("zb"));
        UTEST_ASSERT(!e.has_dependency("zc"));
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
        UTEST_ASSERT(v.set_float("fg", 14.1) == STATUS_OK);
        UTEST_ASSERT(v.set_float("zoom1", GAIN_AMP_M_12_DB) == STATUS_OK);
        UTEST_ASSERT(v.set_float("zoom2", GAIN_AMP_P_12_DB) == STATUS_OK);

        UTEST_ASSERT(v.set_null("za") == STATUS_OK);
        UTEST_ASSERT(v.set_null("zb") == STATUS_OK);

        UTEST_ASSERT(v.set_int("v_0_0", 1234) == STATUS_OK);
        UTEST_ASSERT(v.set_float("v_0_1", 1.234) == STATUS_OK);
        UTEST_ASSERT(v.set_bool("v_1_0", true) == STATUS_OK);
        UTEST_ASSERT(v.set_string("v_1_1", "test") == STATUS_OK);

        LSPString lower, upper;
        UTEST_ASSERT(lower.set_ascii("lower"));
        UTEST_ASSERT(upper.set_ascii("UPPER"));

        UTEST_ASSERT(v.set_string("sa", &lower) == STATUS_OK);
        UTEST_ASSERT(v.set_string("sb", &upper) == STATUS_OK);

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
        test_float("fp (:ie + :id)", &v, 17.0);

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
        test_int("undef <=> :za", &v, -1);
        test_int("int :ba + int :fg", &v, 15);

        test_bool(":zoom1 le -9 db", &v, true);
        test_bool(":zoom2 le -9 db", &v, false);
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
        test_bool("bool :fb", &v, false);

        for (size_t i=0, j=0; i<40; i += 10, ++j)
        {
            UTEST_ASSERT(v.set_int("x", i) == STATUS_OK);
            test_int(":x < 20 ? :x < 10 ? 0 : 1 : :x < 30 ? 2 : 3", &v, j);
        }

        test_int("slen 'abcdef'", &v, 6);
        test_string("'ABC'", &v, "ABC");
        test_string("'1' sc 20+:ib sc :ic*9", &v, "12345");
        test_string("'xy' sr :id", &v, "xyxyxyxyxyxyxy");
        test_string("lc :sa sc uc :sb", &v, "lowerUPPER");
        test_string("uc :sa sc lc :sb", &v, "LOWERupper");
        test_string("srev :sa sc srev :sb", &v, "rewolREPPU");
        test_string("'null: ' sc :za sc ', undef: ' sc :zx", &v, "null: null, undef: undef");
        test_string("str :bc", &v, "true");

        test_substitution("some bare string", &v, "some bare string");
        test_substitution("${ia}", &v, "1");
        test_substitution("Value is: ${ia}", &v, "Value is: 1");
        test_substitution("Value is: ${:ba}", &v, "Value is: true");
        test_substitution("$${ia}", &v, "${ia}");
        test_substitution("${ia}+${:ie}-${:ic}=${:ia+:ie-:ic}", &v, "1+10-5=6");

        test_dependencies(&v);
    }

UTEST_END;


