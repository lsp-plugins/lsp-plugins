/*
 * osc.cpp
 *
 *  Created on: 21 мая 2019 г.
 *      Author: sadko
 */

#include <core/protocol/osc.h>
#include <stdlib.h>
#include <core/stdlib/string.h>
#include <dsp/endian.h>

namespace lsp
{
    namespace osc
    {
        static const uint64_t BUNDLE_SIG    = __IF_LEBE( 0x00656C646E756223ULL, 0x2362756E646C6500ULL ); // "#bundle\0"
        static const uint32_t BUNDLE_SIZE   = 0;
        static const uint32_t EMPTY_PARAMS  = __IF_LEBE( 0x0000002C, 0x2C000000 ); // ",\0\0\0"

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


        status_t forge_begin_fixed(forge_frame_t *ref, forge_t *forge, void *data, size_t size)
        {
            if ((data == NULL) || (ref == NULL) || (forge == NULL))
                return STATUS_BAD_ARGUMENTS;

            forge->data     = reinterpret_cast<uint8_t *>(data);
            forge->offset   = 0;
            forge->capacity = size;
            forge->dynamic  = false;
            forge->refs     = 1;
            forge->toff     = 0;
            forge->tsize    = 0;

            ref->forge      = forge;
            ref->parent     = NULL;
            ref->child      = NULL;
            ref->type       = FRT_ROOT;
            ref->offset     = 0;

            return STATUS_OK;
        }

        status_t forge_begin_dynamic(forge_frame_t *ref, forge_t *forge, size_t reserve)
        {
            if ((ref == NULL) || (forge == NULL))
                return STATUS_BAD_ARGUMENTS;

            uint8_t *buf    = NULL;
            if (reserve > 0)
            {
                if ((buf = reinterpret_cast<uint8_t *>(::malloc(reserve))) == NULL)
                    return STATUS_NO_MEM;
            }

            forge->data     = NULL;
            forge->offset   = 0;
            forge->capacity = reserve;
            forge->dynamic  = true;
            forge->refs     = 1;
            forge->toff     = 0;
            forge->tsize    = 0;

            ref->forge      = forge;
            ref->parent     = NULL;
            ref->child      = NULL;
            ref->type       = FRT_ROOT;
            ref->offset     = 0;

            return STATUS_OK;
        }

        bool forge_check_child(forge_frame_t *child, forge_frame_t *ref)
        {
            if (child == NULL)
                return false;

            for ( ; ref != NULL; ref = ref->parent)
                if (ref == child)
                    return false;

            return true;
        }

        status_t forge_check_capacity(forge_t *buf, size_t capacity)
        {
            if (capacity <= buf->capacity)
                return STATUS_OK;

            if (!buf->dynamic)
                return STATUS_OVERFLOW;

            // Reallocate memory
            size_t to_alloc = ((capacity << 1) + capacity) >> 1; // capacity * 1.5
            uint8_t *ptr    = reinterpret_cast<uint8_t *>(::realloc(buf->data, to_alloc));
            if (ptr == NULL)
                return STATUS_NO_MEM;

            buf->data       = ptr;
            buf->capacity   = to_alloc;

            return STATUS_OK;
        }

        status_t forge_append_bytes(forge_t *buf, const void *data, size_t count)
        {
            status_t res    = forge_check_capacity(buf, buf->offset + count);
            if (res != STATUS_OK)
                return res;

            // Append data to buffer
            ::memcpy(&buf->data[buf->offset], data, count);
            buf->offset    += count;

            return STATUS_OK;
        }

        status_t forge_append_padded(forge_t *buf, const void *data, size_t count)
        {
            size_t capacity = ((count + sizeof(uint32_t) - 1) >> 2) << 2;
            status_t res    = forge_check_capacity(buf, buf->offset + capacity);
            if (res != STATUS_OK)
                return res;

            // Append data to buffer and pad tail with zeros
            ::memcpy(&buf->data[buf->offset], data, count);
            buf->offset    += count;
            while (count++ < capacity)
                buf->data[buf->offset++]    = 0;

            return STATUS_OK;
        }

        status_t forge_parameter(forge_frame_t *ref, forge_param_type_t type, const void *data, size_t count)
        {
            if (ref == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (ref->child != NULL)
                return STATUS_BAD_STATE;

            // Should be FRT_MESSAGE to emit parameters
            if ((ref->type != FRT_MESSAGE) && (ref->type != FRT_ARRAY))
                return STATUS_BAD_STATE;

            forge_t *buf    = ref->forge;
            size_t capacity = buf->offset;

            // Estimate additional space for tag string
            size_t old_pad  = ((buf->tsize + sizeof(uint32_t) - 1) >> 2);
            size_t new_pad  = ((buf->tsize + sizeof(uint32_t)) >> 2);
            if (old_pad != new_pad)
                capacity        += sizeof(uint32_t);

            size_t datacap  = ((count + sizeof(uint32_t) - 1) >> 2) << 2;
            capacity       += datacap;

            // Ensure that there is enough capacity
            status_t res    = forge_check_capacity(buf, capacity);
            if (res != STATUS_OK)
                return res;

            // Need to insert some space for parameter tags?
            if (old_pad != new_pad)
            {
                size_t aoff     = buf->toff + (old_pad << 2);
                uint8_t *ptr    = &buf->data[aoff];

                ::memmove(&ptr[sizeof(uint32_t)], ptr, buf->offset - aoff);
                ptr[0]          = 0;
                ptr[1]          = 0;
                ptr[2]          = 0;
                ptr[3]          = 0;

                buf->offset    += sizeof(uint32_t);
            }

            // Append type tag
            buf->data[buf->toff + buf->tsize - 1] = uint8_t(type);
            ++ buf->tsize;

            // Append data to buffer and pad tail with zeros
            if (count > 0)
            {
                ::memcpy(&buf->data[buf->offset], data, count);
                buf->offset    += count;
                while (count++ < datacap)
                    buf->data[buf->offset++]    = 0;
            }

            return STATUS_OK;
        }

        status_t forge_begin_bundle(forge_frame_t *child, forge_frame_t *ref, uint64_t tag)
        {
            if ((ref == NULL) || (!forge_check_child(child, ref)))
                return STATUS_BAD_ARGUMENTS;
            if ((ref->child != NULL) || (ref->forge == NULL))
                return STATUS_BAD_STATE;

            status_t res;
            forge_t *buf        = ref->forge;
            ssize_t offset      = buf->offset;

            if (ref->type == FRT_BUNDLE)
            {
                // Append bundle header
                sized_bundle_header_t hdr;
                hdr.size        = 0;
                hdr.sig         = BUNDLE_SIG;
                hdr.tag         = CPU_TO_BE(tag);

                res             = forge_append_bytes(buf, &hdr, sizeof(hdr));
                if (res != STATUS_OK)
                    return res;
            }
            else if (ref->type == FRT_ROOT)
            {
                // Disallow multiple bundles
                if (buf->offset > 0)
                    return STATUS_BAD_STATE;

                // Append bundle header
                bundle_header_t hdr;
                hdr.sig         = BUNDLE_SIG;
                hdr.tag         = CPU_TO_BE(tag);

                res             = forge_append_bytes(buf, &hdr, sizeof(hdr));
                if (res != STATUS_OK)
                    return res;
            }
            else
                return STATUS_BAD_STATE;

            // Link and initialize child frame
            ref->child      = child;

            child->forge    = buf;
            child->parent   = ref;
            child->child    = NULL;
            child->type     = FRT_BUNDLE;
            child->offset   = offset;
            ++ buf->refs;

            return STATUS_OK;
        }

        status_t forge_begin_message(forge_frame_t *child, forge_frame_t *ref, const char *address)
        {
            if ((ref == NULL) || (address == NULL) || (!forge_check_child(child, ref)))
                return STATUS_BAD_ARGUMENTS;
            if ((ref->child != NULL) || (ref->forge == NULL))
                return STATUS_BAD_STATE;

            // Need to reserve space for size?
            status_t res;
            forge_t *buf    = ref->forge;
            ssize_t offset  = buf->offset;

            if (ref->type == FRT_BUNDLE)
            {
                res             = forge_append_bytes(buf, &BUNDLE_SIZE, sizeof(BUNDLE_SIZE));
                if (res != STATUS_OK)
                    return res;
            }
            else if (ref->type == FRT_ROOT)
            {
                // Disallow multiple messages
                if (buf->offset > 0)
                    return STATUS_BAD_STATE;
            }
            else
                return STATUS_BAD_STATE;

            // Append message string
            res             = forge_append_padded(buf, address, ::strlen(address) + 1);
            if (res != STATUS_OK)
                return res;

            // Append type string (should be always present)
            buf->toff       = buf->offset;
            buf->tsize      = 2; // ',' + '\0'

            res                 = forge_append_bytes(buf, &EMPTY_PARAMS, sizeof(EMPTY_PARAMS));
            if (res != STATUS_OK)
                return res;

            // Link and initialize child frame
            ref->child      = child;

            child->forge    = buf;
            child->parent   = ref;
            child->child    = NULL;
            child->type     = FRT_MESSAGE;
            child->offset   = offset;
            ++ buf->refs;

            return STATUS_OK;
        }

        status_t forge_begin_array(forge_frame_t *child, forge_frame_t *ref)
        {
            if ((ref == NULL) || (!forge_check_child(child, ref)))
                return STATUS_BAD_ARGUMENTS;
            if ((ref->child != NULL) || (ref->forge == NULL))
                return STATUS_BAD_STATE;
            if ((ref->type != FRT_MESSAGE) && (ref->type != FRT_ARRAY))
                return STATUS_BAD_STATE;

            // Forge array start
            status_t res    = forge_parameter(ref, FPT_ARRAY_START, NULL, 0);
            if (res != STATUS_OK)
                return res;

            // Initialize child
            ref->child      = child;

            child->forge    = ref->forge;
            child->parent   = ref;
            child->child    = NULL;
            child->type     = FRT_ARRAY;
            child->offset   = 0;
            ++ ref->forge->refs;

            return STATUS_OK;
        }

        status_t forge_int32(forge_frame_t *ref, int32_t value)
        {
            int32_t x       = CPU_TO_BE(value);
            return forge_parameter(ref, FPT_INT32, &x, sizeof(x));
        }

        status_t forge_float32(forge_frame_t *ref, float value)
        {
            float x         = CPU_TO_BE(value);
            return forge_parameter(ref, FPT_FLOAT32, &x, sizeof(x));
        }

        status_t forge_string(forge_frame_t *ref, const char *s)
        {
            return forge_parameter(ref, FPT_OSC_STRING, s, ::strlen(s) + 1);
        }

        status_t forge_blob(forge_frame_t *ref, const void *data, size_t bytes)
        {
            if (ref == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (ref->child != NULL)
                return STATUS_BAD_STATE;

            // Should be FRT_MESSAGE to emit parameters
            if ((ref->type != FRT_MESSAGE) && (ref->type != FRT_ARRAY))
                return STATUS_BAD_STATE;

            uint32_t len    = CPU_TO_BE(uint32_t(bytes));
            status_t res    = forge_append_bytes(ref->forge, &len, sizeof(len));
            if (res == STATUS_OK)
                res = forge_parameter(ref, FPT_OSC_BLOB, data, bytes);

            return res;
        }

        status_t forge_int64(forge_frame_t *ref, int64_t value)
        {
            int64_t x       = CPU_TO_BE(value);
            return forge_parameter(ref, FPT_INT64, &x, sizeof(x));
        }

        status_t forge_double64(forge_frame_t *ref, double value)
        {
            double x        = CPU_TO_BE(value);
            return forge_parameter(ref, FPT_DOUBLE64, &x, sizeof(x));
        }

        status_t forge_time_tag(forge_frame_t *ref, uint64_t value)
        {
            uint64_t x      = CPU_TO_BE(value);
            return forge_parameter(ref, FPT_OSC_TIMETAG, &x, sizeof(x));
        }

        status_t forge_type(forge_frame_t *ref, const char *s)
        {
            return forge_parameter(ref, FPT_TYPE, s, ::strlen(s) + 1);
        }

        status_t forge_ascii(forge_frame_t *ref, char c)
        {
            // Needs clarification: OSC spec says "an ascii character, sent as 32 bits"
            // Do we need to send a single byte and pad it to 4 bytes, or convert byte to 32-bit
            // value and send it? More probable variant is the second
            //uint8_t x       = uint8_t(c);
            uint32_t x      = uint8_t(c);
            return forge_parameter(ref, FPT_ASCII_CHAR, &x, sizeof(x));
        }

        status_t forge_rgba(forge_frame_t *ref, const uint32_t rgba)
        {
            uint32_t x      = CPU_TO_BE(rgba);
            return forge_parameter(ref, FPT_RGBA_COLOR, &x, sizeof(x));
        }

        status_t forge_midi(forge_frame_t *ref, const midi_event_t *event)
        {
            uint8_t x[4];
            size_t n        = encode_midi_message(event, x);
            if (n <= 0)
                return STATUS_BAD_ARGUMENTS;
            return forge_parameter(ref, FPT_MIDI_MESSAGE, &x, n);
        }

        status_t forge_midi_raw(forge_frame_t *ref, const void *event, size_t bytes)
        {
            if ((bytes <= 0) || (bytes > 3))
                return STATUS_BAD_ARGUMENTS;
            return forge_parameter(ref, FPT_MIDI_MESSAGE, event, bytes);
        }

        status_t forge_bool(forge_frame_t *ref, bool value)
        {
            return forge_parameter(ref, (value) ? FPT_TRUE : FPT_FALSE, NULL, 0);
        }

        status_t forge_null(forge_frame_t *ref)
        {
            return forge_parameter(ref, FPT_NULL, NULL, 0);
        }

        status_t forge_inf(forge_frame_t *ref)
        {
            return forge_parameter(ref, FPT_INF, NULL, 0);
        }

        status_t forge_end(forge_frame_t *ref)
        {
            if ((ref == NULL) || (ref->child != NULL))
                return STATUS_BAD_STATE;

            forge_t *buf = ref->forge;
            if (buf == NULL)
                return STATUS_BAD_STATE;

            status_t res = STATUS_OK;

            switch (ref->type)
            {
                case FRT_ROOT:
                    if (buf->refs <= 0)
                        return STATUS_BAD_STATE;
                    --buf->refs;
                    return STATUS_OK;

                case FRT_ARRAY:
                    if (ref->parent == NULL)
                        return STATUS_BAD_STATE;
                    res    = forge_parameter(ref, FPT_ARRAY_END, NULL, 0);
                    --buf->refs;
                    break;

                case FRT_BUNDLE:
                case FRT_MESSAGE:
                    if (ref->parent == NULL)
                        return STATUS_BAD_STATE;

                    // Need to commit size to header?
                    if (ref->parent->type == FRT_BUNDLE)
                    {
                        union {
                            uint8_t    *u8;
                            uint32_t   *u32;
                        } xptr;

                        uint32_t size       = buf->offset - ref->offset - sizeof(uint32_t);
                        xptr.u8             = &buf->data[ref->offset];
                        *(xptr.u32)         = CPU_TO_BE(size);
                    }
                    --buf->refs;
                    break;

                default:
                    return STATUS_CORRUPTED;
            }

            // Unlink frame
            ref->parent->child  = NULL;
            ref->forge          = NULL;
            ref->parent         = NULL;
            ref->type           = FRT_UNKNOWN;
            ref->offset         = -1;

            return res;
        }

        status_t forge_close(packet_t *packet, forge_t *forge)
        {
            if ((forge == NULL) || (packet == NULL))
                return STATUS_BAD_ARGUMENTS;
            if ((forge->refs > 0) || (forge->data == NULL))
                return STATUS_BAD_STATE;

            packet->size    = forge->offset;
            packet->data    = forge->data;

            forge->data     = NULL;
            forge->dynamic  = false;
            forge->capacity = 0;
            forge->offset   = 0;

            return STATUS_OK;
        }

        status_t forge_destroy(forge_t *forge)
        {
            if (forge == NULL)
                return STATUS_BAD_ARGUMENTS;
            if ((forge->dynamic) && (forge->data != NULL))
                ::free(forge->data);

            forge->data     = NULL;
            forge->dynamic  = false;
            forge->capacity = 0;
            forge->offset   = 0;
            forge->refs     = 0;
            forge->toff     = 0;
            forge->tsize    = 0;

            return STATUS_OK;
        }

        void forge_free(void *ptr)
        {
            if (ptr != NULL)
                ::free(ptr);
        }
    }
}


