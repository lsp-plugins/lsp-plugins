/*
 * utest.cpp
 *
 *  Created on: 19 авг. 2018 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <string.h>

namespace test
{
    UnitTest *UnitTest::__root = NULL;

    UnitTest::UnitTest(const char *group, const char *name)
    {
        __test_group        = group;
        __test_name         = name;
        __next              = __root;

        // Self-register
        __root              = this;
    }

    UnitTest::~UnitTest()
    {
    }

    bool UnitTest::ignore() const
    {
        return false;
    }

    double UnitTest::time_limit() const
    {
        return 5.0;
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

                fprintf(stderr, "Test '%s' group '%s' has duplicate instance\n", name, group);
                return NULL;
            }
        }

        return UnitTest::__root;
    }

}


