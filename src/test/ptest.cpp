/*
 * ptest.cpp
 *
 *  Created on: 16 авг. 2018 г.
 *      Author: sadko
 */

#include <test/ptest.h>
#include <string.h>

namespace test
{
    namespace ptest
    {
        PerformanceTest *PerformanceTest::__root = NULL;

        PerformanceTest::PerformanceTest(const char *group, const char *name, float time, size_t iterations)
        {
            __test_group        = group;
            __test_name         = name;
            __test_time         = time;
            __test_iterations   = iterations;
            __next              = __root;

            // Self-register
            __root              = this;
        }

        PerformanceTest::~PerformanceTest()
        {
        }

        PerformanceTest *init()
        {
            // Ensure that there are no duplicates in performance tests
            for (PerformanceTest *first = PerformanceTest::__root; first != NULL; first = first->__next)
            {
                const char *group = first->group();
                const char *name  = first->name();

                for (PerformanceTest *next = first->__next; next != NULL; next = next->__next)
                {
                    if (strcasecmp(group, next->group()))
                        continue;
                    if (strcasecmp(name, next->name()))
                        continue;

                    fprintf(stderr, "Test '%s' group '%s' has duplicate instance\n", name, group);
                    return NULL;
                }
            }

            return PerformanceTest::__root;
        }
    }
}


