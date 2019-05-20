/*
 * osc.h
 *
 *  Created on: 20 мая 2019 г.
 *      Author: sadko
 */

#ifndef CORE_PROTOCOL_OSC_H_
#define CORE_PROTOCOL_OSC_H_

#include <core/types.h>
#include <core/status.h>
#include <core/midi.h>

namespace lsp
{
    namespace osc
    {
        typedef struct forge_t  forge_t;

        typedef struct packet_t
        {
            uint32_t        size;
            byte           *data;
        } packet_t;

        typedef struct forge_ref_t
        {
            forge_t        *forge;
            forge_ref_t    *parent;
        } forge_ref_t;

        struct forge_t
        {
            uint8_t    *data;
            size_t      size;
            ssize_t     limit;

            forge_ref_t root;

            size_t      addr_tail;
            size_t      types_head;
            size_t      types_tail;
            size_t      args_head;
            size_t      args_tail;
        };

        status_t forge_init(forge_t *forge, forge_ref_t *ref, void *data, size_t size);

        status_t forge_address(forge_ref_t *ref, const char *s);
        status_t forge_int32(forge_ref_t *ref, int32_t value);
        status_t forge_float32(forge_ref_t *ref, float value);
        status_t forge_string(forge_ref_t *ref, const char *s);
        status_t forge_string(forge_ref_t *ref, const void *data, size_t bytes);
        status_t forge_int64(forge_ref_t *ref, int64_t value);
        status_t forge_time_tag(forge_ref_t *ref, uint64_t value);
        status_t forge_type(forge_ref_t *ref, const char *s);
        status_t forge_ascii(forge_ref_t *ref, char c);
        status_t forge_rgba(forge_ref_t *ref, const uint32_t rgba);
        status_t forge_midi(forge_ref_t *ref, const midi_event_t *event);
        status_t forge_bool(forge_ref_t *ref, bool value);
        status_t forge_null(forge_ref_t *ref);
        status_t forge_inf(forge_ref_t *ref);
        status_t forge_array(forge_ref_t *ref, forge_ref_t *child);

        status_t forge_finish(forge_ref_t *ref);
    }
}

#endif /* CORE_PROTOCOL_OSC_H_ */
