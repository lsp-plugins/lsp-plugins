/*
 * types.h
 *
 *  Created on: 11 февр. 2019 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CONTAINER_TEST_TYPES_H_
#define INCLUDE_CONTAINER_TEST_TYPES_H_

#include <core/types.h>
#include <data/cvector.h>
#include <test/ptest.h>
#include <test/utest.h>
#include <test/mtest.h>

namespace lsp
{

    typedef struct stats_t
    {
        size_t      total;
        size_t      success;
        double      overall;
        cvector<test::Test> failed; // List of failed tests
        cvector<test::Test> ignored; // List of ignored tests
    } stats_t;

    typedef struct task_t
    {
        pid_t               pid;
        struct timespec     submitted;
        test::UnitTest     *utest;
    } task_t;
}

#endif /* INCLUDE_CONTAINER_TEST_TYPES_H_ */
