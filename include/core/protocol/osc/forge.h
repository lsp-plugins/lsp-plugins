/*
 * forge.h
 *
 *  Created on: 29 мая 2019 г.
 *      Author: sadko
 */

#ifndef CORE_PROTOCOL_OSC_FORGE_H_
#define CORE_PROTOCOL_OSC_FORGE_H_

#include <core/protocol/osc/types.h>

namespace lsp
{
    namespace osc
    {
        typedef struct forge_t  forge_t;

        typedef struct forge_frame_t
        {
            forge_t        *forge;
            forge_frame_t  *parent;     // Parent forge
            forge_frame_t  *child;      // Child forge
            size_t          type;

            size_t          offset;     // Offset of the frame in the buffer
        } forge_frame_t;

        struct forge_t
        {
            uint8_t        *data;
            size_t          offset;
            size_t          capacity;
            bool            dynamic;

            size_t          refs;
            size_t          toff;       // Head of types field
            size_t          tsize;      // Size of types field
        };

        /**
         * Initialize OSC packet serializer in fixed buffer size (RT-compatible) mode
         *
         * @param ref pointer to output root frame descriptor
         * @param forge serializer structure to initialize
         * @param data pointer to the buffer
         * @param size size of the buffer
         * @return status of operation
         */
        status_t forge_begin_fixed(forge_frame_t *ref, forge_t *forge, void *data, size_t size);

        /**
         * Initialize OSC packet serializer in dynamic buffer size (non-RT-compatible) mode
         * @param ref pointer to output root frame descriptor
         * @param forge serializer structure to initialize
         * @return status of operation
         */
        status_t forge_begin_dynamic(forge_frame_t *ref, forge_t *forge, size_t reserve = 0);

        /**
         * Begin serialization of OSC bundle
         * @param child pointer to output bundle frame descriptor
         * @param ref current frame descriptor
         * @param tag OSC time tag
         * @return status of operation
         */
        status_t forge_begin_bundle(forge_frame_t *child, forge_frame_t *ref, uint64_t tag);

        /**
         * Begin serialization of OSC message
         * @param child pointer to output message frame descriptor
         * @param ref current frame descriptor
         * @param address the OSC address string
         * @return status of operation
         */
        status_t forge_begin_message(forge_frame_t *child, forge_frame_t *ref, const char *address);

        /**
         * Begin serialization of OSC message
         * @param child pointer to output message frame descriptor
         * @param ref current frame descriptor
         * @param prefix prefix to add to address string
         * @param address the OSC address string
         * @return status of operation
         */
        status_t forge_begin_message(forge_frame_t *child, forge_frame_t *ref, const char *prefix, const char *address);

        /**
         * Forge message
         * @param ref forge reference
         * @param address message address
         * @param params message parameters
         * @param args list of arguments
         * @return status of operation
         */
        status_t forge_message(forge_frame_t *ref, const char *address, const char *params...);

        /**
         * Forge message
         * @param ref forge reference
         * @param address message address
         * @param params message parameters
         * @param args list of arguments
         * @return status of operation
         */
        status_t forge_messagev(forge_frame_t *ref, const char *address, const char *params, va_list args);

        /**
         * Begin serialization of array within OSC message
         * @param child pointer to output array frame descriptor
         * @param ref current frame descriptor
         * @return status of operation
         */
        status_t forge_begin_array(forge_frame_t *child, forge_frame_t *ref);

        status_t forge_int32(forge_frame_t *ref, int32_t value);
        status_t forge_float32(forge_frame_t *ref, float value);
        status_t forge_string(forge_frame_t *ref, const char *s);
        status_t forge_blob(forge_frame_t *ref, const void *data, size_t bytes);
        status_t forge_int64(forge_frame_t *ref, int64_t value);
        status_t forge_double64(forge_frame_t *ref, double value);
        status_t forge_time_tag(forge_frame_t *ref, uint64_t value);
        status_t forge_type(forge_frame_t *ref, const char *s);
        status_t forge_symbol(forge_frame_t *ref, const char *s);
        status_t forge_ascii(forge_frame_t *ref, char c);
        status_t forge_rgba(forge_frame_t *ref, const uint32_t rgba);
        status_t forge_midi(forge_frame_t *ref, const midi::event_t *event);
        status_t forge_midi_raw(forge_frame_t *ref, const void *event, size_t bytes);
        status_t forge_bool(forge_frame_t *ref, bool value);
        status_t forge_null(forge_frame_t *ref);
        status_t forge_inf(forge_frame_t *ref);

        /**
         * Complete serialization of current frame
         * @param ref current frame descriptor
         * @return status of operation
         */
        status_t forge_end(forge_frame_t *ref);

        /**
         * Complete serialization of OSC packet
         * @param packet packet to store size and pointer to the data, for dynamic mode data should be freed
         *   by forge_free();
         * @param ref current frame descriptor
         * @return status of operation
         */
        status_t forge_close(packet_t *packet, forge_t *forge);

        /**
         * Destroy OSC serializer
         * @param forge serializer descriptor
         * @return status of operation
         */
        status_t forge_destroy(forge_t *forge);

        /**
         * Free mamory allocated by the OSC serializator
         * @param ptr ponter to memory
         * @return status of operation
         */
        void forge_free(void *ptr);
    }
}



#endif /* CORE_PROTOCOL_OSC_FORGE_H_ */
