/*
 * midi.cpp
 *
 *  Created on: 14 марта 2016 г.
 *      Author: sadko
 */

#include <stdlib.h>
#include <core/types.h>
#include <core/protocol/midi.h>

namespace lsp
{
    static int compare_midi(const void *p1, const void *p2)
    {
        const midi::event_t *e1 = reinterpret_cast<const midi::event_t *>(p1);
        const midi::event_t *e2 = reinterpret_cast<const midi::event_t *>(p2);
        return (e1->timestamp < e2->timestamp) ? -1 :
                (e1->timestamp > e2->timestamp) ? 1 : 0;
    }

    void midi_t::sort()
    {
        if (nEvents > 1)
            ::qsort(vEvents, sizeof(midi::event_t), nEvents, compare_midi);
    }

    namespace midi
    {
        static size_t decode_system_message(event_t *ev, const uint8_t *b)
        {
            ssize_t size;

            switch (b[0])
            {
                case MIDI_MSG_SYSTEM_EXCLUSIVE: // TODO
                    return -STATUS_NOT_IMPLEMENTED;

                case MIDI_MSG_MTC_QUARTER:
                    if ((b[1]) & 0x80)
                        return -STATUS_CORRUPTED;
                    ev->mtc.type            = b[1] >> 4;
                    ev->mtc.value           = b[1] & 0x0f;
                    size                    = 2;
                    break;

                case MIDI_MSG_SONG_POS:
                    if ((b[1]) & 0x80)
                        return -STATUS_CORRUPTED;
                    if ((b[2]) & 0x80)
                        return -STATUS_CORRUPTED;
                    ev->beats               = uint16_t(b[2] << 7) | uint16_t(b[1]);
                    size                    = 3;
                    break;

                case MIDI_MSG_SONG_SELECT:
                    if ((b[1]) & 0x80)
                        return -STATUS_CORRUPTED;
                    ev->song                = b[1];
                    ev->bparams[1]          = 0;
                    size                    = 2;
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
                    size                    = 1;
                    break;

                default:
                    return -STATUS_BAD_FORMAT;
            }

            ev->timestamp           = 0;
            ev->type                = b[0];
            ev->channel             = 0;

            return size;
        }

        ssize_t decode(event_t *ev, const uint8_t *b)
        {
            ssize_t size;
            if (!((b[0]) & 0x80))
                return - STATUS_CORRUPTED;

            switch ((b[0]) & 0xf0)
            {
                // Channel messages
                case MIDI_MSG_NOTE_OFF:
                case MIDI_MSG_NOTE_ON:
                    if ((b[1]) & 0x80)
                        return -STATUS_CORRUPTED;
                    if ((b[2]) & 0x80)
                        return -STATUS_CORRUPTED;
                    ev->note.pitch          = b[1];
                    ev->note.velocity       = b[2];
                    size                    = 3;
                    break;

                case MIDI_MSG_NOTE_CONTROLLER:
                    if ((b[1]) & 0x80)
                        return -STATUS_CORRUPTED;
                    if ((b[2]) & 0x80)
                        return -STATUS_CORRUPTED;
                    ev->ctl.control         = b[1];
                    ev->ctl.value           = b[2];
                    size                    = 3;
                    break;

                case MIDI_MSG_NOTE_PRESSURE:
                    if ((b[1]) & 0x80)
                        return -STATUS_CORRUPTED;
                    if ((b[2]) & 0x80)
                        return -STATUS_CORRUPTED;
                    ev->atouch.pitch        = b[1];
                    ev->atouch.pressure     = b[2];
                    size                    = 3;
                    break;

                case MIDI_MSG_PROGRAM_CHANGE:
                    if ((b[1]) & 0x80)
                        return -STATUS_CORRUPTED;
                    ev->program             = b[1];
                    size                    = 2;
                    break;

                case MIDI_MSG_CHANNEL_PRESSURE:
                    if ((b[1]) & 0x80)
                        return -STATUS_CORRUPTED;
                    ev->chn.pressure        = b[1];
                    size                    = 2;
                    break;

                case MIDI_MSG_PITCH_BEND:
                    if ((b[1]) & 0x80)
                        return -STATUS_CORRUPTED;
                    if ((b[2]) & 0x80)
                        return -STATUS_CORRUPTED;
                    ev->bend                = uint16_t(b[1] << 7) | uint16_t(b[2]);
                    size                    = 3;
                    break;

                default:
                    return decode_system_message(ev, b);
            }

            ev->type                = b[0] & 0xf0;
            ev->channel             = b[0] & 0x0f;
            ev->timestamp           = 0;
            return size;
        }

        ssize_t encode(uint8_t *bytes, const event_t *ev)
        {
            if (!(ev->type & 0x80))
                return -STATUS_BAD_FORMAT;

            switch (ev->type)
            {
                case MIDI_MSG_NOTE_OFF:
                case MIDI_MSG_NOTE_ON:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->note.pitch >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    if (ev->note.velocity >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    bytes[0]    = ev->type | ev->channel;
                    bytes[1]    = ev->note.pitch;
                    bytes[2]    = ev->note.velocity;
                    return 3;

                case MIDI_MSG_NOTE_PRESSURE:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->atouch.pitch >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    if (ev->atouch.pressure >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    bytes[0]    = ev->type | ev->channel;
                    bytes[1]    = ev->atouch.pitch;
                    bytes[2]    = ev->atouch.pressure;
                    return 3;

                case MIDI_MSG_NOTE_CONTROLLER:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->ctl.control >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    if (ev->ctl.value >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    bytes[0]    = ev->type | ev->channel;
                    bytes[1]    = ev->ctl.control;
                    bytes[2]    = ev->ctl.value;
                    return 3;

                case MIDI_MSG_PROGRAM_CHANGE:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->program >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    bytes[0]    = ev->type | ev->channel;
                    bytes[1]    = ev->program;
                    return 2;

                case MIDI_MSG_CHANNEL_PRESSURE:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->chn.pressure >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    bytes[0]    = ev->type | ev->channel;
                    bytes[1]    = ev->chn.pressure;
                    return 2;

                case MIDI_MSG_PITCH_BEND:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->bend >= 0x4000)
                        return -STATUS_BAD_FORMAT;
                    bytes[0]    = ev->type | ev->channel;
                    bytes[1]    = ev->bend & 0x7f;
                    bytes[2]    = ev->bend >> 7;
                    return 3;

                case MIDI_MSG_SYSTEM_EXCLUSIVE: // TODO
                    return -STATUS_NOT_IMPLEMENTED;

                case MIDI_MSG_MTC_QUARTER:
                    if (ev->mtc.type >= 0x08)
                        return -STATUS_BAD_FORMAT;
                    if (ev->mtc.value >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    bytes[0]    = ev->type;
                    bytes[1]    = (ev->mtc.type << 4) | (ev->mtc.value);
                    return 2;

                case MIDI_MSG_SONG_POS:
                    if (ev->beats >= 0x4000)
                        return -STATUS_BAD_FORMAT;
                    bytes[0]    = ev->type;
                    bytes[1]    = ev->beats & 0x7f;
                    bytes[2]    = ev->beats >> 7;;
                    return 3;

                case MIDI_MSG_SONG_SELECT:
                    if (ev->song >= 0x80)
                        return -STATUS_BAD_FORMAT;
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
                    return -STATUS_BAD_FORMAT;
            }

            return -STATUS_BAD_FORMAT;
        }

        ssize_t size_of(const event_t *ev)
        {
            if (!(ev->type & 0x80))
                return -STATUS_BAD_FORMAT;

            switch (ev->type)
            {
                case MIDI_MSG_NOTE_OFF:
                case MIDI_MSG_NOTE_ON:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->note.pitch >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    if (ev->note.velocity >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    return 3;

                case MIDI_MSG_NOTE_PRESSURE:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->atouch.pitch >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    if (ev->atouch.pressure >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    return 3;

                case MIDI_MSG_NOTE_CONTROLLER:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->ctl.control >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    if (ev->ctl.value >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    return 3;

                case MIDI_MSG_PROGRAM_CHANGE:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->program >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    return 2;

                case MIDI_MSG_CHANNEL_PRESSURE:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->chn.pressure >= 0x80)
                        return -STATUS_BAD_FORMAT;
                    return 2;

                case MIDI_MSG_PITCH_BEND:
                    if (ev->channel >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    if (ev->bend >= 0x4000)
                        return -STATUS_BAD_FORMAT;
                    return 3;

                case MIDI_MSG_SYSTEM_EXCLUSIVE: // TODO
                    return -STATUS_NOT_IMPLEMENTED;

                case MIDI_MSG_MTC_QUARTER:
                    if (ev->mtc.type >= 0x08)
                        return -STATUS_BAD_FORMAT;
                    if (ev->mtc.value >= 0x10)
                        return -STATUS_BAD_FORMAT;
                    return 2;

                case MIDI_MSG_SONG_POS:
                    if (ev->beats >= 0x4000)
                        return -STATUS_BAD_FORMAT;
                    return 3;

                case MIDI_MSG_SONG_SELECT:
                    if (ev->song >= 0x80)
                        return -STATUS_BAD_FORMAT;
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
                    return -STATUS_BAD_FORMAT;
            }

            return -STATUS_BAD_FORMAT;
        }
    }
}


