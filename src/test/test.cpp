/*
 * test.cpp
 *
 *  Created on: 22 авг. 2018 г.
 *      Author: vsadovnikov
 */

#include <test/test.h>

namespace test
{
    lsp::cvector<void> Test::Test::support;

    Test::Test(const char *group, const char *name)
    {
        __test_group        = group;
        __test_name         = name;
        __verbose           = false;
    }

    Test::~Test()
    {
    }

    bool Test::ignore() const
    {
        return false;
    }

    void Test::__mark_supported(const void *ptr)
    {
        support.add(const_cast<void *>(ptr));
    }

    bool Test::__check_supported(const void *ptr)
    {
        return support.index_of(ptr) >= 0;
    }
}



