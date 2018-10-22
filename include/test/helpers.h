/*
 * helpers.h
 *
 *  Created on: 27 авг. 2018 г.
 *      Author: sadko
 */

#ifndef TEST_HELPERS_H_
#define TEST_HELPERS_H_

#include <sys/types.h>
#include <stdint.h>

namespace test
{
    bool float_equals_relative(float a, float b, float tolerance=1e-6);

    bool float_equals_absolute(float a, float b, float tolerance=1e-6);

    bool float_equals_adaptive(float a, float b, float tolerance=1e-6);

    void dump_buffer(const char *text, const size_t *buf, size_t count);

    void dump_buffer(const char *text, const ssize_t *buf, size_t count);

    void dump_buffer(const char *text, const float *buf, size_t count);

    void printf_buffer(const float *buf, size_t count, const char *fmt, ...);

}

#endif /* TEST_HELPERS_H_ */
