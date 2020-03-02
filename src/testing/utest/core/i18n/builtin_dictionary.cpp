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
        UTEST_ASSERT(d.init("builtin://i18n") == STATUS_OK);

        printf("Testing access...\n");
        ck_lookup(&d, "us.lang.local.us", "English (US)");
        ck_lookup(&d, "us.lang.local.ru", "Russian");
        ck_lookup(&d, "ru.lang.local.ru", "Русский");
        ck_lookup(&d, "default.lang.target.de", "Deutsch");
    }

UTEST_END

#endif

