/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 мая 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CORE_PROTOCOL_OSC_TYPES_H_
#define CORE_PROTOCOL_OSC_TYPES_H_

#include <core/types.h>
#include <core/status.h>
#include <core/protocol/midi.h>
#include <stdarg.h>

namespace lsp
{
    namespace osc
    {
        typedef struct packet_t
        {
            size_t          size;
            uint8_t        *data;
        } packet_t;

#ifdef __LSP_PROTOCOL_OSC_IMPL
        const uint64_t BUNDLE_SIG    = __IF_LEBE( 0x00656C646E756223ULL, 0x2362756E646C6500ULL ); // "#bundle\0"
        const uint32_t BUNDLE_SIZE   = 0;
        const uint32_t EMPTY_PARAMS  = __IF_LEBE( 0x0000002C, 0x2C000000 ); // ",\0\0\0"

        #pragma pack(push, 1)
        typedef struct sized_bundle_header_t
        {
            uint32_t    size;
            uint64_t    sig;
            uint64_t    tag;
        } sized_bundle_header_t;

        typedef struct bundle_header_t
        {
            uint64_t    sig;
            uint64_t    tag;
        } bundle_header_t;
        #pragma pack(pop)

        enum forge_ref_type_t
        {
            FRT_UNKNOWN,
            FRT_ROOT,
            FRT_BUNDLE,
            FRT_MESSAGE,
            FRT_ARRAY
        };

        enum forge_param_type_t
        {
            FPT_INT32           = 'i',
            FPT_FLOAT32         = 'f',
            FPT_OSC_STRING      = 's',
            FPT_OSC_BLOB        = 'b',
            FPT_INT64           = 'h',
            FPT_OSC_TIMETAG     = 't',
            FPT_DOUBLE64        = 'd',
            FPT_TYPE            = 'S',
            FPT_ASCII_CHAR      = 'c',
            FPT_RGBA_COLOR      = 'r',
            FPT_MIDI_MESSAGE    = 'm',
            FPT_TRUE            = 'T',
            FPT_FALSE           = 'F',
            FPT_NULL            = 'N',
            FPT_INF             = 'I',
            FPT_ARRAY_START     = '[',
            FPT_ARRAY_END       = ']'
        };
#endif /* __LSP_PROTOCOL_OSC_IMPL */
    }
}

#endif /* CORE_PROTOCOL_OSC_TYPES_H_ */
