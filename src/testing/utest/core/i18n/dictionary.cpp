/*
 * dictionary.cpp
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/resource.h>
#include <core/i18n/Dictionary.h>

using namespace lsp;

UTEST_BEGIN("core.i18n", dictionary)

    void ck_lookup(IDictionary *d, const char *name, const char *value)
    {
        LSPString v;
        printf("  lookup %s ...\n", name, value);
        UTEST_ASSERT(d->lookup(name, &v) == STATUS_OK);
        printf("  got %s -> %s...\n", name, v.get_utf8());
        UTEST_ASSERT(v.equals_utf8(value));
    }

    UTEST_MAIN
    {
        Dictionary d;

        printf("Creating dictionary...\n");
        UTEST_ASSERT(d.init(LSP_RESOURCE_PATH) == STATUS_OK);

        printf("Testing dictionary lookup...\n");
        IDictionary *xd;
        UTEST_ASSERT(d.lookup("test.i18n.k1", &xd) == STATUS_NOT_FOUND);
        UTEST_ASSERT(d.lookup("test.i18n.valid", &xd) == STATUS_OK);
        ck_lookup(xd, "k1", "v1");
        ck_lookup(xd, "k8.k1.k2", "z2");
        ck_lookup(xd, "k7.a3", "x3");

        printf("Testing parameter access...\n");
        LSPString v;
        UTEST_ASSERT(d.lookup("test.i18n", &v) == STATUS_NOT_FOUND);
        UTEST_ASSERT(d.lookup("test.i18n.valid", &v) == STATUS_NOT_FOUND);

        ck_lookup(&d, "test.i18n.valid.k1", "v1");
        ck_lookup(&d, "test.i18n.valid.k8.k1.k2", "z2");
        ck_lookup(&d, "test.i18n.valid.k7.a3", "x3");

        UTEST_ASSERT(d.lookup("x.a", &v) == STATUS_NOT_FOUND);
        UTEST_ASSERT(d.lookup("x.a", &v) == STATUS_NOT_FOUND);
    }

UTEST_END


