/*
 * ptest.cpp
 *
 *  Created on: 16 авг. 2018 г.
 *      Author: sadko
 */

#include <test/ptest.h>

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
    }
}


