/*
 * utest.cpp
 *
 *  Created on: 19 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <string.h>
#include <stdarg.h>

namespace test
{
    UnitTest *UnitTest::__root = NULL;

    UnitTest::UnitTest(const char *group, const char *name): Test(group, name)
    {
        // Self-register
        __next              = __root;
        __root              = this;
    }

    UnitTest::~UnitTest()
    {
    }

    double UnitTest::time_limit() const
    {
        return 5.0;
    }

    int UnitTest::printf(const char *fmt, ...)
    {
        if (!__verbose)
            return 0;

        va_list vl;
        va_start(vl, fmt);
        int res = vprintf(fmt, vl);
        va_end(vl);
        return res;
    }

    UnitTest *utest_init()
    {
        // Ensure that there are no duplicates in performance tests
        for (UnitTest *first = UnitTest::__root; first != NULL; first = first->__next)
        {
            const char *group = first->group();
            const char *name  = first->name();

            for (UnitTest *next = first->__next; next != NULL; next = next->__next)
            {
                if (strcasecmp(group, next->group()))
                    continue;
                if (strcasecmp(name, next->name()))
                    continue;

                fprintf(stderr, "Unit test '%s.%s' has duplicate instance\n", group, name);
                return NULL;
            }
        }

        return UnitTest::__root;
    }

}


