/*
 * charset.cpp
 *
 *  Created on: 13 февр. 2019 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <test/utest.h>
#include <test/helpers.h>
#include <dsp/endian.h>

#include <core/io/charset.h>

// This strlen does not analyze surrogate pairs
template <class char_t>
    static size_t strlen_test(const char_t *s)
    {
        size_t n = 0;
        for (; *s != 0; ++s, ++n) {}
        return n;
    }

static size_t utf16_count_invalid(const lsp_utf16_t *s)
{
    size_t n = 0;
    for (; *s != 0; ++s) {
        if (*s == 0xfffd)
            ++n;
    }
    return n;
}

static size_t utf32_count_invalid(const lsp_utf32_t *s)
{
    size_t n = 0;
    for (; *s != 0; ++s) {
        if (*s == 0xfffd)
            ++n;
    }
    return n;
}

static size_t utf8_count_invalid(const char *s)
{
    lsp_utf32_t cp;
    size_t n = 0;

    while ((cp = lsp::read_utf8_codepoint(&s)) != 0)
    {
        if (cp == 0xfffd)
            ++n;
    }
    return n;
}

template <class char_t>
    static char_t *strdup_bswap(const char_t *src, bool le)
    {
        // Estimate string length
        size_t len = 0;
        for (const char_t *p = src; *p != 0; ++p)
            ++len;

        // Allocate memory
        char_t *dst = reinterpret_cast<char_t *>(::malloc(sizeof(char_t) * (len + 1)));
        if (dst == NULL)
            return NULL;

        // Do byte-reversed copy (if required)
        bool swap = __IF_LEBE(!le, le);
        char_t *p = dst;

        if (swap)
        {
            while (*src != 0)
                *(p++) = byte_swap(*(src++));
        }
        else
        {
            while (*src != 0)
            *(p++) = *(src++);
        }
        *p = 0;

        return dst;
    }

template <class char_t>
    static ssize_t strcmp_test(const char_t *s1, const char_t *s2)
    {
        ssize_t diff;
        do {
            diff = ssize_t(*s1) - ssize_t(*s2);
        } while ((diff == 0) && (*(s1++) != 0) && (*(s2++) != 0));

        return diff;
    }

template <class char_t>
    static ssize_t strcmp_bswap(const char_t *s1, const char_t *s2, bool le)
    {
        ssize_t diff;

        bool swap = __IF_LEBE(!le, le);
        if (swap)
        {
            do {
                diff = ssize_t(*s1) - ssize_t(byte_swap(*s2));
            } while ((diff == 0) && (*(s1++) != 0) && (*(s2++) != 0));
        }
        else
        {
            do {
                diff = ssize_t(*s1) - ssize_t(*s2);
            } while ((diff == 0) && (*(s1++) != 0) && (*(s2++) != 0));
        }

        return diff;
    }

typedef struct utf8_check_t
{
    const char *s;
    size_t u16strlen;
    size_t u32strlen;
    size_t invalid;
} utf8_check_t;

typedef struct utf16_check_t
{
    const uint16_t *s;
    size_t u8strlen;
    size_t u32strlen;
    size_t invalid;
} utf16_check_t;

static utf8_check_t utf8_check[] =
{
    { "", 0, 0, 0 },
    { "Test text in ASCII", 18, 18, 0 },
    { "Тестовый текст на русском языке", 31, 31, 0 },
    { "日本語の物語。それはテスト。", 14, 14, 0 },
    { "Test text in ASCII. Тестовый текст на русском языке. 日本語の物語。それはテスト。", 67, 67, 0 },
    { "\x80", 1, 1, 1 },                       // Invalid sequence
    { "\x80Test", 5, 5, 1 },                   // Invalid sequence + text
    { "\xe0\x80\x80", 3, 3, 3 },               // Invalid sequence
    { "\xe0\x80\x80Test", 7, 7, 3 },           // Invalid sequence + text
    { "\xe0\x80\x80\x80", 4, 4, 4 },           // Two invalid sequences
    { "\xe0\x80\x80\x80Test", 8, 8, 4 },       // Two invalid sequences + text
    { "\xed\xa0\x80", 1, 1, 1 },               // Invalid codepoint (surrogate)
    { "\xed\xa0\x80Test", 5, 5, 1 },           // Invalid codepoint (surrogate) + text
    { "\xed\xa0\x80\xed\xa0\x8f", 2, 2, 2 },   // Two invalid codepoints (surrogate)
    { "\xed\xa0\x80\xed\xa0\x8fTest", 6, 6, 2 },   // Two invalid codepoints (surrogate) + text
    { "\xc0\xbf\xcb\xbf", 3, 3, 2 },           // One valid codepoint, one invalid (2 errors)
    { "\xc0\xbf\xcb\xbfTest", 7, 7, 2 },       // One valid codepoint, one invalid (2 errors) + text
    { "\xf0\x90\x80\x8f", 2, 1, 0 },           // Surrogate pair at output
    { "\xf0\x90\x80\x8fTest", 6, 5, 0 }        // Surrogate pair at output + text
};

static const uint16_t u16str_0[]    = { 0xd801, 0xdc37, 0 };
static const uint16_t u16str_1[]    = { 0xd801, 0xdc37, 'T', 'e', 's', 't', 0 };
static const uint16_t u16str_2[]    = { 0xdc37, 0xd801, 0 };
static const uint16_t u16str_3[]    = { 0xdc37, 0xd801, 'T', 'e', 's', 't', 0 };
static const uint16_t u16str_4[]    = { 0xd801, 0 };
static const uint16_t u16str_5[]    = { 0xd801, 'T', 'e', 's', 't', 0 };
static const uint16_t u16str_6[]    = { 0xdc01, 0 };
static const uint16_t u16str_7[]    = { 0xdc01, 'T', 'e', 's', 't', 0 };
static const uint16_t u16str_8[]    = { 0 };
static const uint16_t u16str_9[]    = { 'T', 'e', 's', 't', ' ', 't', 'e', 'x', 't', ' ', 'i', 'n', ' ', 'A', 'S', 'C', 'I', 'I', 0 };
static const uint16_t u16str_10[]   = {
    0x0422, 0x0435, 0x0441, 0x0442, 0x043e, 0x0432, 0x044b, 0x0439,
    0x0020, 0x0442, 0x0435, 0x043a, 0x0441, 0x0442, 0x0020, 0x043d,
    0x0430, 0x0020, 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x043e,
    0x043c, 0x0020, 0x044f, 0x0437, 0x044b, 0x043a, 0x0435, 0
};

static const uint16_t u16str_11[]   = {
    0x65e5, 0x672c, 0x8a9e, 0x306e, 0x7269, 0x8a9e, 0x3002, 0x305d,
    0x308c, 0x306f, 0x30c6, 0x30b9, 0x30c8, 0x3002, 0
};

static const uint16_t u16str_12[]   = {
    'T', 'e', 's', 't', ' ', 't', 'e', 'x', 't', ' ', 'i', 'n', ' ', 'A', 'S', 'C', 'I', 'I',
    '.', ' ',
    0x0422, 0x0435, 0x0441, 0x0442, 0x043e, 0x0432, 0x044b, 0x0439,
    0x0020, 0x0442, 0x0435, 0x043a, 0x0441, 0x0442, 0x0020, 0x043d,
    0x0430, 0x0020, 0x0440, 0x0443, 0x0441, 0x0441, 0x043a, 0x043e,
    0x043c, 0x0020, 0x044f, 0x0437, 0x044b, 0x043a, 0x0435,
    '.', ' ',
    0x65e5, 0x672c, 0x8a9e, 0x306e, 0x7269, 0x8a9e, 0x3002, 0x305d,
    0x308c, 0x306f, 0x30c6, 0x30b9, 0x30c8, 0x3002,
    0
};

static utf16_check_t utf16_check[] =
{
    { u16str_0, 4, 1, 0 },                     // Surrogate pair
    { u16str_1, 8, 5, 0 },                     // Surrogate pair + text
    { u16str_2, 4, 1, 0 },                     // Another surrogate pair
    { u16str_3, 8, 5, 0 },                     // Another surrogate pair + text
    { u16str_4, 3, 1, 1 },                     // Invalid surrogate
    { u16str_5, 7, 5, 1 },                     // Invalid surrogate + text
    { u16str_6, 3, 1, 1 },                     // Another invalid surrogate
    { u16str_7, 7, 5, 1 },                     // Another invalid surrogate + text
    { u16str_8, 0, 0, 0 },                     // ""
    { u16str_9, 18, 18, 0 },                   // "Test text in ASCII"
    { u16str_10, 58, 31, 0 },                  // "Тестовый текст на русском языке"
    { u16str_11, 42, 14, 0 },                  // "日本語の物語。それはテスト。"
    { u16str_12, 122, 67, 0 }                  // "Test text in ASCII. Тестовый текст на русском языке. 日本語の物語。それはテスト。"
};

UTEST_BEGIN("core.io", charset)

    void check_utf8_to_utfX()
    {
        lsp_utf16_t *s_na = NULL, *s_le = NULL, *s_be = NULL;
        lsp_utf32_t *s32_na = NULL, *s32_le = NULL, *s32_be = NULL;
        size_t len;

        printf("Testing check_utf8_to_utfX\n");

        for (size_t i=0, n=sizeof(utf8_check)/sizeof(utf8_check_t); i<n; ++i)
        {
            printf("  checking test line %d...\n", int(i));

            utf8_check_t *ck = &utf8_check[i];

            // UTF8 -> UTF16
            s_na   = lsp::utf8_to_utf16(ck->s);
            s_le   = lsp::utf8_to_utf16le(ck->s);
            s_be   = lsp::utf8_to_utf16be(ck->s);

            UTEST_ASSERT(s_na != NULL);
            UTEST_ASSERT(s_le != NULL);
            UTEST_ASSERT(s_be != NULL);

            UTEST_ASSERT_MSG(((len = strlen_test(s_na)) == ck->u16strlen),
                    "Error checking line %d: utf16_strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u16strlen));
            UTEST_ASSERT_MSG(((len = utf16_count_invalid(s_na)) == ck->invalid),
                    "Error checking line %d: utf16_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));

            UTEST_ASSERT(strcmp_bswap(s_na, s_le, true) == 0);
            UTEST_ASSERT(strcmp_bswap(s_na, s_be, false) == 0);
            if (len > 0)
                UTEST_ASSERT(strcmp_test(s_le, s_be) != 0);

            free(s_na);

            // UTF8 -> UTF32
            s32_na = lsp::utf8_to_utf32(ck->s);
            s32_le = lsp::utf8_to_utf32le(ck->s);
            s32_be = lsp::utf8_to_utf32be(ck->s);

            UTEST_ASSERT(s32_na != NULL);
            UTEST_ASSERT(s32_le != NULL);
            UTEST_ASSERT(s32_be != NULL);

            UTEST_ASSERT_MSG(((len = strlen_test(s32_na)) == ck->u32strlen),
                    "Error checking line %d: utf32_strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u32strlen));
            UTEST_ASSERT_MSG(((len = utf32_count_invalid(s32_na)) == ck->invalid),
                    "Error checking line %d: utf32_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));

            UTEST_ASSERT(strcmp_bswap(s32_na, s32_le, true) == 0);
            UTEST_ASSERT(strcmp_bswap(s32_na, s32_be, false) == 0);
            if (len > 0)
                UTEST_ASSERT(strcmp_test(s32_le, s32_be) != 0);

            free(s32_na);
        }
    }

    void check_utf16_to_utfX()
    {
        lsp_utf16_t *le = NULL, *be = NULL;
        const lsp_utf16_t *na = NULL;
        char *s_na = NULL, *s_le = NULL, *s_be = NULL;
        lsp_utf32_t *s32[9];
        size_t len;
        printf("Testing check_utf16_to_utfX\n");

        for (size_t i=0, n=sizeof(utf16_check)/sizeof(utf16_check_t); i<n; ++i)
        {
            printf("  checking test line %d...\n", int(i));

            utf16_check_t *ck = &utf16_check[i];

            // Obtain native string and it's LE and BE copies
            na  = reinterpret_cast<const lsp_utf16_t *>(ck->s);
            le  = strdup_bswap(na, true);
            be  = strdup_bswap(na, false);

            len = strlen_test(na);
            UTEST_ASSERT(le != NULL);
            UTEST_ASSERT(be != NULL);
            if (len > 0)
                UTEST_ASSERT(strcmp_test(le, be) != 0);

            // UTF16 -> UTF8
            s_na    = lsp::utf16_to_utf8(na);
            s_le    = lsp::utf16le_to_utf8(le);
            s_be    = lsp::utf16be_to_utf8(be);

            UTEST_ASSERT(s_na != NULL);
            UTEST_ASSERT(s_le != NULL);
            UTEST_ASSERT(s_be != NULL);

            UTEST_ASSERT_MSG(((len = strlen_test(s_na)) == ck->u8strlen),
                    "Error checking line %d: strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u8strlen));
            UTEST_ASSERT_MSG(((len = utf8_count_invalid(s_na)) == ck->invalid),
                    "Error checking line %d: utf8_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));

            UTEST_ASSERT(strcmp_test(s_le, s_be) == 0);
            UTEST_ASSERT(strcmp_test(s_na, s_le) == 0);
            UTEST_ASSERT(strcmp_test(s_na, s_be) == 0);

            free(s_na);
            free(s_le);
            free(s_be);

            // UTF16 -> UTF32
            s32[0]  = lsp::utf16_to_utf32(na);
            s32[1]  = lsp::utf16le_to_utf32(le);
            s32[2]  = lsp::utf16be_to_utf32(be);
            s32[3]  = lsp::utf16_to_utf32le(na);
            s32[4]  = lsp::utf16le_to_utf32le(le);
            s32[5]  = lsp::utf16be_to_utf32le(be);
            s32[6]  = lsp::utf16_to_utf32be(na);
            s32[7]  = lsp::utf16le_to_utf32be(le);
            s32[8]  = lsp::utf16be_to_utf32be(be);

            for (size_t j=0; j<9; ++j)
                UTEST_ASSERT(s32[j] != NULL);

            UTEST_ASSERT_MSG(((len = strlen_test(s32[0])) == ck->u32strlen),
                    "Error checking line %d: strlen=%d, expected=%d",
                    int(i), int(len), int(ck->u32strlen));
            UTEST_ASSERT_MSG(((len = utf32_count_invalid(s32[0])) == ck->invalid),
                    "Error checking line %d: utf32_count_invalid=%d, expected=%d",
                    int(i), int(len), int(ck->invalid));

            UTEST_ASSERT(strcmp_test(s32[0], s32[1]) == 0);
            UTEST_ASSERT(strcmp_test(s32[0], s32[2]) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[3], true) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[4], true) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[5], true) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[6], false) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[7], false) == 0);
            UTEST_ASSERT(strcmp_bswap(s32[0], s32[8], false) == 0);

            if (len > 0)
            {
                UTEST_ASSERT(strcmp_test(s32[4], s32[7]) != 0);
                UTEST_ASSERT(strcmp_test(s32[5], s32[7]) != 0);
                UTEST_ASSERT(strcmp_test(s32[4], s32[8]) != 0);
                UTEST_ASSERT(strcmp_test(s32[5], s32[8]) != 0);
            }

            for (size_t j=0; j<9; ++j)
                free(s32[j]);

            // Free LE and BE copies
            free(le);
            free(be);
        }
    }

    UTEST_MAIN
    {
        check_utf8_to_utfX();
        check_utf16_to_utfX();
    }
UTEST_END;



