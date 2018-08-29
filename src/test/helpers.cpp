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
#include <stdarg.h>

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

    void printf_buffer(const float *buf, size_t count, const char *fmt, ...)
    {
        va_list vl;
        va_start(vl, fmt);
        vprintf(fmt, vl);
        puts("");
        va_end(vl);

        for (size_t i=0; i<count; ++i)
        {
            if (i > 0)
                printf(" ");
            printf("%.6f", buf[i]);
        }
        puts("");
    }
}


