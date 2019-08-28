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
#include <stdlib.h>
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

    void dump_bytes(const char *text, const void *buf, size_t count)
    {
        printf("  dump of buffer %s:\n    ", text);
        const uint8_t *b = reinterpret_cast<const uint8_t *>(buf);
        while (count--)
            printf("%02x ", *(b++));
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

    float randf(float min, float max)
    {
        float v = float(rand()) / (float(RAND_MAX) + 1.0f);
        return min + (max - min) * v;
    }

    void randomize_positive(float *buf, size_t n)
    {
        for (size_t i=0; i<n; ++i)
            buf[i] = (float(rand())/float(RAND_MAX)) + 0.001f;
    }

    void randomize_0to1(float *buf, size_t n)
    {
        for (size_t i=0; i<n; ++i)
            buf[i] = (float(rand())/float(RAND_MAX-1));
    }

    void randomize(float *buf, size_t n, float min, float max)
    {
        float delta = max - min;
        for (size_t i=0; i<n; ++i)
            buf[i] = min + delta * (float(rand())/(RAND_MAX-1));
    }

    void randomize_negative(float *buf, size_t n)
    {
        for (size_t i=0; i<n; ++i)
            buf[i] = - ((float(rand())/float(RAND_MAX)) + 0.001f);
    }

    void randomize_sign(float *buf, size_t n)
    {
        for (size_t i=0; i<n; ++i)
        {
            float tmp = (float(rand())/float(RAND_MAX)) + 0.001f;
            buf[i] = (rand() >= (RAND_MAX >> 1)) ? tmp : -tmp;
        }
    }
}


