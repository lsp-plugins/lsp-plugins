/*
 * coding.h
 *
 *  Created on: 25 июн. 2019 г.
 *      Author: sadko
 */

#ifndef DSP_ARCH_NATIVE_CODING_H_
#define DSP_ARCH_NATIVE_CODING_H_

/*

#include <string.h>
#include <stdio.h>

static const char *base64_table  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

int main()
{
    for (int i=0; i<0x100; ++i)
    {
        const char *p = (i != '=') ? ::strchr(base64_table, i) : base64_table;
        if ((p == NULL) || (i == '\0'))
            printf("  -1, ");
        else
            printf("0x%02x, ", int(p - base64_table));

        if ((i & 0x0f) == 0x0f)
            printf("\n");
    }

    return 0;
}

 */

namespace native
{
    static const char *base64_table  = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    static const int8_t base64_lookup[] = {
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1, 0x3e,   -1,   -1,   -1, 0x3f,
      0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d,   -1,   -1,   -1,   -1,   -1,   -1,
        -1, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
      0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19,   -1,   -1,   -1,   -1,   -1,
        -1, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
      0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30, 0x31, 0x32, 0x33,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
    };

    size_t base64_enc(void *dst, size_t *dst_left, const void *src, size_t *src_left)
    {
        uint8_t *d          = reinterpret_cast<uint8_t *>(dst);
        const uint8_t *s    = reinterpret_cast<const uint8_t *>(src);
        size_t dl = *dst_left, sl = *src_left, n = 0;

        // Main body
        while (sl >= 3)
        {
            // Enough space?
            if (dl < 4)
                break;

            // Perform encode
            uint32_t x  = (s[0] << 16) | (s[1] << 8) | s[2];
            d[0]        = base64_table[x >> 18];
            d[1]        = base64_table[(x >> 12) & 0x3f];
            d[2]        = base64_table[(x >> 6) & 0x3f];
            d[3]        = base64_table[x & 0x3f];

            n          += 3;
            s          += 3;
            d          += 4;
            sl         -= 3;
            dl         -= 4;
        }

        // Tail
        if (sl > 0)
        {
            if ((sl == 1) && (dl >= 2))
            {
                uint32_t x  = s[0];
                d[0]        = base64_table[x >> 2];
                d[1]        = base64_table[(x << 4) & 0x3f];

                n          += 1;
                sl         -= 1;
                dl         -= 2;
            }
            else if ((sl == 2) && (dl >= 3))
            {
                uint32_t x  = (s[0] << 8) | s[1];
                d[0]        = base64_table[x >> 10];
                d[1]        = base64_table[(x >> 4) & 0x3f];
                d[2]        = base64_table[(x << 2) & 0x3f];

                n          += 2;
                sl         -= 2;
                dl         -= 3;
            }
        }

        *dst_left   = dl;
        *src_left   = sl;

        return n;
    }

    ssize_t base64_dec(void *dst, size_t *dst_left, const void *src, size_t *src_left)
    {
        uint8_t *d          = reinterpret_cast<uint8_t *>(dst);
        const uint8_t *s    = reinterpret_cast<const uint8_t *>(src);
        size_t dl = *dst_left, sl = *src_left, n = 0;
        int32_t x;

        // Main body
        while (sl >= 4)
        {
            // Enough space?
            if (dl < 3)
                break;

            x       = (base64_lookup[s[0]] << 18) |
                      (base64_lookup[s[1]] << 12) |
                      (base64_lookup[s[2]] << 6) |
                      base64_lookup[s[3]];

            if (x < 0)
            {
                if (n > 0) break;
                else return -1;
            }

            d[0]    = uint8_t(x >> 16);
            d[1]    = uint8_t(x >> 8);
            d[2]    = uint8_t(x);

            d      += 3;
            s      += 4;
            dl     -= 3;
            sl     -= 4;
            n      += 3;
        }

        // Tail
        if (sl > 0)
        {
            if ((sl == 3) && (dl >= 2))
            {
                x       = (base64_lookup[s[0]] << 12) |
                          (base64_lookup[s[1]] << 6) |
                          base64_lookup[s[2]];

                if (x >= 0)
                {
                    d[0]    = uint8_t(x >> 10);
                    d[1]    = uint8_t(x >> 2);

                    dl     -= 2;
                    sl     -= 3;
                    n      += 2;

                }
                else if (n <= 0)
                    return -1;
            }
            else if ((sl == 2) && (dl >= 1))
            {
                x       = (base64_lookup[s[0]] << 6) |
                          base64_lookup[s[1]];

                if (x >= 0)
                {
                    d[0]    = uint8_t(x >> 4);

                    dl     -= 1;
                    sl     -= 2;
                    n      += 1;
                }
                else if (n <= 0)
                    return -1;
            }
            else if ((sl == 1) && (n <= 0))
                return -1;
        }

        *dst_left   = dl;
        *src_left   = sl;

        return n;
    }
}

#endif /* DSP_ARCH_NATIVE_CODING_H_ */
