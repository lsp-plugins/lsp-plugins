/*
 * json_dictionary.cpp
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/resource.h>
#include <core/i18n/JsonDictionary.h>

using namespace lsp;

UTEST_BEGIN("core.i18n", json_dictionary)

    void ck_att(IDictionary *d, size_t index, const char *name, const char *value)
    {
        LSPString k, v;
        printf("  fetching %s -> %s...\n", name, value);
        UTEST_ASSERT(d->get_value(index, &k, &v) == STATUS_OK);
        printf("  fetched %s -> %s...\n", k.get_utf8(), v.get_utf8());
        UTEST_ASSERT(k.equals_utf8(name));
        UTEST_ASSERT(v.equals_utf8(value));
    }

    void ck_child(IDictionary *d, size_t index, const char *name)
    {
        LSPString k;
        IDictionary *c = NULL;

        printf("  fetching child: -> %s...\n", name);
        UTEST_ASSERT(d->get_child(index, &k, &c) == STATUS_OK);
        UTEST_ASSERT(k.equals_utf8(name));

        ck_att(c, 0, "a1", "x1");
        ck_att(c, 1, "a2", "x2");
        ck_att(c, 2, "a3", "x3");
    }

    void ck_lookup(IDictionary *d, const char *name, const char *value)
    {
        LSPString v;
        printf("  lookup %s ...\n", name, value);
        UTEST_ASSERT(d->lookup(name, &v) == STATUS_OK);
        printf("  got %s -> %s...\n", name, v.get_utf8());
        UTEST_ASSERT(v.equals_utf8(value));
    }

    void validate(IDictionary *d)
    {
        ck_att(d, 0, "k1", "v1");
        ck_child(d, 1, "k2");
        ck_child(d, 2, "k3");
        ck_child(d, 3, "k4");
        ck_child(d, 4, "k5");
        ck_child(d, 5, "k6");
        ck_child(d, 6, "k7");

        ck_lookup(d, "k1", "v1");
        ck_lookup(d, "k2.a3", "x3");
        ck_lookup(d, "k8.k1.k2", "z2");
    }

    UTEST_MAIN
    {
        JsonDictionary d;

        printf("Testing load of valid dictionary...\n");
        UTEST_ASSERT(d.init(LSP_RESOURCE_PATH "/test/i18n/valid.json") == STATUS_OK);

        printf("Testing lookup of dictionary node...\n");
        IDictionary *xd = NULL;
        UTEST_ASSERT(d.lookup("lalala", &xd) == STATUS_NOT_FOUND);
        UTEST_ASSERT(d.lookup("k2", &xd) == STATUS_OK);
        ck_att(xd, 0, "a1", "x1");
        ck_att(xd, 1, "a2", "x2");
        ck_att(xd, 2, "a3", "x3");
        UTEST_ASSERT(d.lookup("k8.k1", &xd) == STATUS_OK);
        ck_att(xd, 0, "k2", "z2");

        printf("Validating dictionary...\n");
        validate(&d);

        printf("Testing load of invalid dictionary...\n");
        UTEST_ASSERT(d.init("res/test/i18n/invalid1.json") != STATUS_OK);
        UTEST_ASSERT(d.init("res/test/i18n/invalid2.json") != STATUS_OK);
        UTEST_ASSERT(d.init("res/test/i18n/invalid3.json") != STATUS_OK);

        printf("Validating that state of dictionary has not changed...\n");
        validate(&d);
    }

UTEST_END
