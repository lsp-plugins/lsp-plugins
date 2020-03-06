/*
 * parameters.cpp
 *
 *  Created on: 21 февр. 2020 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <core/calc/Parameters.h>

using namespace lsp;
using namespace lsp::calc;

UTEST_BEGIN("core.calc", parameters)
    #define OK(expr) UTEST_ASSERT(expr == STATUS_OK);
    #define FC(a, b) UTEST_ASSERT(float_equals_relative(a, b));
    #define EC(a, b) UTEST_ASSERT(a == b);

    typedef struct values_t
    {
        ssize_t     iv;
        double      fv;
        bool        bv;
        LSPString   sv;
        LSPString   sv2;
        value_t     xv;
    } values_t;

    void test_add()
    {
        Parameters p;
        value_t v;

        LSPString tmp, k;

        // Append named parameters
        OK(p.add_int("1", 123));
        OK(p.add_float("2", 440.0));
        OK(p.add_cstring("3", "string0"));
        UTEST_ASSERT(tmp.set_utf8("string1"));
        OK(p.add_string("4", &tmp));
        OK(p.add_bool("5", true));
        OK(p.add_null("6"));
        OK(p.add_undef("7"));
        v.type      = VT_INT;
        v.v_int     = 42;
        OK(p.add("8", &v));

        UTEST_ASSERT(p.size() == 8);

        // Append named parameters(2)
        UTEST_ASSERT(k.set_utf8("1"));
        OK(p.add_int(&k, 456));
        UTEST_ASSERT(k.set_utf8("2"));
        OK(p.add_float(&k, 880.0));
        UTEST_ASSERT(k.set_utf8("3"));
        OK(p.add_cstring(&k, "stringA"));
        UTEST_ASSERT(tmp.set_utf8("stringB"));
        UTEST_ASSERT(k.set_utf8("4"));
        OK(p.add_string(&k, &tmp));
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.add_bool(&k, false));
        UTEST_ASSERT(k.set_utf8("6"));
        OK(p.add_null(&k));
        UTEST_ASSERT(k.set_utf8("7"));
        OK(p.add_undef(&k));
        UTEST_ASSERT(k.set_utf8("8"));
        v.type      = VT_FLOAT;
        v.v_int     = 42.0;
        OK(p.add(&k, &v));

        UTEST_ASSERT(p.size() == 16);

        // Append anonymous parameters
        OK(p.add_int(789));
        OK(p.add_float(220.0));
        OK(p.add_cstring("test0"));
        UTEST_ASSERT(tmp.set_utf8("test1"));
        OK(p.add_string(&tmp));
        OK(p.add_bool(true));
        OK(p.add_null());
        OK(p.add_undef());
        v.type      = VT_BOOL;
        v.v_int     = true;
        OK(p.add(&v));

        UTEST_ASSERT(p.size() == 24);

        // Validate
        values_t vv;
        init_value(&vv.xv);
        size_t i=0;

        // Part 1 check
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = false;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        OK(p.get_int(i++, &vv.iv));
        OK(p.get_float(i++, &vv.fv));
        OK(p.get_string(i++, &vv.sv));
        OK(p.get_string(i++, &vv.sv2));
        OK(p.get_bool(i++, &vv.bv));
        OK(p.get_null(i++));
        OK(p.get_undef(i++));
        OK(p.get(i++, &vv.xv));

        UTEST_ASSERT(vv.iv == 123);
        UTEST_ASSERT(vv.fv == 440.0);
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));
        UTEST_ASSERT(vv.sv2.equals_ascii("string1"));
        UTEST_ASSERT(vv.bv == true);
        UTEST_ASSERT(vv.xv.type == VT_INT);
        UTEST_ASSERT(vv.xv.v_int == 42);

        // Part 2 check
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = true;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        OK(p.get_int(i++, &vv.iv));
        OK(p.get_float(i++, &vv.fv));
        OK(p.get_string(i++, &vv.sv));
        OK(p.get_string(i++, &vv.sv2));
        OK(p.get_bool(i++, &vv.bv));
        OK(p.get_null(i++));
        OK(p.get_undef(i++));
        OK(p.get(i++, &vv.xv));

        UTEST_ASSERT(vv.iv == 456);
        UTEST_ASSERT(vv.fv == 880.0);
        UTEST_ASSERT(vv.sv.equals_ascii("stringA"));
        UTEST_ASSERT(vv.sv2.equals_ascii("stringB"));
        UTEST_ASSERT(vv.bv == false);
        UTEST_ASSERT(vv.xv.type == VT_FLOAT);
        UTEST_ASSERT(vv.xv.v_int == 42.0);

        // Part 3
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = false;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        OK(p.get_int(i++, &vv.iv));
        OK(p.get_float(i++, &vv.fv));
        OK(p.get_string(i++, &vv.sv));
        OK(p.get_string(i++, &vv.sv2));
        OK(p.get_bool(i++, &vv.bv));
        OK(p.get_null(i++));
        OK(p.get_undef(i++));
        OK(p.get(i++, &vv.xv));

        UTEST_ASSERT(vv.iv == 789);
        UTEST_ASSERT(vv.fv == 220.0);
        UTEST_ASSERT(vv.sv.equals_ascii("test0"));
        UTEST_ASSERT(vv.sv2.equals_ascii("test1"));
        UTEST_ASSERT(vv.bv == true);
        UTEST_ASSERT(vv.xv.type == VT_BOOL);
        UTEST_ASSERT(vv.xv.v_bool == true);

        // Part 4
        UTEST_ASSERT(p.get_int(i, &vv.iv) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.get_float(i, &vv.fv) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.get_bool(i, &vv.bv) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.get_string(i, &vv.sv) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.get(i, &vv.xv) == STATUS_INVALID_VALUE);

        // Validate fetching parameters by name (part 1)
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = false;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        OK(p.get_int("1", &vv.iv));
        OK(p.get_float("2", &vv.fv));
        OK(p.get_string("3", &vv.sv));
        OK(p.get_string("4", &vv.sv2));
        OK(p.get_bool("5", &vv.bv));
        OK(p.get_null("6"));
        OK(p.get_undef("7"));
        OK(p.get("8", &vv.xv));

        UTEST_ASSERT(vv.iv == 123);
        UTEST_ASSERT(vv.fv == 440.0);
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));
        UTEST_ASSERT(vv.sv2.equals_ascii("string1"));
        UTEST_ASSERT(vv.bv == true);
        UTEST_ASSERT(vv.xv.type == VT_INT);
        UTEST_ASSERT(vv.xv.v_int == 42);

        UTEST_ASSERT(p.get_int("9", &vv.iv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get_float("9", &vv.fv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get_bool("9", &vv.bv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get_string("9", &vv.sv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get("9", &vv.xv) == STATUS_NOT_FOUND);

        // Validate fetching parameters by name (part 2)
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = false;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        UTEST_ASSERT(k.set_utf8("1"));
        OK(p.get_int(&k, &vv.iv));
        UTEST_ASSERT(k.set_utf8("2"));
        OK(p.get_float(&k, &vv.fv));
        UTEST_ASSERT(k.set_utf8("3"));
        OK(p.get_string(&k, &vv.sv));
        UTEST_ASSERT(k.set_utf8("4"));
        OK(p.get_string(&k, &vv.sv2));
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.get_bool(&k, &vv.bv));
        UTEST_ASSERT(k.set_utf8("6"));
        OK(p.get_null(&k));
        UTEST_ASSERT(k.set_utf8("7"));
        OK(p.get_undef(&k));
        UTEST_ASSERT(k.set_utf8("8"));
        OK(p.get(&k, &vv.xv));

        UTEST_ASSERT(vv.iv == 123);
        UTEST_ASSERT(vv.fv == 440.0);
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));
        UTEST_ASSERT(vv.sv2.equals_ascii("string1"));
        UTEST_ASSERT(vv.bv == true);
        UTEST_ASSERT(vv.xv.type == VT_INT);
        UTEST_ASSERT(vv.xv.v_int == 42);

        UTEST_ASSERT(k.set_utf8("9"));
        UTEST_ASSERT(p.get_int(&k, &vv.iv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get_float(&k, &vv.fv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get_bool(&k, &vv.bv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get_string(&k, &vv.sv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get(&k, &vv.xv) == STATUS_NOT_FOUND);

        destroy_value(&vv.xv);
    }

    void test_insert()
    {
        Parameters p;
        value_t v;
        LSPString tmp, k;
        values_t vv;
        init_value(&vv.xv);

        // Append named parameters
        OK(p.add_int("1", 123));
        OK(p.add_float("2", 440.0));
        OK(p.add_cstring("3", "string0"));
        UTEST_ASSERT(tmp.set_utf8("string1"));
        OK(p.add_string("4", &tmp));
        OK(p.add_bool("5", true));
        OK(p.add_null("6"));
        OK(p.add_undef("7"));
        v.type      = VT_INT;
        v.v_int     = 42;
        OK(p.add("8", &v));

        UTEST_ASSERT(p.size() == 8);

        // Insert named parameters
        OK(p.insert_int(0, "1", 456));
        OK(p.insert_float(1, "2", 220.0));
        OK(p.insert_string(2, "3", "stringA"));
        UTEST_ASSERT(tmp.set_utf8("stringB"));
        OK(p.insert_string(3, "4", &tmp));
        OK(p.insert_bool(4, "5", false));
        OK(p.insert_null(5, "6"));
        OK(p.insert_undef(6, "7"));
        v.type      = VT_FLOAT;
        v.v_int     = 42.0;
        OK(p.insert(7, "8", &v));

        UTEST_ASSERT(p.size() == 16);

        // Check parameter types
        // Validate fetching parameters by name (part 1)
        vv.iv = 0;
        vv.bv = true;
        vv.fv = 0.0;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        OK(p.get_int("1", &vv.iv));
        OK(p.get_float("2", &vv.fv));
        OK(p.get_string("3", &vv.sv));
        OK(p.get_string("4", &vv.sv2));
        OK(p.get_bool("5", &vv.bv));
        OK(p.get_null("6"));
        OK(p.get_undef("7"));
        OK(p.get("8", &vv.xv));

        UTEST_ASSERT(vv.iv == 456);
        UTEST_ASSERT(vv.fv == 220.0);
        UTEST_ASSERT(vv.sv.equals_ascii("stringA"));
        UTEST_ASSERT(vv.sv2.equals_ascii("stringB"));
        UTEST_ASSERT(vv.bv == false);
        UTEST_ASSERT(vv.xv.type == VT_FLOAT);
        UTEST_ASSERT(vv.xv.v_int == 42.0);

        // Insert more named parameters
        UTEST_ASSERT(k.set_utf8("1"));
        OK(p.insert_int(0, &k, 789));
        UTEST_ASSERT(k.set_utf8("2"));
        OK(p.insert_float(1, &k, 880.0));
        UTEST_ASSERT(k.set_utf8("3"));
        OK(p.insert_string(2, &k, "testA"));
        UTEST_ASSERT(k.set_utf8("4"));
        UTEST_ASSERT(tmp.set_utf8("testB"));
        OK(p.insert_string(3, &k, &tmp));
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.insert_bool(4, &k, true));
        UTEST_ASSERT(k.set_utf8("6"));
        OK(p.insert_null(5, &k));
        UTEST_ASSERT(k.set_utf8("7"));
        OK(p.insert_undef(6, &k));
        v.type      = VT_BOOL;
        v.v_bool    = true;
        UTEST_ASSERT(k.set_utf8("8"));
        OK(p.insert(7, &k, &v));

        UTEST_ASSERT(p.size() == 24);

        // Check parameter types
        // Validate fetching parameters by name (part 1)
        vv.iv = 0;
        vv.bv = false;
        vv.fv = 0.0;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        OK(p.get_int("1", &vv.iv));
        OK(p.get_float("2", &vv.fv));
        OK(p.get_string("3", &vv.sv));
        OK(p.get_string("4", &vv.sv2));
        OK(p.get_bool("5", &vv.bv));
        OK(p.get_null("6"));
        OK(p.get_undef("7"));
        OK(p.get("8", &vv.xv));

        UTEST_ASSERT(vv.iv == 789);
        UTEST_ASSERT(vv.fv == 880.0);
        UTEST_ASSERT(vv.sv.equals_ascii("testA"));
        UTEST_ASSERT(vv.sv2.equals_ascii("testB"));
        UTEST_ASSERT(vv.bv == true);
        UTEST_ASSERT(vv.xv.type == VT_BOOL);
        UTEST_ASSERT(vv.xv.v_int == true);

        // Test invalid inserts
        UTEST_ASSERT(p.insert_int(100, 789) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.insert_float(100, 1.0) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.insert_cstring(100, "abc") == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.insert_string(100, &tmp) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.insert_null(100) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.insert_undef(100) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.insert(100, &v) == STATUS_INVALID_VALUE);

    }

    void test_cast()
    {
        Parameters p;
        value_t v;
        LSPString tmp, k;
        values_t vv;
        init_value(&vv.xv);

        // Append named parameters
        OK(p.add_int("1", 123));
        OK(p.add_float("2", 440.0));
        OK(p.add_cstring("3", "string0"));
        UTEST_ASSERT(tmp.set_utf8("456"));
        OK(p.add_string("4", &tmp));
        OK(p.add_bool("5", true));
        OK(p.add_null("6"));
        OK(p.add_undef("7"));
        v.type      = VT_INT;
        v.v_int     = 42;
        OK(p.add("8", &v));

        UTEST_ASSERT(p.size() == 8);

        // Check casts to int
        OK(p.as_int(size_t(0), &v.v_int));
        UTEST_ASSERT(v.v_int == 123);
        OK(p.as_int(1, &v.v_int));
        UTEST_ASSERT(v.v_int == 440);
        UTEST_ASSERT(p.as_int(2, &v.v_int) == STATUS_BAD_TYPE);
        OK(p.as_int(3, &v.v_int));
        UTEST_ASSERT(v.v_int == 456);
        OK(p.as_int(4, &v.v_int));
        UTEST_ASSERT(v.v_int == 1);
        UTEST_ASSERT(p.as_int(5, &v.v_int) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.as_int(6, &v.v_int) == STATUS_BAD_TYPE);
        OK(p.as_value(7, &v, VT_INT));
        UTEST_ASSERT(v.type == VT_INT);
        UTEST_ASSERT(v.v_int == 42);
        UTEST_ASSERT(p.as_int(8, &v.v_int) == STATUS_INVALID_VALUE);

        OK(p.as_int("1", &v.v_int));
        UTEST_ASSERT(v.v_int == 123);
        OK(p.as_int("2", &v.v_int));
        UTEST_ASSERT(v.v_int == 440);
        UTEST_ASSERT(p.as_int("3", &v.v_int) == STATUS_BAD_TYPE);
        OK(p.as_int("4", &v.v_int));
        UTEST_ASSERT(v.v_int == 456);
        OK(p.as_int("5", &v.v_int));
        UTEST_ASSERT(v.v_int == 1);
        UTEST_ASSERT(p.as_int("6", &v.v_int) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.as_int("7", &v.v_int) == STATUS_BAD_TYPE);
        OK(p.as_value("8", &v, VT_INT));
        UTEST_ASSERT(v.type == VT_INT);
        UTEST_ASSERT(v.v_int == 42);
        UTEST_ASSERT(p.as_int("9", &v.v_int) == STATUS_NOT_FOUND);

        UTEST_ASSERT(k.set_utf8("1"));
        OK(p.as_int(&k, &v.v_int));
        UTEST_ASSERT(v.v_int == 123);
        UTEST_ASSERT(k.set_utf8("2"));
        OK(p.as_int(&k, &v.v_int));
        UTEST_ASSERT(v.v_int == 440);
        UTEST_ASSERT(k.set_utf8("3"));
        UTEST_ASSERT(p.as_int(&k, &v.v_int) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("4"));
        OK(p.as_int(&k, &v.v_int));
        UTEST_ASSERT(v.v_int == 456);
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.as_int(&k, &v.v_int));
        UTEST_ASSERT(v.v_int == 1);
        UTEST_ASSERT(k.set_utf8("6"));
        UTEST_ASSERT(p.as_int(&k, &v.v_int) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("7"));
        UTEST_ASSERT(p.as_int(&k, &v.v_int) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("8"));
        OK(p.as_value(&k, &v, VT_INT));
        UTEST_ASSERT(v.type == VT_INT);
        UTEST_ASSERT(v.v_int == 42);
        UTEST_ASSERT(k.set_utf8("9"));
        UTEST_ASSERT(p.as_int(&k, &v.v_int) == STATUS_NOT_FOUND);

        // Check casts to float
        OK(p.as_float(size_t(0), &v.v_float));
        FC(v.v_float, 123.0);
        OK(p.as_float(1, &v.v_float));
        FC(v.v_float, 440.0);
        UTEST_ASSERT(p.as_float(2, &v.v_float) == STATUS_BAD_TYPE);
        OK(p.as_float(3, &v.v_float));
        FC(v.v_float, 456.0);
        OK(p.as_float(4, &v.v_float));
        FC(v.v_float, 1.0);
        UTEST_ASSERT(p.as_float(5, &v.v_float) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.as_float(6, &v.v_float) == STATUS_BAD_TYPE);
        OK(p.as_value(7, &v, VT_FLOAT));
        UTEST_ASSERT(v.type == VT_FLOAT);
        FC(v.v_float, 42.0);
        UTEST_ASSERT(p.as_float(8, &v.v_float) == STATUS_INVALID_VALUE);

        OK(p.as_float("1", &v.v_float));
        FC(v.v_float, 123.0);
        OK(p.as_float("2", &v.v_float));
        FC(v.v_float, 440.0);
        UTEST_ASSERT(p.as_float("3", &v.v_float) == STATUS_BAD_TYPE);
        OK(p.as_float("4", &v.v_float));
        FC(v.v_float, 456.0);
        OK(p.as_float("5", &v.v_float));
        FC(v.v_float, 1.0);
        UTEST_ASSERT(p.as_float("6", &v.v_float) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.as_float("7", &v.v_float) == STATUS_BAD_TYPE);
        OK(p.as_value("8", &v, VT_FLOAT));
        UTEST_ASSERT(v.type == VT_FLOAT);
        FC(v.v_float, 42.0);
        UTEST_ASSERT(p.as_float("9", &v.v_float) == STATUS_NOT_FOUND);

        UTEST_ASSERT(k.set_utf8("1"));
        OK(p.as_float(&k, &v.v_float));
        FC(v.v_float, 123.0);
        UTEST_ASSERT(k.set_utf8("2"));
        OK(p.as_float(&k, &v.v_float));
        FC(v.v_float, 440.0);
        UTEST_ASSERT(k.set_utf8("3"));
        UTEST_ASSERT(p.as_float(&k, &v.v_float) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("4"));
        OK(p.as_float(&k, &v.v_float));
        FC(v.v_float, 456.0);
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.as_float(&k, &v.v_float));
        FC(v.v_float, 1.0);
        UTEST_ASSERT(k.set_utf8("6"));
        UTEST_ASSERT(p.as_float(&k, &v.v_float) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("7"));
        UTEST_ASSERT(p.as_float(&k, &v.v_float) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("8"));
        OK(p.as_value(&k, &v, VT_FLOAT));
        UTEST_ASSERT(v.type == VT_FLOAT);
        FC(v.v_float, 42.0);
        UTEST_ASSERT(k.set_utf8("9"));
        UTEST_ASSERT(p.as_float(&k, &v.v_float) == STATUS_NOT_FOUND);

        // Check casts to bool
        OK(p.as_bool(size_t(0), &v.v_bool));
        EC(v.v_bool, true);
        OK(p.as_bool(1, &v.v_bool));
        EC(v.v_bool, true);
        UTEST_ASSERT(p.as_bool(2, &v.v_bool) == STATUS_BAD_TYPE);
        OK(p.as_bool(3, &v.v_bool));
        EC(v.v_bool, true);
        OK(p.as_bool(4, &v.v_bool));
        EC(v.v_bool, true);
        UTEST_ASSERT(p.as_bool(5, &v.v_bool) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.as_bool(6, &v.v_bool) == STATUS_BAD_TYPE);
        OK(p.as_value(7, &v, VT_BOOL));
        UTEST_ASSERT(v.type == VT_BOOL);
        EC(v.v_bool, true);
        UTEST_ASSERT(p.as_bool(8, &v.v_bool) == STATUS_INVALID_VALUE);

        OK(p.as_bool("1", &v.v_bool));
        EC(v.v_bool, true);
        OK(p.as_bool("2", &v.v_bool));
        EC(v.v_bool, true);
        UTEST_ASSERT(p.as_bool("3", &v.v_bool) == STATUS_BAD_TYPE);
        OK(p.as_bool("4", &v.v_bool));
        EC(v.v_bool, true);
        OK(p.as_bool("5", &v.v_bool));
        EC(v.v_bool, true);
        UTEST_ASSERT(p.as_bool("6", &v.v_bool) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.as_bool("7", &v.v_bool) == STATUS_BAD_TYPE);
        OK(p.as_value("8", &v, VT_BOOL));
        UTEST_ASSERT(v.type == VT_BOOL);
        EC(v.v_bool, true);
        UTEST_ASSERT(p.as_bool("9", &v.v_bool) == STATUS_NOT_FOUND);

        UTEST_ASSERT(k.set_utf8("1"));
        OK(p.as_bool(&k, &v.v_bool));
        EC(v.v_bool, true);
        UTEST_ASSERT(k.set_utf8("2"));
        OK(p.as_bool(&k, &v.v_bool));
        EC(v.v_bool, true);
        UTEST_ASSERT(k.set_utf8("3"));
        UTEST_ASSERT(p.as_bool(&k, &v.v_bool) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("4"));
        OK(p.as_bool(&k, &v.v_bool));
        EC(v.v_bool, true);
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.as_bool(&k, &v.v_bool));
        EC(v.v_bool, true);
        UTEST_ASSERT(k.set_utf8("6"));
        UTEST_ASSERT(p.as_bool(&k, &v.v_bool) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("7"));
        UTEST_ASSERT(p.as_bool(&k, &v.v_bool) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("8"));
        OK(p.as_value(&k, &v, VT_BOOL));
        UTEST_ASSERT(v.type == VT_BOOL);
        EC(v.v_bool, true);
        UTEST_ASSERT(k.set_utf8("9"));
        UTEST_ASSERT(p.as_bool(&k, &v.v_bool) == STATUS_NOT_FOUND);

        // Check casts to string
        OK(p.as_string(size_t(0), &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("123"));
        OK(p.as_string(1, &vv.sv));
        UTEST_ASSERT(vv.sv.starts_with_ascii("440."));
        OK(p.as_string(2, &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));
        OK(p.as_string(3, &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("456"));
        OK(p.as_string(4, &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("true"));
        UTEST_ASSERT(p.as_string(5, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.as_string(6, &vv.sv) == STATUS_BAD_TYPE);
        OK(p.as_value(7, &vv.xv, VT_STRING));
        UTEST_ASSERT(vv.xv.type == VT_STRING);
        UTEST_ASSERT(vv.xv.v_str->equals_ascii("42"));
        UTEST_ASSERT(p.as_string(8, &vv.sv) == STATUS_INVALID_VALUE);

        OK(p.as_string("1", &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("123"));
        OK(p.as_string("2", &vv.sv));
        UTEST_ASSERT(vv.sv.starts_with_ascii("440."));
        OK(p.as_string("3", &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));
        OK(p.as_string("4", &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("456"));
        OK(p.as_string("5", &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("true"));
        UTEST_ASSERT(p.as_string("6", &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.as_string("7", &vv.sv) == STATUS_BAD_TYPE);
        OK(p.as_value("8", &vv.xv, VT_STRING));
        UTEST_ASSERT(vv.xv.type == VT_STRING);
        UTEST_ASSERT(vv.xv.v_str->equals_ascii("42"));
        UTEST_ASSERT(p.as_string("9", &vv.sv) == STATUS_NOT_FOUND);

        UTEST_ASSERT(k.set_utf8("1"));
        OK(p.as_string(&k, &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("123"));
        UTEST_ASSERT(k.set_utf8("2"));
        OK(p.as_string(&k, &vv.sv));
        UTEST_ASSERT(vv.sv.starts_with_ascii("440."));
        UTEST_ASSERT(k.set_utf8("3"));
        OK(p.as_string(&k, &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));
        UTEST_ASSERT(k.set_utf8("4"));
        OK(p.as_string(&k, &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("456"));
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.as_string(&k, &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("true"));
        UTEST_ASSERT(k.set_utf8("6"));
        UTEST_ASSERT(p.as_string(&k, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("7"));
        UTEST_ASSERT(p.as_string(&k, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(k.set_utf8("8"));
        OK(p.as_value(&k, &vv.xv, VT_STRING));
        UTEST_ASSERT(vv.xv.type == VT_STRING);
        UTEST_ASSERT(vv.xv.v_str->equals_ascii("42"));
        UTEST_ASSERT(k.set_utf8("9"));
        UTEST_ASSERT(p.as_string(&k, &vv.sv) == STATUS_NOT_FOUND);

        // Check casts to null
        OK(p.as_null(size_t(0)));
        OK(p.as_null(1));
        OK(p.as_null(2));
        OK(p.as_null(3));
        OK(p.as_null(4));
        OK(p.as_null(5));
        OK(p.as_null(6));
        OK(p.as_value(7, &vv.xv, VT_NULL));
        UTEST_ASSERT(vv.xv.type == VT_NULL);
        UTEST_ASSERT(p.as_null(8) == STATUS_INVALID_VALUE);

        OK(p.as_null("1"));
        OK(p.as_null("2"));
        OK(p.as_null("3"));
        OK(p.as_null("4"));
        OK(p.as_null("5"));
        OK(p.as_null("6"));
        OK(p.as_null("7"));
        OK(p.as_value("8", &vv.xv, VT_NULL));
        UTEST_ASSERT(vv.xv.type == VT_NULL);
        UTEST_ASSERT(p.as_null("9") == STATUS_NOT_FOUND);

        UTEST_ASSERT(k.set_utf8("1"));
        OK(p.as_null(&k));
        UTEST_ASSERT(k.set_utf8("2"));
        OK(p.as_null(&k));
        UTEST_ASSERT(k.set_utf8("3"));
        OK(p.as_null(&k));
        UTEST_ASSERT(k.set_utf8("4"));
        OK(p.as_null(&k));
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.as_null(&k));
        UTEST_ASSERT(k.set_utf8("6"));
        OK(p.as_null(&k));
        UTEST_ASSERT(k.set_utf8("7"));
        OK(p.as_null(&k));
        UTEST_ASSERT(k.set_utf8("8"));
        OK(p.as_value(&k, &vv.xv, VT_NULL));
        UTEST_ASSERT(vv.xv.type == VT_NULL);
        UTEST_ASSERT(k.set_utf8("9"));
        UTEST_ASSERT(p.as_null(&k) == STATUS_NOT_FOUND);

        // Check casts to undef
        OK(p.as_undef(size_t(0)));
        OK(p.as_undef(1));
        OK(p.as_undef(2));
        OK(p.as_undef(3));
        OK(p.as_undef(4));
        OK(p.as_undef(5));
        OK(p.as_undef(6));
        OK(p.as_value(7, &vv.xv, VT_UNDEF));
        UTEST_ASSERT(vv.xv.type == VT_UNDEF);
        UTEST_ASSERT(p.as_undef(8) == STATUS_INVALID_VALUE);

        OK(p.as_undef("1"));
        OK(p.as_undef("2"));
        OK(p.as_undef("3"));
        OK(p.as_undef("4"));
        OK(p.as_undef("5"));
        OK(p.as_undef("6"));
        OK(p.as_undef("7"));
        OK(p.as_value("8", &vv.xv, VT_UNDEF));
        UTEST_ASSERT(vv.xv.type == VT_UNDEF);
        UTEST_ASSERT(p.as_undef("9") == STATUS_NOT_FOUND);

        UTEST_ASSERT(k.set_utf8("1"));
        OK(p.as_undef(&k));
        UTEST_ASSERT(k.set_utf8("2"));
        OK(p.as_undef(&k));
        UTEST_ASSERT(k.set_utf8("3"));
        OK(p.as_undef(&k));
        UTEST_ASSERT(k.set_utf8("4"));
        OK(p.as_undef(&k));
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.as_undef(&k));
        UTEST_ASSERT(k.set_utf8("6"));
        OK(p.as_undef(&k));
        UTEST_ASSERT(k.set_utf8("7"));
        OK(p.as_undef(&k));
        UTEST_ASSERT(k.set_utf8("8"));
        OK(p.as_value(&k, &vv.xv, VT_UNDEF));
        UTEST_ASSERT(vv.xv.type == VT_UNDEF);
        UTEST_ASSERT(k.set_utf8("9"));
        UTEST_ASSERT(p.as_undef(&k) == STATUS_NOT_FOUND);
    }

    void test_set()
    {
        Parameters p;
        value_t v;
        LSPString tmp, k;
        values_t vv;
        init_value(&vv.xv);
        size_t i=0;

        OK(p.add_undef("1"));
        OK(p.add_undef("2"));
        OK(p.add_undef("3"));
        OK(p.add_undef("4"));
        OK(p.add_undef("5"));
        OK(p.add_undef("6"));
        OK(p.add_undef("7"));
        OK(p.add_undef("8"));

        // Test setting by index
        OK(p.set_int(size_t(0), 123));
        OK(p.set_float(1, 220.0));
        OK(p.set_bool(2, true));
        OK(p.set_cstring(3, "string0"));
        UTEST_ASSERT(tmp.set_ascii("string1"));
        OK(p.set_string(4, &tmp));
        OK(p.set_null(5));
        OK(p.set_undef(6));
        v.type = VT_INT;
        v.v_int = 42;
        OK(p.set(7, &v));

        UTEST_ASSERT(p.set_int(8, 456) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.set_float(8, 440.0) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.set_bool(8, false) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.set_cstring(8, "bad") == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.set_string(8, &tmp) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.set_null(8) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.set_undef(8) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.set(8, &v) == STATUS_INVALID_VALUE);

        // Validate values
        i = 0;
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = false;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        OK(p.get_int(i++, &vv.iv));
        OK(p.get_float(i++, &vv.fv));
        OK(p.get_bool(i++, &vv.bv));
        OK(p.get_string(i++, &vv.sv));
        OK(p.get_string(i++, &vv.sv2));
        OK(p.get_null(i++));
        OK(p.get_undef(i++));
        OK(p.get(i++, &vv.xv));

        UTEST_ASSERT(vv.iv == 123);
        UTEST_ASSERT(vv.fv == 220.0);
        UTEST_ASSERT(vv.bv == true);
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));
        UTEST_ASSERT(vv.sv2.equals_ascii("string1"));
        UTEST_ASSERT(vv.xv.type == VT_INT);
        UTEST_ASSERT(vv.xv.v_int == 42);

        // Test setting by name
        OK(p.set_int("1", 456));
        OK(p.set_float("2", 440.0));
        OK(p.set_bool("3", false));
        OK(p.set_cstring("4", "stringA"));
        UTEST_ASSERT(tmp.set_ascii("stringB"));
        OK(p.set_string("5", &tmp));
        OK(p.set_null("6"));
        OK(p.set_undef("7"));
        v.type = VT_FLOAT;
        v.v_float = 42.0;
        OK(p.set("8", &v));

        // Validate values
        i = 0;
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = true;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        OK(p.get_int(i++, &vv.iv));
        OK(p.get_float(i++, &vv.fv));
        OK(p.get_bool(i++, &vv.bv));
        OK(p.get_string(i++, &vv.sv));
        OK(p.get_string(i++, &vv.sv2));
        OK(p.get_null(i++));
        OK(p.get_undef(i++));
        OK(p.get(i++, &vv.xv));

        UTEST_ASSERT(vv.iv == 456);
        UTEST_ASSERT(vv.fv == 440.0);
        UTEST_ASSERT(vv.bv == false);
        UTEST_ASSERT(vv.sv.equals_ascii("stringA"));
        UTEST_ASSERT(vv.sv2.equals_ascii("stringB"));
        UTEST_ASSERT(vv.xv.type == VT_FLOAT);
        UTEST_ASSERT(vv.xv.v_float == 42.0);

        // Test setting non-existing values
        OK(p.set_int("9", 123));
        OK(p.set_float("10", 220.0));
        OK(p.set_bool("11", true));
        OK(p.set_cstring("12", "string0"));
        UTEST_ASSERT(tmp.set_ascii("string1"));
        OK(p.set_string("13", &tmp));
        OK(p.set_null("14"));
        OK(p.set_undef("15"));
        v.type = VT_INT;
        v.v_int = 42;
        OK(p.set("16", &v));

        UTEST_ASSERT(p.size() == 16);
        UTEST_ASSERT(p.get_index("9") == 8);
        UTEST_ASSERT(p.get_index("10") == 9);
        UTEST_ASSERT(p.get_index("11") == 10);
        UTEST_ASSERT(p.get_index("12") == 11);
        UTEST_ASSERT(p.get_index("13") == 12);
        UTEST_ASSERT(p.get_index("14") == 13);
        UTEST_ASSERT(p.get_index("15") == 14);
        UTEST_ASSERT(p.get_index("16") == 15);

        // Test setting by name (2)
        UTEST_ASSERT(k.set_ascii("1"));
        OK(p.set_int(&k, 789));
        UTEST_ASSERT(k.set_ascii("2"));
        OK(p.set_float(&k, 880.0));
        UTEST_ASSERT(k.set_ascii("3"));
        OK(p.set_bool(&k, true));
        UTEST_ASSERT(k.set_ascii("4"));
        OK(p.set_cstring(&k, "testA"));
        UTEST_ASSERT(tmp.set_ascii("testB"));
        UTEST_ASSERT(k.set_ascii("5"));
        OK(p.set_string(&k, &tmp));
        UTEST_ASSERT(k.set_ascii("6"));
        OK(p.set_null(&k));
        UTEST_ASSERT(k.set_ascii("7"));
        OK(p.set_undef(&k));
        v.type = VT_INT;
        v.v_int = 42;
        UTEST_ASSERT(k.set_ascii("8"));
        OK(p.set(&k, &v));

        // Validate values
        i = 0;
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = false;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        OK(p.get_int(i++, &vv.iv));
        OK(p.get_float(i++, &vv.fv));
        OK(p.get_bool(i++, &vv.bv));
        OK(p.get_string(i++, &vv.sv));
        OK(p.get_string(i++, &vv.sv2));
        OK(p.get_null(i++));
        OK(p.get_undef(i++));
        OK(p.get(i++, &vv.xv));

        UTEST_ASSERT(vv.iv == 789);
        UTEST_ASSERT(vv.fv == 880.0);
        UTEST_ASSERT(vv.bv == true);
        UTEST_ASSERT(vv.sv.equals_ascii("testA"));
        UTEST_ASSERT(vv.sv2.equals_ascii("testB"));
        UTEST_ASSERT(vv.xv.type == VT_INT);
        UTEST_ASSERT(vv.xv.v_int == 42);

        // Test setting non-existing values
        UTEST_ASSERT(k.set_ascii("17"));
        OK(p.set_int(&k, 123));
        UTEST_ASSERT(k.set_ascii("18"));
        OK(p.set_float(&k, 220.0));
        UTEST_ASSERT(k.set_ascii("19"));
        OK(p.set_bool(&k, true));
        UTEST_ASSERT(k.set_ascii("20"));
        OK(p.set_cstring(&k, "string0"));
        UTEST_ASSERT(tmp.set_ascii("string1"));
        UTEST_ASSERT(k.set_ascii("21"));
        OK(p.set_string(&k, &tmp));
        UTEST_ASSERT(k.set_ascii("22"));
        OK(p.set_null(&k));
        UTEST_ASSERT(k.set_ascii("23"));
        OK(p.set_undef(&k));
        v.type = VT_INT;
        v.v_int = 42;
        UTEST_ASSERT(k.set_ascii("24"));
        OK(p.set(&k, &v));

        UTEST_ASSERT(p.size() == 24);
        UTEST_ASSERT(k.set_ascii("17"));
        UTEST_ASSERT(p.get_index(&k) == 16);
        UTEST_ASSERT(k.set_ascii("18"));
        UTEST_ASSERT(p.get_index(&k) == 17);
        UTEST_ASSERT(k.set_ascii("19"));
        UTEST_ASSERT(p.get_index(&k) == 18);
        UTEST_ASSERT(k.set_ascii("20"));
        UTEST_ASSERT(p.get_index(&k) == 19);
        UTEST_ASSERT(k.set_ascii("21"));
        UTEST_ASSERT(p.get_index(&k) == 20);
        UTEST_ASSERT(k.set_ascii("22"));
        UTEST_ASSERT(p.get_index(&k) == 21);
        UTEST_ASSERT(k.set_ascii("23"));
        UTEST_ASSERT(p.get_index(&k) == 22);
        UTEST_ASSERT(k.set_ascii("24"));
        UTEST_ASSERT(p.get_index(&k) == 23);
    }

    void test_remove()
    {
        Parameters p;
        value_t v;
        LSPString tmp, k;
        values_t vv;
        init_value(&vv.xv);
        size_t i=0;

        // Append named parameters
        OK(p.add_int("1", 123));
        OK(p.add_float("2", 440.0));
        OK(p.add_cstring("3", "string0"));
        OK(p.add_bool("4", true));
        OK(p.add_null("5"));
        OK(p.add_undef("6"));
        v.type      = VT_INT;
        v.v_int     = 42;
        OK(p.add("7", &v));

        // Remove parameters by index
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = false;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        UTEST_ASSERT(p.remove_float(i, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(i, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(i, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(i, VT_FLOAT, &v) == STATUS_BAD_TYPE);
        OK(p.remove_int(i, &vv.iv));
        UTEST_ASSERT(vv.iv == 123);

        UTEST_ASSERT(p.remove_int(i, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(i, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(i, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(i, VT_INT, &v) == STATUS_BAD_TYPE);
        OK(p.remove_float(i, &vv.fv));
        UTEST_ASSERT(vv.fv == 440.0);

        UTEST_ASSERT(p.remove_int(i, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float(i, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(i, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(i, VT_NULL, &v) == STATUS_BAD_TYPE);
        OK(p.remove_string(i, &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));

        UTEST_ASSERT(p.remove_int(i, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float(i, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(i, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(i, VT_STRING, &v) == STATUS_BAD_TYPE);
        OK(p.remove_bool(i, &vv.bv));
        UTEST_ASSERT(vv.bv == true);

        UTEST_ASSERT(p.remove_int(i, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float(i, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(i, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(i, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(i, VT_UNDEF, &v) == STATUS_BAD_TYPE);
        OK(p.remove_null(i));

        UTEST_ASSERT(p.remove_int(i, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float(i, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(i, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(i, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(i) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(i, VT_BOOL, &v) == STATUS_BAD_TYPE);
        OK(p.remove_undef(i));

        OK(p.remove(i, &vv.xv));
        UTEST_ASSERT(vv.xv.type == VT_INT);
        UTEST_ASSERT(vv.xv.v_int == 42);

        UTEST_ASSERT(p.remove_int(i, &vv.iv) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.remove_float(i, &vv.fv) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.remove_bool(i, &vv.bv) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.remove_string(i, &vv.sv) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.remove_null(i) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.remove_undef(i) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.remove_value(i, VT_STRING, &v) == STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.remove(i, &vv.xv) == STATUS_INVALID_VALUE);

        UTEST_ASSERT(p.size() == 0);

        // Append named parameters
        OK(p.add_int("1", 123));
        OK(p.add_float("2", 440.0));
        OK(p.add_cstring("3", "string0"));
        OK(p.add_bool("4", true));
        OK(p.add_null("5"));
        OK(p.add_undef("6"));
        v.type      = VT_INT;
        v.v_int     = 42;
        OK(p.add("7", &v));

        // Remove parameters by name
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = false;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        UTEST_ASSERT(p.remove_float("1", &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string("1", &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool("1", &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null("1") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef("1") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value("1", VT_FLOAT, &v) == STATUS_BAD_TYPE);
        OK(p.remove_int("1", &vv.iv));
        UTEST_ASSERT(vv.iv == 123);

        UTEST_ASSERT(p.remove_int("2", &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string("2", &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool("2", &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null("2") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef("2") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value("2", VT_INT, &v) == STATUS_BAD_TYPE);
        OK(p.remove_float("2", &vv.fv));
        UTEST_ASSERT(vv.fv == 440.0);

        UTEST_ASSERT(p.remove_int("3", &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float("3", &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool("3", &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null("3") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef("3") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value("3", VT_NULL, &v) == STATUS_BAD_TYPE);
        OK(p.remove_string("3", &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));

        UTEST_ASSERT(p.remove_int("4", &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float("4", &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string("4", &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null("4") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef("4") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value("4", VT_STRING, &v) == STATUS_BAD_TYPE);
        OK(p.remove_bool("4", &vv.bv));
        UTEST_ASSERT(vv.bv == true);

        UTEST_ASSERT(p.remove_int("5", &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float("5", &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string("5", &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool("5", &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef("5") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value("5", VT_UNDEF, &v) == STATUS_BAD_TYPE);
        OK(p.remove_null("5"));

        UTEST_ASSERT(p.remove_int("6", &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float("6", &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string("6", &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool("6", &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null("6") == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value("6", VT_BOOL, &v) == STATUS_BAD_TYPE);
        OK(p.remove_undef("6"));

        OK(p.remove("7", &vv.xv));
        UTEST_ASSERT(vv.xv.type == VT_INT);
        UTEST_ASSERT(vv.xv.v_int == 42);

        UTEST_ASSERT(p.remove_int("8", &vv.iv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_float("8", &vv.fv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_string("8", &vv.sv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_bool("8", &vv.bv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_null("8") == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_undef("8") == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_value("8", VT_BOOL, &v) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove("8", &v) == STATUS_NOT_FOUND);

        UTEST_ASSERT(p.size() == 0);

        // Append named parameters
        OK(p.add_int("1", 123));
        OK(p.add_float("2", 440.0));
        OK(p.add_cstring("3", "string0"));
        OK(p.add_bool("4", true));
        OK(p.add_null("5"));
        OK(p.add_undef("6"));
        v.type      = VT_INT;
        v.v_int     = 42;
        OK(p.add("7", &v));

        // Remove parameters by name (2)
        vv.iv = 0;
        vv.fv = 0.0;
        vv.bv = false;
        vv.sv.clear();
        vv.sv2.clear();
        destroy_value(&vv.xv);

        UTEST_ASSERT(k.set_ascii("1"));
        UTEST_ASSERT(p.remove_float(&k, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(&k, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(&k, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(&k, VT_FLOAT, &v) == STATUS_BAD_TYPE);
        OK(p.remove_int(&k, &vv.iv));
        UTEST_ASSERT(vv.iv == 123);

        UTEST_ASSERT(k.set_ascii("2"));
        UTEST_ASSERT(p.remove_int(&k, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(&k, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(&k, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(&k, VT_INT, &v) == STATUS_BAD_TYPE);
        OK(p.remove_float(&k, &vv.fv));
        UTEST_ASSERT(vv.fv == 440.0);

        UTEST_ASSERT(k.set_ascii("3"));
        UTEST_ASSERT(p.remove_int(&k, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float(&k, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(&k, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(&k, VT_NULL, &v) == STATUS_BAD_TYPE);
        OK(p.remove_string(&k, &vv.sv));
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));

        UTEST_ASSERT(k.set_ascii("4"));
        UTEST_ASSERT(p.remove_int(&k, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float(&k, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(&k, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(&k, VT_STRING, &v) == STATUS_BAD_TYPE);
        OK(p.remove_bool(&k, &vv.bv));
        UTEST_ASSERT(vv.bv == true);

        UTEST_ASSERT(k.set_ascii("5"));
        UTEST_ASSERT(p.remove_int(&k, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float(&k, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(&k, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(&k, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_undef(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(&k, VT_UNDEF, &v) == STATUS_BAD_TYPE);
        OK(p.remove_null(&k));

        UTEST_ASSERT(k.set_ascii("6"));
        UTEST_ASSERT(p.remove_int(&k, &vv.iv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_float(&k, &vv.fv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_string(&k, &vv.sv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_bool(&k, &vv.bv) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_null(&k) == STATUS_BAD_TYPE);
        UTEST_ASSERT(p.remove_value(&k, VT_BOOL, &v) == STATUS_BAD_TYPE);
        OK(p.remove_undef(&k));

        UTEST_ASSERT(k.set_ascii("7"));
        OK(p.remove(&k, &vv.xv));
        UTEST_ASSERT(vv.xv.type == VT_INT);
        UTEST_ASSERT(vv.xv.v_int == 42);

        UTEST_ASSERT(k.set_ascii("8"));
        UTEST_ASSERT(p.remove_int(&k, &vv.iv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_float(&k, &vv.fv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_string(&k, &vv.sv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_bool(&k, &vv.bv) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_null(&k) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_undef(&k) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove_value(&k, VT_BOOL, &v) == STATUS_NOT_FOUND);
        UTEST_ASSERT(p.remove(&k, &v) == STATUS_NOT_FOUND);

        UTEST_ASSERT(p.size() == 0);

        destroy_value(&vv.xv);
    }

    void test_get_type()
    {
        Parameters p;
        LSPString tmp, k;
        values_t vv;
        init_value(&vv.xv);

        // Append named parameters
        OK(p.add_int("1", 123));
        OK(p.add_float("2", 440.0));
        OK(p.add_cstring("3", "string0"));
        OK(p.add_bool("4", true));
        OK(p.add_null("5"));
        OK(p.add_undef("6"));

        // Get names
        OK(p.get_name(0, &k));
        UTEST_ASSERT(p.get_type(size_t(0)) == VT_INT);
        UTEST_ASSERT(k.equals_ascii("1"));
        OK(p.get_name(1, &k));
        UTEST_ASSERT(p.get_type(1) == VT_FLOAT);
        UTEST_ASSERT(k.equals_ascii("2"));
        OK(p.get_name(2, &k));
        UTEST_ASSERT(p.get_type(2) == VT_STRING);
        UTEST_ASSERT(k.equals_ascii("3"));
        OK(p.get_name(3, &k));
        UTEST_ASSERT(p.get_type(3) == VT_BOOL);
        UTEST_ASSERT(k.equals_ascii("4"));
        OK(p.get_name(4, &k));
        UTEST_ASSERT(p.get_type(4) == VT_NULL);
        UTEST_ASSERT(k.equals_ascii("5"));
        OK(p.get_name(5, &k));
        UTEST_ASSERT(p.get_type(5) == VT_UNDEF);
        UTEST_ASSERT(k.equals_ascii("6"));
        UTEST_ASSERT(p.get_type(6) == -STATUS_INVALID_VALUE);
        UTEST_ASSERT(p.get_name(6, &k) == STATUS_INVALID_VALUE);

        // Get indexes
        UTEST_ASSERT(p.get_index("1") == 0);
        UTEST_ASSERT(p.get_type("1") == VT_INT);
        UTEST_ASSERT(p.get_index("2") == 1);
        UTEST_ASSERT(p.get_type("2") == VT_FLOAT);
        UTEST_ASSERT(p.get_index("3") == 2);
        UTEST_ASSERT(p.get_type("3") == VT_STRING);
        UTEST_ASSERT(p.get_index("4") == 3);
        UTEST_ASSERT(p.get_type("4") == VT_BOOL);
        UTEST_ASSERT(p.get_index("5") == 4);
        UTEST_ASSERT(p.get_type("5") == VT_NULL);
        UTEST_ASSERT(p.get_index("6") == 5);
        UTEST_ASSERT(p.get_type("6") == VT_UNDEF);
        UTEST_ASSERT(p.get_index("7") == -STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get_type("7") == -STATUS_NOT_FOUND);

        // Get indexes (LSPString)
        UTEST_ASSERT(k.set_ascii("1"));
        UTEST_ASSERT(p.get_index(&k) == 0);
        UTEST_ASSERT(p.get_type(&k) == VT_INT);
        UTEST_ASSERT(k.set_ascii("2"));
        UTEST_ASSERT(p.get_index(&k) == 1);
        UTEST_ASSERT(p.get_type(&k) == VT_FLOAT);
        UTEST_ASSERT(k.set_ascii("3"));
        UTEST_ASSERT(p.get_index(&k) == 2);
        UTEST_ASSERT(p.get_type(&k) == VT_STRING);
        UTEST_ASSERT(k.set_ascii("4"));
        UTEST_ASSERT(p.get_index(&k) == 3);
        UTEST_ASSERT(p.get_type(&k) == VT_BOOL);
        UTEST_ASSERT(k.set_ascii("5"));
        UTEST_ASSERT(p.get_index(&k) == 4);
        UTEST_ASSERT(p.get_type(&k) == VT_NULL);
        UTEST_ASSERT(k.set_ascii("6"));
        UTEST_ASSERT(p.get_index(&k) == 5);
        UTEST_ASSERT(p.get_type(&k) == VT_UNDEF);
        UTEST_ASSERT(k.set_ascii("7"));
        UTEST_ASSERT(p.get_index(&k) == -STATUS_NOT_FOUND);
        UTEST_ASSERT(p.get_type(&k) == -STATUS_NOT_FOUND);

        destroy_value(&vv.xv);
    }

    void test_set_operations()
    {
        Parameters p;
        OK(p.add_int("1", 123));
        OK(p.add_float("2", 440.0));
        OK(p.add_cstring("3", "string0"));
        OK(p.add_bool("4", true));
        OK(p.add_null("5"));
        OK(p.add_undef("6"));
        UTEST_ASSERT(p.size() == 6);

        // Check clones
        Parameters *tmp = p.clone();
        UTEST_ASSERT(tmp->size() == 6);
        UTEST_ASSERT(tmp->get_type("1") == VT_INT);
        UTEST_ASSERT(tmp->get_type("2") == VT_FLOAT);
        UTEST_ASSERT(tmp->get_type("3") == VT_STRING);
        UTEST_ASSERT(tmp->get_type("4") == VT_BOOL);
        UTEST_ASSERT(tmp->get_type("5") == VT_NULL);
        UTEST_ASSERT(tmp->get_type("6") == VT_UNDEF);

        // Clear
        tmp->clear();
        UTEST_ASSERT(tmp->size() == 0);

        // Add parameters
        OK(tmp->add_int("4", 42));
        OK(tmp->add(&p, 3, 6));
        OK(tmp->insert(0, &p, 0, 3));
        UTEST_ASSERT(tmp->size() == 7);

        UTEST_ASSERT(tmp->get_type("1") == VT_INT);
        UTEST_ASSERT(tmp->get_type("2") == VT_FLOAT);
        UTEST_ASSERT(tmp->get_type("3") == VT_STRING);
        UTEST_ASSERT(tmp->get_type("4") == VT_INT);
        UTEST_ASSERT(tmp->get_type("5") == VT_NULL);
        UTEST_ASSERT(tmp->get_type("6") == VT_UNDEF);

        UTEST_ASSERT(tmp->get_type(size_t(0)) == VT_INT);
        UTEST_ASSERT(tmp->get_type(1) == VT_FLOAT);
        UTEST_ASSERT(tmp->get_type(2) == VT_STRING);
        UTEST_ASSERT(tmp->get_type(3) == VT_INT);
        UTEST_ASSERT(tmp->get_type(4) == VT_BOOL);
        UTEST_ASSERT(tmp->get_type(5) == VT_NULL);
        UTEST_ASSERT(tmp->get_type(6) == VT_UNDEF);

        // Remove parameters
        OK(tmp->remove(2, 5));
        UTEST_ASSERT(tmp->size() == 4);

        UTEST_ASSERT(tmp->get_type(size_t(0)) == VT_INT);
        UTEST_ASSERT(tmp->get_type(1) == VT_FLOAT);
        UTEST_ASSERT(tmp->get_type(2) == VT_NULL);
        UTEST_ASSERT(tmp->get_type(3) == VT_UNDEF);

        // Test swap
        p.swap(tmp);
        UTEST_ASSERT(p.size() == 4);
        UTEST_ASSERT(tmp->size() == 6);

        delete tmp;
    }

    UTEST_MAIN
    {
        printf("Testing add functions...\n");
        test_add();

        printf("Testing insert functions...\n");
        test_insert();

        printf("Testing cast functions...\n");
        test_cast();

        printf("Testing set functions...\n");
        test_set();

        printf("Testing remove functions...\n");
        test_remove();

        printf("Testing get_type functions...\n");
        test_get_type();

        printf("Testing functions for manipulating set of parameters...\n");
        test_set_operations();
    }

UTEST_END





