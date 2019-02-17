/*
 * midi.h
 *
 *  Created on: 14 марта 2016 г.
 *      Author: sadko
 */

#ifndef CORE_MIDI_H_
#define CORE_MIDI_H_

#include <core/types.h>

namespace lsp
{
    // Generalized midi event structure
#pragma pack(push, 1)
    typedef struct midi_event_t
    {
        uint32_t        timestamp;      // Timestamp
        uint8_t         type;           // Type of event
        uint8_t         channel;        // ID of the MIDI channel
        union
        {
            uint8_t         bparams[2];     // Byte parameters
            uint16_t        wparam;         // Word parameter

            struct
            {
                uint8_t         pitch;           // Note key
                uint8_t         velocity;      // Note velocity
            } note;

            struct
            {
                uint8_t         control;       // Control
                uint8_t         value;         // Value
            } ctl;

            uint8_t         program;         // program
            uint8_t         aftertouch;      // pressure
            uint16_t        bend;

            struct
            {
                uint8_t         type;
                uint8_t         value;
            } mtc;

            uint16_t        beats;
            uint8_t         song;
        };
    } midi_event_t;
#pragma pack(pop)

    // Midi port structure
    typedef struct midi_t
    {
        size_t          nEvents;
        midi_event_t    vEvents[MIDI_EVENTS_MAX];

        inline bool push(const midi_event_t *me)
        {
            if (nEvents >= MIDI_EVENTS_MAX)
                return false;
            vEvents[nEvents++]      = *me;
            return true;
        }

        inline bool push(const midi_event_t &me)
        {
            return push(&me);
        }

        inline bool push_all(const midi_t *src)
        {
            size_t avail    = MIDI_EVENTS_MAX - nEvents;
            size_t count    = (src->nEvents > avail) ? avail : src->nEvents;
            if (count > 0)
            {
                ::memcpy(&vEvents[nEvents], src->vEvents, count * sizeof(midi_event_t));
                nEvents        += count;
            }

            return count >= src->nEvents;
        }

        inline bool push_all(const midi_t &src)
        {
            return push_all(&src);
        }

        inline void copy_from(const midi_t *src)
        {
            nEvents     = src->nEvents;
            if (nEvents > 0)
                ::memcpy(vEvents, src->vEvents, nEvents * sizeof(midi_event_t));
        }

        inline void copy_to(midi_t *dst) const
        {
            dst->copy_from(this);
        }

        inline void clear()
        {
            nEvents     = 0;
        }

        void sort();
    } midi_t;

    enum midi_message_t
    {
        MIDI_MSG_NOTE_OFF               = 0x80,
        MIDI_MSG_NOTE_ON                = 0x90,
        MIDI_MSG_NOTE_PRESSURE          = 0xa0,
        MIDI_MSG_NOTE_CONTROLLER        = 0xb0,
        MIDI_MSG_PROGRAM_CHANGE         = 0xc0,
        MIDI_MSG_CHANNEL_PRESSURE       = 0xd0,
        MIDI_MSG_PITCH_BEND             = 0xe0,
        MIDI_MSG_SYSTEM_EXCLUSIVE       = 0xf0,
        MIDI_MSG_MTC_QUARTER            = 0xf1,
        MIDI_MSG_SONG_POS               = 0xf2,
        MIDI_MSG_SONG_SELECT            = 0xf3,
        MIDI_MSG_TUNE_REQUEST           = 0xf6,
        MIDI_MSG_END_EXCLUSIVE          = 0xf7,
        MIDI_MSG_CLOCK                  = 0xf8,
        MIDI_MSG_START                  = 0xfa,
        MIDI_MSG_CONTINUE               = 0xfb,
        MIDI_MSG_STOP                   = 0xfc,
        MIDI_MSG_ACTIVE_SENSING         = 0xfe,
        MIDI_MSG_RESET                  = 0xff
    };

    enum midi_controller_t
    {
        MIDI_CTL_MSB_BANK               = 0x00,
        MIDI_CTL_MSB_MODWHEEL           = 0x01,
        MIDI_CTL_MSB_BREATH             = 0x02,
        MIDI_CTL_MSB_FOOT               = 0x04,
        MIDI_CTL_MSB_PORTAMENTO_TIME    = 0x05,
        MIDI_CTL_MSB_DATA_ENTRY         = 0x06,
        MIDI_CTL_MSB_MAIN_VOLUME        = 0x07,
        MIDI_CTL_MSB_BALANCE            = 0x08,
        MIDI_CTL_MSB_PAN                = 0x0a,
        MIDI_CTL_MSB_EXPRESSION         = 0x0b,
        MIDI_CTL_MSB_EFFECT1            = 0x0c,
        MIDI_CTL_MSB_EFFECT2            = 0x0d,
        MIDI_CTL_MSB_GENERAL_PURPOSE1   = 0x10,
        MIDI_CTL_MSB_GENERAL_PURPOSE2   = 0x11,
        MIDI_CTL_MSB_GENERAL_PURPOSE3   = 0x12,
        MIDI_CTL_MSB_GENERAL_PURPOSE4   = 0x13,
        MIDI_CTL_LSB_BANK               = 0x20,
        MIDI_CTL_LSB_MODWHEEL           = 0x21,
        MIDI_CTL_LSB_BREATH             = 0x22,
        MIDI_CTL_LSB_FOOT               = 0x24,
        MIDI_CTL_LSB_PORTAMENTO_TIME    = 0x25,
        MIDI_CTL_LSB_DATA_ENTRY         = 0x26,
        MIDI_CTL_LSB_MAIN_VOLUME        = 0x27,
        MIDI_CTL_LSB_BALANCE            = 0x28,
        MIDI_CTL_LSB_PAN                = 0x2a,
        MIDI_CTL_LSB_EXPRESSION         = 0x2b,
        MIDI_CTL_LSB_EFFECT1            = 0x2c,
        MIDI_CTL_LSB_EFFECT2            = 0x2d,
        MIDI_CTL_LSB_GENERAL_PURPOSE1   = 0x30,
        MIDI_CTL_LSB_GENERAL_PURPOSE2   = 0x31,
        MIDI_CTL_LSB_GENERAL_PURPOSE3   = 0x32,
        MIDI_CTL_LSB_GENERAL_PURPOSE4   = 0x33,
        MIDI_CTL_SUSTAIN                = 0x40,
        MIDI_CTL_PORTAMENTO             = 0x41,
        MIDI_CTL_SOSTENUTO              = 0x42,
        MIDI_CTL_SOFT_PEDAL             = 0x43,
        MIDI_CTL_LEGATO_FOOTSWITCH      = 0x44,
        MIDI_CTL_HOLD2                  = 0x45,
        MIDI_CTL_SC1_SOUND_VARIATION    = 0x46,
        MIDI_CTL_SC2_TIMBRE             = 0x47,
        MIDI_CTL_SC3_RELEASE_TIME       = 0x48,
        MIDI_CTL_SC4_ATTACK_TIME        = 0x49,
        MIDI_CTL_SC5_BRIGHTNESS         = 0x4a,
        MIDI_CTL_SC6                    = 0x4b,
        MIDI_CTL_SC7                    = 0x4c,
        MIDI_CTL_SC8                    = 0x4d,
        MIDI_CTL_SC9                    = 0x4e,
        MIDI_CTL_SC10                   = 0x4f,
        MIDI_CTL_GENERAL_PURPOSE5       = 0x50,
        MIDI_CTL_GENERAL_PURPOSE6       = 0x51,
        MIDI_CTL_GENERAL_PURPOSE7       = 0x52,
        MIDI_CTL_GENERAL_PURPOSE8       = 0x53,
        MIDI_CTL_PORTAMENTO_CONTROL     = 0x54,
        MIDI_CTL_E1_REVERB_DEPTH        = 0x5b,
        MIDI_CTL_E2_TREMOLO_DEPTH       = 0x5c,
        MIDI_CTL_E3_CHORUS_DEPTH        = 0x5d,
        MIDI_CTL_E4_DETUNE_DEPTH        = 0x5e,
        MIDI_CTL_E5_PHASER_DEPTH        = 0x5f,
        MIDI_CTL_DATA_INCREMENT         = 0x60,
        MIDI_CTL_DATA_DECREMENT         = 0x61,
        MIDI_CTL_NRPN_LSB               = 0x62,
        MIDI_CTL_NRPN_MSB               = 0x63,
        MIDI_CTL_RPN_LSB                = 0x64,
        MIDI_CTL_RPN_MSB                = 0x65,
        MIDI_CTL_ALL_SOUNDS_OFF         = 0x78,
        MIDI_CTL_RESET_CONTROLLERS      = 0x79,
        MIDI_CTL_LOCAL_CONTROL_SWITCH   = 0x7a,
        MIDI_CTL_ALL_NOTES_OFF          = 0x7b,
        MIDI_CTL_OMNI_OFF               = 0x7c,
        MIDI_CTL_OMNI_ON                = 0x7d,
        MIDI_CTL_MONO1                  = 0x7e,
        MIDI_CTL_MONO2                  = 0x7f
    };

    bool decode_midi_message(midi_event_t *ev, const uint8_t *bytes);

    size_t encode_midi_message(const midi_event_t *ev, uint8_t *bytes);

    size_t encoded_midi_message_size(const midi_event_t *ev);
}

#endif /* CORE_MIDI_H_ */
