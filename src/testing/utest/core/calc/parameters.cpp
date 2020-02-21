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
        OK(p.add_string("3", "string0"));
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
        OK(p.add_string(&k, "stringA"));
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
        OK(p.add_string("3", "string0"));
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

        UTEST_ASSERT(vv.iv == 456);
        UTEST_ASSERT(vv.fv == 220.0);
        UTEST_ASSERT(vv.sv.equals_ascii("string0"));
        UTEST_ASSERT(vv.sv2.equals_ascii("string1"));
        UTEST_ASSERT(vv.bv == true);
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
        UTEST_ASSERT(k.set_utf8("5"));
        OK(p.insert_string(3, &k, &tmp));
        UTEST_ASSERT(k.set_utf8("6"));
        OK(p.insert_bool(4, &k, false));
        UTEST_ASSERT(k.set_utf8("7"));
        OK(p.insert_null(5, &k));
        UTEST_ASSERT(k.set_utf8("8"));
        OK(p.insert_undef(6, &k));
        v.type      = VT_BOOL;
        v.v_int     = true;
        OK(p.insert(7, &k, &v));

        UTEST_ASSERT(p.size() == 24);

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

        UTEST_ASSERT(vv.iv == 789);
        UTEST_ASSERT(vv.fv == 880.0);
        UTEST_ASSERT(vv.sv.equals_ascii("testA"));
        UTEST_ASSERT(vv.sv2.equals_ascii("testB"));
        UTEST_ASSERT(vv.bv == false);
        UTEST_ASSERT(vv.xv.type == VT_BOOL);
        UTEST_ASSERT(vv.xv.v_int == true);
    }

    UTEST_MAIN
    {
        printf("Testing add functions...\n");
        test_add();

        printf("Testing insert functions...\n");
        test_add();
    }

UTEST_END





