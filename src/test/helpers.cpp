/*
 * helpers.cpp
 *
 *  Created on: 27 авг. 2018 г.
 *      Author: sadko
 */

#include <test/helpers.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdint.h>

namespace test
{
    void dump_buffer(const char *text, const size_t *buf, size_t count)
    {
        printf("  dump of buffer %s:\n    ", text);
        while (count--)
            printf("%lu ", *(buf++));
        printf("\n");
    }

    void dump_buffer(const char *text, const ssize_t *buf, size_t count)
    {
        printf("  dump of buffer %s:\n    ", text);
        while (count--)
            printf("%ld ", *(buf++));
        printf("\n");
    }

    void dump_buffer(const char *text, const float *buf, size_t count)
    {
        printf("dump of buffer %s:\n", text);
        while (count--)
            printf("%.30f\n", *(buf++));
    }
}


