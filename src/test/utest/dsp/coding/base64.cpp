/*
 * base64.cpp
 *
 *  Created on: 25 июн. 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/ByteBuffer.h>
#include <dsp/dsp.h>

typedef size_t (* base64_enc_t)(void *dst, size_t *dst_left, const void *src, size_t *src_left);
typedef ssize_t (* base64_dec_t)(void *dst, size_t *dst_left, const void *src, size_t *src_left);

static const char *text =
        "Man is distinguished, not only by his reason, but by this singular passion from other animals, "
        "which is a lust of the mind, that by a perseverance of delight in the continued and indefatigable "
        "generation of knowledge, exceeds the short vehemence of any carnal pleasure.";

static const char *base64 =
        "TWFuIGlzIGRpc3Rpbmd1aXNoZWQsIG5vdCBvbmx5IGJ5IGhpcyByZWFzb24sIGJ1dCBieSB0aGlz"
        "IHNpbmd1bGFyIHBhc3Npb24gZnJvbSBvdGhlciBhbmltYWxzLCB3aGljaCBpcyBhIGx1c3Qgb2Yg"
        "dGhlIG1pbmQsIHRoYXQgYnkgYSBwZXJzZXZlcmFuY2Ugb2YgZGVsaWdodCBpbiB0aGUgY29udGlu"
        "dWVkIGFuZCBpbmRlZmF0aWdhYmxlIGdlbmVyYXRpb24gb2Yga25vd2xlZGdlLCBleGNlZWRzIHRo"
        "ZSBzaG9ydCB2ZWhlbWVuY2Ugb2YgYW55IGNhcm5hbCBwbGVhc3VyZS4=";

typedef struct base64check_t
{
    const char *decoded;
    const char *encoded;
} base64check_t;

static const base64check_t base64checks[] =
{
    { "", "" },
    { "1", "MQ" },
    { "12", "MTI" },
    { "123", "MTIz" },
    { "Test string for base64 encoding", "VGVzdCBzdHJpbmcgZm9yIGJhc2U2NCBlbmNvZGluZw" },
    { "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ+/", "MDEyMzQ1Njc4OWFiY2RlZmdoaWprbG1ub3BxcnN0dXZ3eHl6QUJDREVGR0hJSktMTU5PUFFSU1RVVldYWVorLw" },
    { NULL, NULL }
};

namespace native
{
    size_t base64_enc(void *dst, size_t *dst_left, const void *src, size_t *src_left);
    ssize_t base64_dec(void *dst, size_t *dst_left, const void *src, size_t *src_left);
}

UTEST_BEGIN("dsp.coding", base64)

    UTEST_TIMELIMIT(30)

    void test_encode(const char *caption, base64_enc_t enc)
    {
        printf("Testing %s...\n", caption);

        size_t src_left = strlen(text);
        size_t dst_left = strlen(base64) - 1;

        ByteBuffer buf(dst_left);
        size_t n = enc(buf.data<uint8_t>(), &dst_left, text, &src_left);

        UTEST_ASSERT(!buf.corrupted());
        UTEST_ASSERT(n == strlen(text));
        UTEST_ASSERT(dst_left == 0);
        UTEST_ASSERT(src_left == 0);
        UTEST_ASSERT(memcmp(buf.data<void>(), base64, (strlen(base64)-1)) == 0);
    }

    void test_decode(const char *caption, base64_dec_t dec)
    {
        printf("Testing %s...\n", caption);

        size_t src_left = strlen(base64);
        size_t dst_left = strlen(text);

        ByteBuffer buf(dst_left);
        buf.fill_zero();
        ssize_t n = dec(buf.data<uint8_t>(), &dst_left, base64, &src_left);
        printf("decoded = %s\n", buf.data<char>());

        UTEST_ASSERT(!buf.corrupted());
        UTEST_ASSERT(dst_left == 2);
        UTEST_ASSERT(src_left == 4);
        UTEST_ASSERT(n == ssize_t(strlen(text)-dst_left));

        size_t dst_off = strlen(text) - dst_left;
        size_t src_off = strlen(base64) - src_left;
        --src_left;

        n = dec(buf.data<uint8_t>(dst_off), &dst_left, &base64[src_off], &src_left);
        UTEST_ASSERT(n == 2);
        UTEST_ASSERT(dst_left == 0);
        UTEST_ASSERT(src_left == 0);

        UTEST_ASSERT(memcmp(buf.data<uint8_t>(), text, (strlen(text)-dst_left)) == 0);
    }

    void test_encdec(const char *caption, base64_enc_t enc, base64_dec_t dec)
    {
        printf("Testing %s...\n", caption);

        for (const base64check_t *c = base64checks; c->decoded != NULL; ++c)
        {
            printf("Testing encoding of string \"%s\"...\n", c->decoded);
            size_t src_left = strlen(c->decoded);
            size_t dst_left = strlen(c->encoded);

            ByteBuffer ebuf(dst_left);
            size_t n = enc(ebuf.data<uint8_t>(), &dst_left, c->decoded, &src_left);
            UTEST_ASSERT(!ebuf.corrupted());
            UTEST_ASSERT(n == strlen(c->decoded));
            UTEST_ASSERT(dst_left == 0);
            UTEST_ASSERT(src_left == 0);
            UTEST_ASSERT(memcmp(ebuf.data<void>(), c->encoded, (strlen(c->encoded))) == 0);

            printf("Testing decoding of string \"%s\"...\n", c->encoded);
            src_left = strlen(c->encoded);
            dst_left = strlen(c->decoded);
            ByteBuffer dbuf(dst_left);
            n = dec(dbuf.data<uint8_t>(), &dst_left, c->encoded, &src_left);
            UTEST_ASSERT(!dbuf.corrupted());
            UTEST_ASSERT(n == strlen(c->decoded));
            UTEST_ASSERT(dst_left == 0);
            UTEST_ASSERT(src_left == 0);
            UTEST_ASSERT(memcmp(dbuf.data<void>(), c->decoded, (strlen(c->decoded))) == 0);
        }
    }

    UTEST_MAIN
    {
        test_encode("native::base64_enc", native::base64_enc);
        test_decode("native::base64_dec", native::base64_dec);
        test_encdec("native::base64_encdec", native::base64_enc, native::base64_dec);
    }
UTEST_END;


