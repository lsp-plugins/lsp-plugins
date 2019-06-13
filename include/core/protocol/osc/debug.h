/*
 * debug.h
 *
 *  Created on: 29 мая 2019 г.
 *      Author: sadko
 */

#ifndef CORE_PROTOCOL_OSC_DEBUG_H_
#define CORE_PROTOCOL_OSC_DEBUG_H_

#include <core/protocol/osc/types.h>
#include <core/debug.h>

namespace lsp
{
    namespace osc
    {
    #ifdef LSP_TRACE
        void dump_packet(const packet_t *packet);

        void dump_packet(const void *data, size_t size);
    #else
        inline void dump_packet(const packet_t *packet) {}

        inline void dump_packet(const void *data, size_t size) {}
    #endif
    }
}


#endif /* CORE_PROTOCOL_OSC_DEBUG_H_ */
