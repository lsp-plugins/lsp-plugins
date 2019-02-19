/*
 * midi.cpp
 *
 *  Created on: 14 марта 2016 г.
 *      Author: sadko
 */

#include <core/midi.h>
#include <stdlib.h>

namespace lsp
{
    static int compare_midi(const void *p1, const void *p2)
    {
        const midi_event_t *e1 = reinterpret_cast<const midi_event_t *>(p1);
        const midi_event_t *e2 = reinterpret_cast<const midi_event_t *>(p2);
        return (e1->timestamp < e2->timestamp) ? -1 :
                (e1->timestamp > e2->timestamp) ? 1 : 0;
    }

    void midi_t::sort()
    {
        if (nEvents > 1)
            ::qsort(vEvents, sizeof(midi_event_t), nEvents, compare_midi);
    }

    static bool decode_system_message(midi_event_t *ev, const uint8_t *b)
    {
        switch (b[0])
        {
            case MIDI_MSG_SYSTEM_EXCLUSIVE: // TODO
                return false;
            case MIDI_MSG_MTC_QUARTER:
                if ((b[1]) & 0x80)
                    return false;
                if ((b[2]) & 0x80)
                    return false;
                ev->mtc.type           = b[1] >> 4;
                ev->mtc.value          = b[1] & 0x0f;
                break;
            case MIDI_MSG_SONG_POS:
                if ((b[1]) & 0x80)
                    return false;
                if ((b[2]) & 0x80)
                    return false;
                ev->wparam              = uint16_t(b[1] << 7) | uint16_t(b[2]);
                break;
            case MIDI_MSG_SONG_SELECT:
                if ((b[1]) & 0x80)
                    return false;
                ev->song                = b[1];
                ev->bparams[1]          = 0;
                break;
            case MIDI_MSG_TUNE_REQUEST:
            case MIDI_MSG_END_EXCLUSIVE:
            case MIDI_MSG_CLOCK:
            case MIDI_MSG_START:
            case MIDI_MSG_CONTINUE:
            case MIDI_MSG_STOP:
            case MIDI_MSG_ACTIVE_SENSING:
            case MIDI_MSG_RESET:
                ev->bparams[0]          = 0;
                ev->bparams[1]          = 0;
                break;
            default:
                return false;
        }
        ev->timestamp           = 0;
        ev->type                = b[0];
        ev->channel             = 0;

        return true;
    }

    bool decode_midi_message(midi_event_t *ev, const uint8_t *b)
    {
        if (!((b[0]) & 0x80))
            return false;

        switch ((b[0]) & 0xf0)
        {
            case MIDI_MSG_NOTE_OFF:
            case MIDI_MSG_NOTE_ON:
            case MIDI_MSG_NOTE_PRESSURE:
            case MIDI_MSG_NOTE_CONTROLLER:
            case MIDI_MSG_PROGRAM_CHANGE:
            case MIDI_MSG_CHANNEL_PRESSURE:
                if ((b[1]) & 0x80)
                    return false;
                if ((b[2]) & 0x80)
                    return false;
                ev->bparams[0]          = b[1];
                ev->bparams[1]          = b[2];
                break;

            case MIDI_MSG_PITCH_BEND:
                if ((b[1]) & 0x80)
                    return false;
                if ((b[2]) & 0x80)
                    return false;
                ev->bend                = uint16_t(b[1] << 7) | uint16_t(b[2]);
                break;

            case MIDI_MSG_SYSTEM_EXCLUSIVE:
                return decode_system_message(ev, b);

            default:
                return false;
        }

        ev->timestamp           = 0;
        ev->type                = b[0] & 0xf0;
        ev->channel             = b[0] & 0x0f;

        return true;
    }

    size_t encode_midi_message(const midi_event_t *ev, uint8_t *bytes)
    {
        if (!(ev->type & 0x80))
            return 0;

        switch (ev->type)
        {
            case MIDI_MSG_NOTE_OFF:
            case MIDI_MSG_NOTE_ON:
            case MIDI_MSG_NOTE_PRESSURE:
            case MIDI_MSG_NOTE_CONTROLLER:
            case MIDI_MSG_PROGRAM_CHANGE:
            case MIDI_MSG_CHANNEL_PRESSURE:
                if (ev->channel >= 0x10)
                    return 0;
                if (ev->bparams[0] >= 0x80)
                    return 0;
                if (ev->bparams[0] >= 0x80)
                    return 0;
                bytes[0]    = ev->type | ev->channel;
                bytes[1]    = ev->bparams[0];
                bytes[2]    = ev->bparams[1];
                return 3;

            case MIDI_MSG_PITCH_BEND:
                if (ev->channel >= 0x10)
                    return 0;
                if (ev->bend >= 0x4000)
                    return 0;
                bytes[0]    = ev->type | ev->channel;
                bytes[1]    = ev->bend >> 7;
                bytes[2]    = ev->bend & 0x7f;
                return 3;

            case MIDI_MSG_SYSTEM_EXCLUSIVE: // TODO
                return 0;

            case MIDI_MSG_MTC_QUARTER:
                if (ev->mtc.type >= 0x08)
                    return 0;
                if (ev->mtc.value >= 0x10)
                    return 0;
                bytes[0]    = ev->type;
                bytes[1]    = (ev->mtc.type << 4) | (ev->mtc.value);
                return 2;

            case MIDI_MSG_SONG_POS:
                if (ev->wparam >= 0x4000)
                    return 0;
                bytes[0]    = ev->type;
                bytes[1]    = ev->wparam >> 7;
                bytes[2]    = ev->wparam & 0x7f;
                return 3;

            case MIDI_MSG_SONG_SELECT:
                if (ev->song >= 0x80)
                    return 0;
                bytes[0]    = ev->type;
                bytes[1]    = ev->song;
                return 2;

            case MIDI_MSG_TUNE_REQUEST:
            case MIDI_MSG_END_EXCLUSIVE:
            case MIDI_MSG_CLOCK:
            case MIDI_MSG_START:
            case MIDI_MSG_CONTINUE:
            case MIDI_MSG_STOP:
            case MIDI_MSG_ACTIVE_SENSING:
            case MIDI_MSG_RESET:
                bytes[0]    = ev->type;
                return 1;

            default:
                return 0;
        }

        return 0;
    }

    size_t encoded_midi_message_size(const midi_event_t *ev)
    {
        if (!(ev->type & 0x80))
            return 0;

        switch (ev->type)
        {
            case MIDI_MSG_NOTE_OFF:
            case MIDI_MSG_NOTE_ON:
            case MIDI_MSG_NOTE_PRESSURE:
            case MIDI_MSG_NOTE_CONTROLLER:
            case MIDI_MSG_PROGRAM_CHANGE:
            case MIDI_MSG_CHANNEL_PRESSURE:
                if (ev->channel >= 0x10)
                    return 0;
                if (ev->bparams[0] >= 0x80)
                    return 0;
                if (ev->bparams[0] >= 0x80)
                    return 0;
                return 3;

            case MIDI_MSG_PITCH_BEND:
                if (ev->channel >= 0x10)
                    return 0;
                if (ev->bend >= 0x4000)
                    return 0;
                return 3;

            case MIDI_MSG_SYSTEM_EXCLUSIVE: // TODO
                return 0;

            case MIDI_MSG_MTC_QUARTER:
                if (ev->mtc.type >= 0x08)
                    return 0;
                if (ev->mtc.value >= 0x10)
                    return 0;
                return 2;

            case MIDI_MSG_SONG_POS:
                if (ev->wparam >= 0x4000)
                    return 0;
                return 3;

            case MIDI_MSG_SONG_SELECT:
                if (ev->song >= 0x80)
                    return 0;
                return 2;

            case MIDI_MSG_TUNE_REQUEST:
            case MIDI_MSG_END_EXCLUSIVE:
            case MIDI_MSG_CLOCK:
            case MIDI_MSG_START:
            case MIDI_MSG_CONTINUE:
            case MIDI_MSG_STOP:
            case MIDI_MSG_ACTIVE_SENSING:
            case MIDI_MSG_RESET:
                return 1;

            default:
                return 0;
        }

        return 0;
    }
}


