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
#include <math.h>

namespace test
{
    bool float_equals_relative(float a, float b, float tolerance)
    {
        if (a == 0.0f)
            return (fabs(b) < tolerance);
        else if (b == 0.0f)
            return (fabs(a) < tolerance);

        float diff = (a > b) ? fabs(a/b) : fabs(b/a);
        return fabs(1.0f - diff) < tolerance;
    }

    bool float_equals_absolute(float a, float b, float tolerance)
    {
        return fabs(a - b) <= tolerance;
    }

    bool float_equals_adaptive(float a, float b, float tolerance)
    {
        return (fabs(a) > 1.0f) ?
            float_equals_relative(a, b, tolerance) :
            float_equals_absolute(a, b, tolerance);
    }

    void dump_buffer(const char *text, const size_t *buf, size_t count)
    {
        printf("  dump of buffer %s:\n    ", text);
        while (count--)
            printf("%lu ", (long unsigned int)(*(buf++)));
        printf("\n");
    }

    void dump_buffer(const char *text, const ssize_t *buf, size_t count)
    {
        printf("  dump of buffer %s:\n    ", text);
        while (count--)
            printf("%ld ", (long)(*(buf++)));
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


