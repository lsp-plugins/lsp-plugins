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
    void dump_buffer(const char *text, const size_t *buf, size_t count);

    void dump_buffer(const char *text, const ssize_t *buf, size_t count);

    void dump_buffer(const char *text, const float *buf, size_t count);

}

#endif /* TEST_HELPERS_H_ */
