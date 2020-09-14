/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 2 мар. 2020 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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

