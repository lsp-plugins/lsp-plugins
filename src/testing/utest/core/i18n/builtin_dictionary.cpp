/*
 * builtin_dictionary.cpp
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <core/resource.h>
#include <core/i18n/Dictionary.h>

#ifdef LSP_BUILTIN_RESOURCES

using namespace lsp;

UTEST_BEGIN("core.i18n", builtin_dictionary)

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

        printf("Accessing builtin dictionary...\n");
        UTEST_ASSERT(d.init(LSP_BUILTIN_PREFIX "i18n") == STATUS_OK);

        printf("Testing nested dictionary lookup...\n");
        IDictionary *xd;
        UTEST_ASSERT(d.lookup("unexisting", &xd) == STATUS_NOT_FOUND);
        UTEST_ASSERT(d.lookup("us.lang.local", &xd) == STATUS_OK);
        ck_lookup(xd, "us", "English (US)");
        UTEST_ASSERT(d.lookup("ru.lang", &xd) == STATUS_OK);
        ck_lookup(xd, "local.ru", "Русский");

        printf("Testing access...\n");
        ck_lookup(&d, "us.lang.local.us", "English (US)");
        ck_lookup(&d, "us.lang.local.ru", "Russian");
        ck_lookup(&d, "ru.lang.local.ru", "Русский");
        ck_lookup(&d, "default.lang.target.us", "English (US)");
    }

UTEST_END

#endif

