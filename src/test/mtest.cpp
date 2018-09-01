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
        // Ensure that there are no duplicates in performance tests
        for (ManualTest *first = ManualTest::__root; first != NULL; first = first->__next)
        {
            const char *group = first->group();
            const char *name  = first->name();

            for (ManualTest *next = first->__next; next != NULL; next = next->__next)
            {
                if (strcasecmp(group, next->group()))
                    continue;
                if (strcasecmp(name, next->name()))
                    continue;

                fprintf(stderr, "Manual test '%s.%s' has duplicate instance\n", group, name);
                return NULL;
            }
        }

        return ManualTest::__root;
    }

}


