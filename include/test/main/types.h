/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 февр. 2019 г.
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

#ifndef TEST_MAIN_TYPES_H_
#define TEST_MAIN_TYPES_H_

#include <common/types.h>
#include <data/cvector.h>
#include <test/ptest.h>
#include <test/utest.h>
#include <test/mtest.h>
#include <sys/stat.h>

#ifdef PLATFORM_WINDOWS
    #include <processthreadsapi.h>
    #include <sysinfoapi.h>
    #include <errhandlingapi.h>
#endif

#ifdef PLATFORM_UNIX_COMPATIBLE
    #include <unistd.h>
    #include <sys/wait.h>

    #include <fcntl.h>
#endif /* PLATFORM_UNIX_COMPATIBLE */

#if defined(PLATFORM_LINUX) && defined(__GLIBC__)
    #include <mcheck.h>
#endif /* PLATFORM_LINUX */

#if defined(PLATFORM_WINDOWS)
    typedef PROCESS_INFORMATION     test_pid_t;
    typedef FILETIME                test_clock_t;
#else
    typedef pid_t                   test_pid_t;
    typedef struct timespec         test_clock_t;
#endif

namespace lsp
{
    typedef struct stats_t
    {
        size_t      total;
        double      overall;

        cvector<test::Test> success; // List of failed tests
        cvector<test::Test> failed; // List of failed tests
        cvector<test::Test> ignored; // List of ignored tests
    } stats_t;

    typedef struct task_t
    {
        pid_t               pid;
        struct timespec     submitted;
        test::UnitTest     *utest;
    } task_t;

#if defined(PLATFORM_WINDOWS)
    inline void get_test_time(test_clock_t *clock)
    {
        GetSystemTimeAsFileTime(clock);
    }

    inline double calc_test_time_difference(const test_clock_t *begin, const test_clock_t *end)
    {
        uint64_t ibegin = (uint64_t(begin->dwHighDateTime) << 32) | begin->dwLowDateTime;
        uint64_t iend   = (uint64_t(end->dwHighDateTime) << 32) | end->dwLowDateTime;
        return (iend - ibegin) * 1e-7;
    }
#else
    inline void get_test_time(test_clock_t *clock)
    {
        clock_gettime(CLOCK_REALTIME, clock);
    }

    inline double calc_test_time_difference(const test_clock_t *begin, const test_clock_t *end)
    {
        return (end->tv_sec - begin->tv_sec) + (end->tv_nsec - begin->tv_nsec) * 1e-9;
    }
#endif /* PLATFORM_WINDOWS */
}

#endif /* TEST_MAIN_TYPES_H_ */
