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
        return UnitTest::__root;
    }

}


