/*
 * bits.cpp
 *
 *  Created on: 31 окт. 2018 г.
 *      Author: sadko
 */

#include <dsp/bits.h>
#include <test/utest.h>

UTEST_BEGIN("dsp", bits)
    template <typename T>
        void call(const char *label)
        {
            printf("Testing %s...\n", label);
            size_t n = sizeof(T) * 8;

            for (size_t i=0; i<n; ++i)
            {
                T x     = T(1) << i;
                T y     = reverse_bits(x);
                T ck    = (T(1) << (n - i - 1));
                UTEST_ASSERT_MSG(y == ck, "%s: tier1, i=%d, n=%d", label, int(i), int(n));
            }

            for (size_t i=0; i<(n - 4); ++i)
            {
                T x     = T(1) << i;
                T y     = reverse_bits(x, n - 4);
                T ck    = (T(1) << (n - 5 - i));
                UTEST_ASSERT_MSG(y == ck, "%s: tier2, i=%d, n=%d", label, int(i), int(n));
            }

            for (size_t i=0; i<4; ++i)
            {
                T x     = T(1) << i;
                T y     = reverse_bits(x, 4);
                T ck    = (T(1) << (3 - i));
                UTEST_ASSERT_MSG(y == ck, "%s: tier3, i=%d, n=%d", label, int(i), int(n));
            }
        }

    UTEST_MAIN
    {
        call<uint8_t>("reverse_bits u8");
        call<int8_t>("reverse_bits i8");
        call<uint16_t>("reverse_bits u16");
        call<int16_t>("reverse_bits i16");
        call<uint32_t>("reverse_bits u32");
        call<int32_t>("reverse_bits i32");
        call<uint64_t>("reverse_bits u64");
        call<int64_t>("reverse_bits i64");
    }

UTEST_END;


