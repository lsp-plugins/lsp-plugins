/*
 * mtest.cpp
 *
 *  Created on: 24 авг. 2018 г.
 *      Author: sadko
 */

#include <test/mtest.h>
#include <string.h>
#include <stdarg.h>

namespace test
{
    ManualTest *ManualTest::__root = NULL;

    ManualTest::ManualTest(const char *group, const char *name): Test(group, name)
    {
        // Self-register
        __next              = __root;
        __root              = this;
    }

    ManualTest::~ManualTest()
    {
    }

    ManualTest *mtest_init()
    {
        return ManualTest::__root;
    }

}


