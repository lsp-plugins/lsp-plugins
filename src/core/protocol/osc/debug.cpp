/*
 * debug.cpp
 *
 *  Created on: 29 мая 2019 г.
 *      Author: sadko
 */

#define __LSP_PROTOCOL_OSC_IMPL

#include <core/protocol/osc.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <core/stdlib/string.h>
#include <dsp/endian.h>
#include <core/debug.h>

namespace lsp
{
    namespace osc
    {
#ifdef LSP_TRACE
        void dump_packet(const packet_t *packet)
        {
            const uint8_t *ddump        = packet->data;
            lsp_trace("OSC packet dump (%d bytes):", int(packet->size));

            for (size_t offset=0; offset < packet->size; offset += 16)
            {
                // Print HEX dump
                lsp_nprintf("%08x: ", int(offset));
                for (size_t i=0; i<0x10; ++i)
                {
                    if ((offset + i) < packet->size)
                        lsp_nprintf("%02x ", int(ddump[i]));
                    else
                        lsp_nprintf("   ");
                }
                lsp_nprintf("   ");

                // Print character dump
                for (size_t i=0; i<0x10; ++i)
                {
                    if ((offset + i) < packet->size)
                    {
                        uint8_t c   = ddump[i];
                        if ((c < 0x20) || (c >= 0x80))
                            c           = '.';
                        lsp_nprintf("%c", c);
                    }
                    else
                        lsp_nprintf(" ");
                }
                lsp_printf("");

                // Move pointer
                ddump       += 0x10;
            }
        }
#endif /* LSP_TRACE */
    }
}








