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
        void test_reverse_bits(const char *label)
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

    template <typename T>
        void test_int_log2(const char *label)
        {
            printf("Testing %s...\n", label);

            int log2 = int_log2(T(0));
            UTEST_ASSERT_MSG(log2 == 0, "%s: zero input detected as %d", label, log2);

            for (int i=0; i<int(sizeof(T)*8); ++i)
            {
                T x     = T(1) << i;
                int y   = int_log2(x);
                UTEST_ASSERT_MSG(y == i, "%s: bit=%d but detected as %d", label, i, y);
            }
        }

    UTEST_MAIN
    {
        test_reverse_bits<uint8_t>("reverse_bits u8");
        test_reverse_bits<int8_t>("reverse_bits i8");
        test_reverse_bits<uint16_t>("reverse_bits u16");
        test_reverse_bits<int16_t>("reverse_bits i16");
        test_reverse_bits<uint32_t>("reverse_bits u32");
        test_reverse_bits<int32_t>("reverse_bits i32");
        test_reverse_bits<uint64_t>("reverse_bits u64");
        test_reverse_bits<int64_t>("reverse_bits i64");

        test_int_log2<uint8_t>("int_log2 u8");
        test_int_log2<int8_t>("int_log2 i8");
        test_int_log2<uint16_t>("int_log2 u16");
        test_int_log2<int16_t>("int_log2 i16");
        test_int_log2<uint32_t>("int_log2 u32");
        test_int_log2<int32_t>("int_log2 i32");
        test_int_log2<uint64_t>("int_log2 u64");
        test_int_log2<int64_t>("int_log2 i64");
    }

UTEST_END;


