/*
 * osc.cpp
 *
 *  Created on: 21 мая 2019 г.
 *      Author: sadko
 */

#include <test/utest.h>
#include <test/helpers.h>
#include <core/stdlib/string.h>
#include <core/protocol/osc.h>
#include <dsp/endian.h>

using namespace lsp;

static uint8_t simple_message[] =
{
    '/',    'o',    's',    'c',
    'i',    'l',    'l',    'a',
    't',    'o',    'r',    '/',
    '4',    '/',    'f',    'r',
    'e',    'q',    'u',    'e',
    'n',    'c',    'y',    0,
    ',',    'f',    0,      0,
    0x43,   0xdc,   0,      0
};

static uint8_t legacy_message[] =
{
    '/',    't',    'e',    's',
    't',    0,      0,      0
};

#pragma pack(push, 1)
typedef struct parameter_message_t
{
    char        address[12];
    char        types[32];

    int32_t     int32;
    float       float32;
    char        string[8];
    uint32_t    blob_len;
    char        blob[8];
    int64_t     int64;
    double      double64;
    uint64_t    time_tag;
    char        type[8];
    uint32_t    ascii;
    uint32_t    rgba;
    uint8_t     midi[4];
    uint8_t     raw_midi[4];

    struct {
        int32_t     int32;
        struct {
            char        string[8];
            uint32_t    blob_size;
            char        blob[8];
            int32_t     int32;
        } array;
        float       float32;
    } array;
} parameter_message_t;

#pragma pack(pop)

#pragma pack(push, 1)
typedef struct message_bundle_t
{
    char        signature[8];
    uint64_t    time_tag;

    uint32_t    bundle1_size;
    struct
    {
        char        signature[8];
        uint64_t    time_tag;

        uint32_t    message1_size;
        struct
        {
            char        address[4];
            char        types[4];
            int32_t     int32;
        } message1;

        uint32_t    message2_size;
        struct
        {
            char        address[8];
            char        types[8];
            int32_t     int32;
            float       float32;
        } message2;
    } bundle1;

    uint32_t    bundle2_size;
    struct
    {
        char        signature[8];
        uint64_t    time_tag;

        uint32_t    message1_size;
        struct
        {
            char        address[4];
            char        types[4];
            int32_t     int32;
        } message1;

        uint32_t    message2_size;
        struct
        {
            char        address[4];
            char        types[8];
        } message2;
    } bundle2;

    uint32_t    message_size;
    struct
    {
        char        address[8];
        char        types[8];
        int32_t     int32;
    } message;
} message_bundle_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct format_message_t
{
    char        address[12];
    char        types[16];

    int32_t     int32;
    float       float32;
    char        string[12];
    int64_t     int64;
    uint64_t    time_tag;
    double      double64;
    char        type[12];
    uint32_t    ascii;
    uint32_t    rgba;
} format_message_t;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct special_message_t
{
    char        address[12];
    char        types[24];
} special_message_t;
#pragma pack(pop)

UTEST_BEGIN("core.protocol", osc)
    void test_forge_simple_message()
    {
        osc::packet_t packet;
        osc::forge_t forge;
        osc::forge_frame_t frame, message, other;

        // Curly braces are used for visualization of nested structures
        UTEST_ASSERT(osc::forge_begin_dynamic(&frame, &forge) == STATUS_OK);
        {
            // Perform some 'dumb' error checks
            UTEST_ASSERT(osc::forge_close(&packet, &forge) == STATUS_BAD_STATE);
            UTEST_ASSERT(osc::forge_begin_message(&frame, &frame, "/fail") == STATUS_BAD_ARGUMENTS);
            UTEST_ASSERT(osc::forge_float32(&frame, 440.0f) == STATUS_BAD_STATE);

            // Start message
            UTEST_ASSERT(osc::forge_begin_message(&message, &frame, "/oscillator/4/frequency") == STATUS_OK);
            {
                // Perform some 'dumb' error checks
                UTEST_ASSERT(osc::forge_close(&packet, &forge) == STATUS_BAD_STATE);
                UTEST_ASSERT(osc::forge_end(&frame) == STATUS_BAD_STATE);
                UTEST_ASSERT(osc::forge_begin_message(&other, &frame, "/fail") == STATUS_BAD_STATE);
                UTEST_ASSERT(osc::forge_begin_message(&other, &message, "/fail") == STATUS_BAD_STATE);
                UTEST_ASSERT(osc::forge_begin_bundle(&other, &message, 0) == STATUS_BAD_STATE);

                // Emit parameter
                UTEST_ASSERT(osc::forge_float32(&message, 440.0f) == STATUS_OK);
            }
            UTEST_ASSERT(osc::forge_end(&message) == STATUS_OK);

            // Perform some 'dump' error checks
            UTEST_ASSERT(osc::forge_end(&message) == STATUS_BAD_STATE);
            UTEST_ASSERT(osc::forge_begin_message(&message, &frame, "/fail") == STATUS_BAD_STATE);
            UTEST_ASSERT(osc::forge_begin_bundle(&message, &frame, 0) == STATUS_BAD_STATE);
            UTEST_ASSERT(osc::forge_float32(&frame, 440.0f) == STATUS_BAD_STATE);
        }
        UTEST_ASSERT(osc::forge_end(&frame) == STATUS_OK);
        UTEST_ASSERT(osc::forge_end(&frame) == STATUS_BAD_STATE); // 'dumb' check

        UTEST_ASSERT(osc::forge_close(&packet, &forge) == STATUS_OK);
        UTEST_ASSERT(osc::forge_close(&packet, &forge) == STATUS_BAD_STATE); // 'dumb' check

        UTEST_ASSERT(osc::forge_destroy(&forge) == STATUS_OK);

        // Validate output data
        UTEST_ASSERT(packet.data != NULL);
        bool matches = (packet.size == sizeof(simple_message)) && (::memcmp(packet.data, simple_message, sizeof(simple_message)) == 0);
        if (!matches)
        {
            dump_bytes("src", simple_message, sizeof(simple_message));
            dump_bytes("dst", packet.data, packet.size);
            UTEST_FAIL_MSG("Source and destination messages differ");
        }
        osc::forge_free(packet.data);
    }

    void test_forge_parameters()
    {
        // Initialize parametrized message
        parameter_message_t m;
        bzero(&m, sizeof(m));
        strcpy(m.address, "/address/01");
        strcpy(m.types, ",ifsbhdtScrmm[iT[sbiI]f]TFNI");

        m.int32         = CPU_TO_BE(int32_t(0xdeadbeef));
        m.float32       = CPU_TO_BE(float(M_PI));
        strcpy(m.string, "string1");
        m.blob_len      = CPU_TO_BE(uint32_t(5));
        strcpy(m.blob, "BLOB1");
        m.int64         = CPU_TO_BE(uint64_t(0x1234567855aacc33ULL));
        m.double64      = CPU_TO_BE(double(M_E));
        m.time_tag      = CPU_TO_BE(uint64_t(0xff00cc01aa028803ULL));
        strcpy(m.type, "type01");
        m.ascii         = CPU_TO_BE(uint32_t(uint8_t('\n')));
        m.rgba          = CPU_TO_BE(uint32_t(0x8080ff40));
        m.midi[0]       = 0x94;
        m.midi[1]       = 51;
        m.midi[2]       = 0x7f;
        m.raw_midi[0]   = 0x84;
        m.raw_midi[1]   = 51;
        m.raw_midi[2]   = 0x5f;
        m.array.int32   = CPU_TO_BE(int32_t(0xfffefcfd));
        strcpy(m.array.array.string, "string2");
        m.array.array.blob_size = CPU_TO_BE(uint32_t(6));
        strcpy(m.array.array.blob, "BLOB02");
        m.array.array.int32 = CPU_TO_BE(int32_t(0xcafeface));
        m.array.float32 = CPU_TO_BE(float(440.0f));

        midi_event_t midi;
        midi.timestamp      = 0;
        midi.type           = MIDI_MSG_NOTE_ON;
        midi.channel        = 4;
        midi.note.pitch     = 51;
        midi.note.velocity  = 0x7f;

        uint8_t raw_midi[4];
        raw_midi[0]         = MIDI_MSG_NOTE_OFF | 4;
        raw_midi[1]         = 51;
        raw_midi[2]         = 0x5f;

        // Forge the same message using OSC serializer
        osc::packet_t packet;
        osc::forge_t forge;
        osc::forge_frame_t frame, message, array1, array2;

        UTEST_ASSERT(osc::forge_begin_dynamic(&frame, &forge) == STATUS_OK);
        UTEST_ASSERT(osc::forge_begin_message(&message, &frame, "/address/01") == STATUS_OK);
        {
            UTEST_ASSERT(osc::forge_int32(&message, 0xdeadbeef) == STATUS_OK);
            UTEST_ASSERT(osc::forge_float32(&message, float(M_PI)) == STATUS_OK);
            UTEST_ASSERT(osc::forge_string(&message, "string1") == STATUS_OK);
            UTEST_ASSERT(osc::forge_blob(&message, "BLOB1", 5) == STATUS_OK);
            UTEST_ASSERT(osc::forge_int64(&message, uint64_t(0x1234567855aacc33ULL)) == STATUS_OK);
            UTEST_ASSERT(osc::forge_double64(&message, double(M_E)) == STATUS_OK);
            UTEST_ASSERT(osc::forge_time_tag(&message, uint64_t(0xff00cc01aa028803ULL)) == STATUS_OK);
            UTEST_ASSERT(osc::forge_type(&message, "type01") == STATUS_OK);
            UTEST_ASSERT(osc::forge_ascii(&message, '\n') == STATUS_OK);
            UTEST_ASSERT(osc::forge_rgba(&message, 0x8080ff40) == STATUS_OK);
            UTEST_ASSERT(osc::forge_midi(&message, &midi) == STATUS_OK);
            UTEST_ASSERT(osc::forge_midi_raw(&message, raw_midi, 3) == STATUS_OK);

            UTEST_ASSERT(osc::forge_begin_array(&array1, &message) == STATUS_OK);
            {
                UTEST_ASSERT(osc::forge_int32(&array1, int32_t(0xfffefcfd)) == STATUS_OK);
                UTEST_ASSERT(osc::forge_bool(&array1, true) == STATUS_OK);
                UTEST_ASSERT(osc::forge_begin_array(&array2, &array1) == STATUS_OK);
                {
                    UTEST_ASSERT(osc::forge_string(&array2, "string2") == STATUS_OK);
                    UTEST_ASSERT(osc::forge_blob(&array2, "BLOB02", 6) == STATUS_OK);
                    UTEST_ASSERT(osc::forge_int32(&array2, int32_t(0xcafeface)) == STATUS_OK);
                    UTEST_ASSERT(osc::forge_inf(&array2) == STATUS_OK);
                }
                UTEST_ASSERT(osc::forge_end(&array2) == STATUS_OK);
                UTEST_ASSERT(osc::forge_float32(&array1, float(440.0f)) == STATUS_OK);
            }
            UTEST_ASSERT(osc::forge_end(&array1) == STATUS_OK);

            UTEST_ASSERT(osc::forge_bool(&message, true) == STATUS_OK);
            UTEST_ASSERT(osc::forge_bool(&message, false) == STATUS_OK);
            UTEST_ASSERT(osc::forge_null(&message) == STATUS_OK);
            UTEST_ASSERT(osc::forge_inf(&message) == STATUS_OK);
        }
        UTEST_ASSERT(osc::forge_end(&message) == STATUS_OK);
        UTEST_ASSERT(osc::forge_end(&frame) == STATUS_OK);
        UTEST_ASSERT(osc::forge_close(&packet, &forge) == STATUS_OK);
        UTEST_ASSERT(osc::forge_destroy(&forge) == STATUS_OK);

        // Validate output data
        UTEST_ASSERT(packet.data != NULL);
        bool matches = (packet.size == sizeof(m)) && (::memcmp(packet.data, &m, sizeof(m)) == 0);
        if (!matches)
        {
            dump_bytes("src", &m, sizeof(m));
            dump_bytes("dst", packet.data, packet.size);
            UTEST_FAIL_MSG("Source and destination messages differ");
        }
        osc::forge_free(packet.data);
    }

    void test_forge_message_bundle()
    {
        // Initialize message bundle
        message_bundle_t b;
        bzero(&b, sizeof(b));

        strcpy(b.signature, "#bundle");
        b.time_tag  = CPU_TO_BE(uint64_t(0x1122334455667788ULL));

        b.bundle1_size              = CPU_TO_BE(uint32_t(sizeof(b.bundle1)));
        {
            strcpy(b.bundle1.signature, "#bundle");
            b.bundle1.time_tag          = CPU_TO_BE(uint64_t(0x2233445566778899ULL));

            b.bundle1.message1_size     = CPU_TO_BE(uint32_t(sizeof(b.bundle1.message1)));
            {
                strcpy(b.bundle1.message1.address, "/a0");
                strcpy(b.bundle1.message1.types, ",i");
                b.bundle1.message1.int32    = CPU_TO_BE(int32_t(0xdeadbeef));
            }

            b.bundle1.message2_size     = CPU_TO_BE(uint32_t(sizeof(b.bundle1.message2)));
            {
                strcpy(b.bundle1.message2.address, "/addr0");
                strcpy(b.bundle1.message2.types, ",TFif");
                b.bundle1.message2.int32    = CPU_TO_BE(int32_t(0xcafeface));
                b.bundle1.message2.float32  = CPU_TO_BE(float(440.0f));
            }
        }

        b.bundle2_size              = CPU_TO_BE(uint32_t(sizeof(b.bundle2)));
        {
            strcpy(b.bundle2.signature, "#bundle");
            b.bundle2.time_tag          = CPU_TO_BE(uint64_t(0x5566778899aabbccULL));

            b.bundle2.message1_size     = CPU_TO_BE(uint32_t(sizeof(b.bundle2.message1)));
            {
                strcpy(b.bundle2.message1.address, "/b0");
                strcpy(b.bundle2.message1.types, ",Ii");
                b.bundle2.message1.int32    = CPU_TO_BE(int32_t(0xf00dfeed));
            }

            b.bundle2.message2_size     = CPU_TO_BE(uint32_t(sizeof(b.bundle2.message2)));
            {
                strcpy(b.bundle2.message2.address, "/c0");
                strcpy(b.bundle2.message2.types, ",TFN");
            }
        }

        b.message_size              = CPU_TO_BE(uint32_t(sizeof(b.message)));
        {
            strcpy(b.message.address, "/addr1");
            strcpy(b.message.types, ",TFiNI");
            b.message.int32             = CPU_TO_BE(int32_t(0x55aacc33));
        }

        // Serialize message
        osc::packet_t packet;
        osc::forge_t forge;
        osc::forge_frame_t frame, bundle1, bundle2, message;

        UTEST_ASSERT(osc::forge_begin_dynamic(&frame, &forge) == STATUS_OK);

        UTEST_ASSERT(osc::forge_begin_bundle(&bundle1, &frame, uint64_t(0x1122334455667788ULL)) == STATUS_OK);
        {
            // Sub-bundle 1
            UTEST_ASSERT(osc::forge_begin_bundle(&bundle2, &bundle1, uint64_t(0x2233445566778899ULL)) == STATUS_OK);
            {
                UTEST_ASSERT(osc::forge_begin_message(&message, &bundle2, "/a0") == STATUS_OK);
                {
                    UTEST_ASSERT(osc::forge_int32(&message, int32_t(0xdeadbeef)) == STATUS_OK);
                }
                UTEST_ASSERT(osc::forge_end(&message) == STATUS_OK);

                UTEST_ASSERT(osc::forge_begin_message(&message, &bundle2, "/addr0") == STATUS_OK);
                {
                    UTEST_ASSERT(osc::forge_bool(&message, true) == STATUS_OK);
                    UTEST_ASSERT(osc::forge_bool(&message, false) == STATUS_OK);
                    UTEST_ASSERT(osc::forge_int32(&message, int32_t(0xcafeface)) == STATUS_OK);
                    UTEST_ASSERT(osc::forge_float32(&message, float(440.0f)) == STATUS_OK);
                }
                UTEST_ASSERT(osc::forge_end(&message) == STATUS_OK);
            }
            UTEST_ASSERT(osc::forge_end(&bundle2) == STATUS_OK);

            // Sub-bundle 2
            UTEST_ASSERT(osc::forge_begin_bundle(&bundle2, &bundle1, uint64_t(0x5566778899aabbccULL)) == STATUS_OK);
            {
                UTEST_ASSERT(osc::forge_begin_message(&message, &bundle2, "/b0") == STATUS_OK);
                {
                    UTEST_ASSERT(osc::forge_inf(&message) == STATUS_OK);
                    UTEST_ASSERT(osc::forge_int32(&message, int32_t(0xf00dfeed)) == STATUS_OK);
                }
                UTEST_ASSERT(osc::forge_end(&message) == STATUS_OK);

                UTEST_ASSERT(osc::forge_begin_message(&message, &bundle2, "/c0") == STATUS_OK);
                {
                    UTEST_ASSERT(osc::forge_bool(&message, true) == STATUS_OK);
                    UTEST_ASSERT(osc::forge_bool(&message, false) == STATUS_OK);
                    UTEST_ASSERT(osc::forge_null(&message) == STATUS_OK);
                }
                UTEST_ASSERT(osc::forge_end(&message) == STATUS_OK);
            }
            UTEST_ASSERT(osc::forge_end(&bundle2) == STATUS_OK);

            // Sub-message
            UTEST_ASSERT(osc::forge_begin_message(&message, &bundle1, "/addr1") == STATUS_OK);
            {
                UTEST_ASSERT(osc::forge_bool(&message, true) == STATUS_OK);
                UTEST_ASSERT(osc::forge_bool(&message, false) == STATUS_OK);
                UTEST_ASSERT(osc::forge_int32(&message, int32_t(0x55aacc33)) == STATUS_OK);
                UTEST_ASSERT(osc::forge_null(&message) == STATUS_OK);
                UTEST_ASSERT(osc::forge_inf(&message) == STATUS_OK);
            }
            UTEST_ASSERT(osc::forge_end(&message) == STATUS_OK);
        }
        UTEST_ASSERT(osc::forge_end(&bundle1) == STATUS_OK);

        UTEST_ASSERT(osc::forge_end(&frame) == STATUS_OK);
        UTEST_ASSERT(osc::forge_close(&packet, &forge) == STATUS_OK);
        UTEST_ASSERT(osc::forge_destroy(&forge) == STATUS_OK);

        // Validate output data
        UTEST_ASSERT(packet.data != NULL);
        bool matches = (packet.size == sizeof(b)) && (::memcmp(packet.data, &b, sizeof(b)) == 0);
        if (!matches)
        {
            dump_bytes("src", &b, sizeof(b));
            dump_bytes("dst", packet.data, packet.size);
            UTEST_FAIL_MSG("Source and destination messages differ");
        }
        osc::forge_free(packet.data);
    }

    void test_forge_overflow()
    {
        uint8_t buf[22];

        struct
        {
            char        addr[12];
            char        types[4];
            int32_t     int32;
        } m;

        bzero(&m, sizeof(m));
        strcpy(m.addr, "/message");
        strcpy(m.types, ",i");
        m.int32     = CPU_TO_BE(uint32_t(0xdeadcafe));

        osc::packet_t packet;
        osc::forge_t forge;
        osc::forge_frame_t frame, message;

        UTEST_ASSERT(osc::forge_begin_fixed(&frame, &forge, buf, sizeof(buf)) == STATUS_OK);
        {
            UTEST_ASSERT(osc::forge_begin_message(&message, &frame, "/message") == STATUS_OK);
            {
                UTEST_ASSERT(osc::forge_int32(&message, 0xdeadcafe) == STATUS_OK);
                UTEST_ASSERT(osc::forge_int32(&message, 0xdeadbeef) == STATUS_OVERFLOW);
            }
            UTEST_ASSERT(osc::forge_end(&message) == STATUS_OK);
        }
        UTEST_ASSERT(osc::forge_end(&frame) == STATUS_OK);
        UTEST_ASSERT(osc::forge_close(&packet, &forge) == STATUS_OK);
        UTEST_ASSERT(osc::forge_destroy(&forge) == STATUS_OK);

        // Validate output data
        UTEST_ASSERT(packet.data != NULL);
        bool matches = (packet.size == sizeof(m)) && (::memcmp(packet.data, &m, sizeof(m)) == 0);
        if (!matches)
        {
            dump_bytes("src", &m, sizeof(m));
            dump_bytes("dst", packet.data, packet.size);
            UTEST_FAIL_MSG("Source and destination messages differ");
        }
    }

    void test_forge_format()
    {
        format_message_t m;

        bzero(&m, sizeof(m));
        strcpy(m.address, "/test/msg");
        strcpy(m.types, ",ifshtdScrFNNI");
        m.int32     = CPU_TO_BE(uint32_t(0xdeadbeef));
        m.float32   = CPU_TO_BE(float(440.0f));
        strcpy(m.string, "some_string");
        m.int64     = CPU_TO_BE(int64_t(0x1122334455667788ULL));
        m.time_tag  = CPU_TO_BE(uint64_t(0xcafebabecafefaceULL));
        m.double64  = CPU_TO_BE(double(M_PI));
        strcpy(m.type, "some_type");
        m.ascii     = CPU_TO_BE(uint32_t('\n'));
        m.rgba      = CPU_TO_BE(uint32_t(0xccffee44));

        osc::packet_t packet;
        osc::forge_t forge;
        osc::forge_frame_t frame;

        UTEST_ASSERT(osc::forge_begin_dynamic(&frame, &forge) == STATUS_OK);
        {
            status_t res = osc::forge_message(&frame, "/test/msg", "ifshtdScrTsSf",
                    uint32_t(0xdeadbeef),
                    float(440.0f),
                    "some_string",
                    int64_t(0x1122334455667788ULL),
                    uint64_t(0xcafebabecafefaceULL),
                    double(M_PI),
                    "some_type",
                    '\n',
                    uint32_t(0xccffee44),
                    false,
                    NULL,
                    NULL,
                    INFINITY
                );

            UTEST_ASSERT(res == STATUS_OK);
        }
        UTEST_ASSERT(osc::forge_end(&frame) == STATUS_OK);
        UTEST_ASSERT(osc::forge_close(&packet, &forge) == STATUS_OK);
        UTEST_ASSERT(osc::forge_destroy(&forge) == STATUS_OK);

        // Validate output data
        UTEST_ASSERT(packet.data != NULL);
        bool matches = (packet.size == sizeof(m)) && (::memcmp(packet.data, &m, sizeof(m)) == 0);
        if (!matches)
        {
            dump_bytes("src", &m, sizeof(m));
            dump_bytes("dst", packet.data, packet.size);
            UTEST_FAIL_MSG("Source and destination messages differ");
        }
        osc::forge_free(packet.data);
    }

    void test_parse_legacy_message()
    {
        osc::parser_t parser;
        osc::parser_frame_t frame, message;
        osc::parser_token_t token;
        uint64_t time_tag;
        char address[80];

        UTEST_ASSERT(osc::parse_begin(&frame, &parser, legacy_message, sizeof(legacy_message) - 1) != STATUS_OK);
        UTEST_ASSERT(osc::parse_begin(&frame, &parser, NULL, sizeof(legacy_message) - 1) != STATUS_OK);
        UTEST_ASSERT(osc::parse_begin(&frame, &parser, legacy_message, sizeof(legacy_message)) == STATUS_OK);
        {
            UTEST_ASSERT(osc::parse_token(&frame, &token) == STATUS_OK);
            UTEST_ASSERT(token == osc::PT_MESSAGE);

            UTEST_ASSERT(osc::parse_begin_bundle(&message, &frame, &time_tag) != STATUS_OK);
            UTEST_ASSERT(osc::parse_begin_array(&message, &frame) != STATUS_OK);
            UTEST_ASSERT(osc::parse_begin_message(&frame, &frame, address, sizeof(address)) != STATUS_OK);
            UTEST_ASSERT(osc::parse_begin_message(&message, &frame, address, 5) == STATUS_OVERFLOW);

            UTEST_ASSERT(osc::parse_begin_message(&message, &frame, address, 6) == STATUS_OK);
            {
                UTEST_ASSERT(strcmp(address, "/test") == 0);
                UTEST_ASSERT(osc::parse_token(&frame, &token) != STATUS_OK);
                UTEST_ASSERT(osc::parse_token(&message, &token) == STATUS_OK);
                UTEST_ASSERT(token == osc::PT_EOR);
            }
            UTEST_ASSERT(osc::parse_end(&frame) != STATUS_OK);
            UTEST_ASSERT(osc::parse_end(&message) == STATUS_OK);
            UTEST_ASSERT(osc::parse_end(&message) != STATUS_OK);

            UTEST_ASSERT(osc::parse_token(&frame, &token) == STATUS_OK);
            UTEST_ASSERT(token == osc::PT_EOR);
        }
        UTEST_ASSERT(osc::parse_end(&frame) == STATUS_OK);
        UTEST_ASSERT(osc::parse_end(&frame) != STATUS_OK);
        UTEST_ASSERT(osc::parse_destroy(&parser) == STATUS_OK);
    }

    void test_parse_simple_message()
    {
        osc::parser_t parser;
        osc::parser_frame_t frame, message, subframe;
        osc::parser_token_t token;
        char address[80];

        union
        {
            uint32_t    u32;
            float       f32;
            double      d64;
            int32_t     i32;
            uint64_t    u64;
            int64_t     i64;
            size_t      sz;
            char        ch;
            midi_event_t midi;
            bool        tf;
        } xptr;

        UTEST_ASSERT(osc::parse_begin(&frame, &parser, simple_message, sizeof(simple_message)) == STATUS_OK);
        {
            UTEST_ASSERT(osc::parse_begin_message(&message, &frame, address, sizeof(address)) == STATUS_OK);
            {
                UTEST_ASSERT(strcmp(address, "/oscillator/4/frequency") == 0);
                UTEST_ASSERT(osc::parse_token(&message, &token) == STATUS_OK);
                UTEST_ASSERT(token == osc::PT_FLOAT32);

                UTEST_ASSERT(osc::parse_begin_bundle(&subframe, &message, &xptr.u64) != STATUS_OK);
                UTEST_ASSERT(osc::parse_begin_message(&subframe, &message, address, sizeof(address)) != STATUS_OK);
                UTEST_ASSERT(osc::parse_begin_array(&subframe, &message) != STATUS_OK);

                UTEST_ASSERT(osc::parse_int32(&message, &xptr.i32) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_string(&message, address, sizeof(address)) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_blob(&message, address, &xptr.sz, sizeof(address)) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_int64(&message, &xptr.i64) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_double64(&message, &xptr.d64) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_time_tag(&message, &xptr.u64) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_type(&message, address, sizeof(address)) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_ascii(&message, &xptr.ch) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_rgba(&message, &xptr.u32) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_midi(&message, &xptr.midi) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_midi_raw(&message, address, &xptr.sz) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_bool(&message, &xptr.tf) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_null(&message) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_inf(&message) == STATUS_BAD_TYPE);

                UTEST_ASSERT(osc::parse_float32(&message, &xptr.f32) == STATUS_OK);
                UTEST_ASSERT(xptr.f32 == 440.0f);

                UTEST_ASSERT(osc::parse_token(&message, &token) == STATUS_OK);
                UTEST_ASSERT(token == osc::PT_EOR);

                UTEST_ASSERT(osc::parse_int32(&message, &xptr.i32) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_string(&message, address, sizeof(address)) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_blob(&message, address, &xptr.sz, sizeof(address)) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_int64(&message, &xptr.i64) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_double64(&message, &xptr.d64) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_time_tag(&message, &xptr.u64) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_type(&message, address, sizeof(address)) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_ascii(&message, &xptr.ch) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_rgba(&message, &xptr.u32) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_midi(&message, &xptr.midi) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_midi_raw(&message, address, &xptr.sz) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_bool(&message, &xptr.tf) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_null(&message) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_inf(&message) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_float32(&message, &xptr.f32) == STATUS_EOF);

            }
            UTEST_ASSERT(osc::parse_end(&frame) != STATUS_OK);
            UTEST_ASSERT(osc::parse_end(&message) == STATUS_OK);
            UTEST_ASSERT(osc::parse_end(&message) != STATUS_OK);

            UTEST_ASSERT(osc::parse_token(&frame, &token) == STATUS_OK);
            UTEST_ASSERT(token == osc::PT_EOR);
        }
        UTEST_ASSERT(osc::parse_end(&frame) == STATUS_OK);
        UTEST_ASSERT(osc::parse_end(&frame) != STATUS_OK);
        UTEST_ASSERT(osc::parse_destroy(&parser) == STATUS_OK);
    }

    void test_parse_specials_message()
    {
        special_message_t m;
        bzero(&m, sizeof(m));
        strcpy(m.address, "/message001");
        strcpy(m.types, ",NNNNNNNNNNNNNNNIIII");

        // Parse the message
        osc::parser_t parser;
        osc::parser_frame_t frame, message;
        osc::parser_token_t token;
        char address[80];

        float f32 = 0.0f;
        double f64 = 0.0;

        UTEST_ASSERT(osc::parse_begin(&frame, &parser, &m, sizeof(m)) == STATUS_OK);
        {
            UTEST_ASSERT(osc::parse_begin_message(&message, &frame, address, sizeof(address)) == STATUS_OK);
            {
                UTEST_ASSERT(strcmp(address, "/message001") == 0);
                UTEST_ASSERT(osc::parse_token(&message, &token) == STATUS_OK);
                UTEST_ASSERT(token == osc::PT_NULL);

                UTEST_ASSERT(osc::parse_int32(&message, NULL) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_float32(&message, NULL) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_string(&message, NULL, 0) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_blob(&message, NULL, NULL, 0) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_int64(&message, NULL) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_double64(&message, NULL) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_time_tag(&message, NULL) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_type(&message, NULL, 0) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_ascii(&message, NULL) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_rgba(&message, NULL) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_midi(&message, NULL) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_midi_raw(&message, NULL, 0) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_bool(&message, NULL) == STATUS_NULL);
                UTEST_ASSERT(osc::parse_null(&message) == STATUS_OK);
                UTEST_ASSERT(osc::parse_inf(&message) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_null(&message) == STATUS_OK);

                UTEST_ASSERT(osc::parse_int32(&message, NULL) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_inf(&message) == STATUS_OK);
                UTEST_ASSERT(osc::parse_null(&message) == STATUS_BAD_TYPE);
                UTEST_ASSERT(osc::parse_inf(&message) == STATUS_OK);

                UTEST_ASSERT(osc::parse_float32(&message, &f32) == STATUS_OK);
                UTEST_ASSERT(isinf(f32));
                UTEST_ASSERT(osc::parse_double64(&message, &f64) == STATUS_OK);
                UTEST_ASSERT(isinf(f64));

                UTEST_ASSERT(osc::parse_null(&message) == STATUS_EOF);
                UTEST_ASSERT(osc::parse_token(&message, &token) == STATUS_OK);
                UTEST_ASSERT(token == osc::PT_EOR);
            }
            UTEST_ASSERT(osc::parse_end(&message) == STATUS_OK);

            UTEST_ASSERT(osc::parse_token(&frame, &token) == STATUS_OK);
            UTEST_ASSERT(token == osc::PT_EOR);
        }
        UTEST_ASSERT(osc::parse_end(&frame) == STATUS_OK);
        UTEST_ASSERT(osc::parse_destroy(&parser) == STATUS_OK);
    }

    UTEST_MAIN
    {
        test_forge_simple_message();
        test_forge_parameters();
        test_forge_message_bundle();
        test_forge_overflow();
        test_forge_format();

        test_parse_legacy_message();
        test_parse_simple_message();
        test_parse_specials_message();
    }
UTEST_END;


