/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 22 авг. 2018 г.
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

#include <core/stdlib/stdio.h>
#include <test/test.h>
#include <stdarg.h>

namespace test
{
    lsp::cvector<void> Test::Test::support;

    Test::Test(const char *group, const char *name)
    {
        __test_group        = group;
        __test_name         = name;
        __verbose           = false;
        __full_name         = NULL;
        __executable        = NULL;
    }

    Test::~Test()
    {
        if ((__full_name != NULL) && (__full_name != __test_name))
        {
            free(__full_name);
            __full_name = NULL;
        }
    }

    const char *Test::full_name() const
    {
        if (__full_name == NULL)
        {
            if ((__test_group != NULL) && (strlen(__test_group) > 0))
            {
                int n = asprintf(&__full_name, "%s.%s", __test_group, __test_name);
                if (n < 0)
                    return NULL;
            }
            if (__full_name == NULL)
                __full_name         = const_cast<char *>(__test_name);
        }
        return __full_name;
    }

    bool Test::ignore() const
    {
        return false;
    }

    void Test::init()
    {
    }

    void Test::destroy()
    {
        __executable        = NULL;
    }

    void Test::__mark_supported(const void *ptr)
    {
        support.add(const_cast<void *>(ptr));
    }

    bool Test::__check_supported(const void *ptr)
    {
        return support.index_of(ptr) >= 0;
    }

    int Test::printf(const char *fmt, ...)
    {
        if (!__verbose)
            return 0;

        va_list vl;
        va_start(vl, fmt);
        int res = ::vprintf(fmt, vl);
        va_end(vl);
        fflush(stdout);
        return res;
    }

    int Test::eprintf(const char *fmt, ...)
    {
        va_list vl;
        va_start(vl, fmt);
        int res = ::vfprintf(stderr, fmt, vl);
        va_end(vl);
        fflush(stdout);
        return res;
    }
}



