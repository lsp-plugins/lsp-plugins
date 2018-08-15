#include <core/types.h>
#include <dsp/endian.h>

namespace endian_test
{
    using namespace lsp;

#pragma pack(push, 1)
    typedef union {
        uint8_t     u8[32];
        int8_t      i8[32];
        uint16_t    u16[16];
        int16_t     i16[16];
        uint32_t    u32[8];
        int32_t     i32[8];
        uint64_t    u64[4];
        int64_t     i64[4];
        float       f32[8];
        double      f64[4];
    } content_t;

    uint8_t u8_ex[] = {
                0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
                0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
                0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17,
                0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f
            };

    uint8_t u16_ex[] = {
                0x01, 0x00, 0x03, 0x02, 0x05, 0x04, 0x07, 0x06,
                0x09, 0x08, 0x0b, 0x0a, 0x0d, 0x0c, 0x0f, 0x0e,
                0x11, 0x10, 0x13, 0x12, 0x15, 0x14, 0x17, 0x16,
                0x19, 0x18, 0x1b, 0x1a, 0x1d, 0x1c, 0x1f, 0x1e
            };

    uint8_t u32_ex[] = {
                0x03, 0x02, 0x01, 0x00, 0x07, 0x06, 0x05, 0x04,
                0x0b, 0x0a, 0x09, 0x08, 0x0f, 0x0e, 0x0d, 0x0c,
                0x13, 0x12, 0x11, 0x10, 0x17, 0x16, 0x15, 0x14,
                0x1b, 0x1a, 0x19, 0x18, 0x1f, 0x1e, 0x1d, 0x1c
            };

    uint8_t u64_ex[] = {
                0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x00,
                0x0f, 0x0e, 0x0d, 0x0c, 0x0b, 0x0a, 0x09, 0x08,
                0x17, 0x16, 0x15, 0x14, 0x13, 0x12, 0x11, 0x10,
                0x1f, 0x1e, 0x1d, 0x1c, 0x1b, 0x1a, 0x19, 0x18
            };
#pragma pack(pop)

#define VASSERT(expr, expected, result) if ((expr) != expected) return result;
#define MASSERT(expr, expected, result) if (memcmp(expr, expected, sizeof(content_t)) != 0) return result;

    void init(content_t *c)
    {
        for (size_t i=0; i<sizeof(content_t); ++i)
            c->u8[i] = i;
    }

    int test(int argc, const char **argv)
    {
        dsp::init();

        content_t c;

        VASSERT(byte_swap(uint8_t(0xa5)), uint8_t(0xa5), 0);
        VASSERT(byte_swap(int8_t(0xa5)), int8_t(0xa5), 1);
        VASSERT(byte_swap(uint16_t(0xa55a)), uint16_t(0x5aa5), 2);
        VASSERT(byte_swap(int16_t(0xa55a)), int16_t(0x5aa5), 3);
        VASSERT(byte_swap(uint32_t(0x01020304)), uint32_t(0x04030201), 4);
        VASSERT(byte_swap(int32_t(0x01020304)), int32_t(0x04030201), 5);
        VASSERT(byte_swap(uint64_t(0x0102030405060708LL)), uint64_t(0x0807060504030201LL), 6);
        VASSERT(byte_swap(int64_t(0x0102030405060708LL)), int64_t(0x0807060504030201LL), 7);

        init(&c);
        byte_swap(c.u8, sizeof(content_t) / sizeof(uint8_t));
        MASSERT(c.u8, u8_ex, 8);

        init(&c);
        byte_swap(c.i8, sizeof(content_t) / sizeof(int8_t));
        MASSERT(c.i8, u8_ex, 9);

        init(&c);
        byte_swap(c.u16, sizeof(content_t) / sizeof(uint16_t));
        MASSERT(c.u16, u16_ex, 10);

        init(&c);
        byte_swap(c.i16, sizeof(content_t) / sizeof(int16_t));
        MASSERT(c.i16, u16_ex, 11);

        init(&c);
        byte_swap(c.u32, sizeof(content_t) / sizeof(uint32_t));
        MASSERT(c.u32, u32_ex, 12);

        init(&c);
        byte_swap(c.i32, sizeof(content_t) / sizeof(int32_t));
        MASSERT(c.i32, u32_ex, 13);

        init(&c);
        byte_swap(c.u64, sizeof(content_t) / sizeof(uint64_t));
        MASSERT(c.u64, u64_ex, 14);

        init(&c);
        byte_swap(c.i64, sizeof(content_t) / sizeof(int64_t));
        MASSERT(c.i64, u64_ex, 15);

        init(&c);
        byte_swap(c.f32, sizeof(content_t) / sizeof(float));
        MASSERT(c.f32, u32_ex, 16);

        init(&c);
        byte_swap(c.f64, sizeof(content_t) / sizeof(double));
        MASSERT(c.f64, u64_ex, 17);

        printf("OK\n");

        return 0;
    }
    
#undef VASSERT
#undef MASSERT

}
