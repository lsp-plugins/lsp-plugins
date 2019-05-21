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

UTEST_BEGIN("core.protocol", osc)
    void test_simple_message()
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
        UTEST_ASSERT(packet.size == sizeof(simple_message));
        UTEST_ASSERT(::memcmp(packet.data, simple_message, sizeof(simple_message)) == 0);
        osc::forge_free(packet.data);
    }

    void test_parameters()
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
        m.ascii         = uint8_t('\n');
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

    UTEST_MAIN
    {
//        test_simple_message();
        test_parameters();
    }
UTEST_END;


