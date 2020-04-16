/*
 * forge.cpp
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

            forge->data     = buf;
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
            return forge_begin_message(child, ref, NULL, address);
        }

        status_t forge_begin_message(forge_frame_t *child, forge_frame_t *ref, const char *prefix, const char *address)
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

            // Append message prefix
            if (prefix != NULL)
            {
                res             = forge_append_bytes(buf, prefix, ::strlen(prefix));
                if (res != STATUS_OK)
                    return res;
            }

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

        status_t forge_message(forge_frame_t *ref, const char *address, const char *params...)
        {
            va_list args;
            va_start(args, params);
            status_t res = forge_messagev(ref, address, params, args);
            va_end(args);
            return res;
        }

        status_t forge_messagev(forge_frame_t *ref, const char *address, const char *params, va_list args)
        {
            forge_frame_t message;
            status_t res = forge_begin_message(&message, ref, address);
            if (res != STATUS_OK)
                return res;

            ssize_t recursive = 0;

            if (params != NULL)
            {
                for (const char *fmt=params; *fmt != '\0'; ++fmt)
                {
                    switch (*fmt)
                    {
                        case FPT_INT32:
                            res = forge_int32(&message, va_arg(args, int32_t));
                            break;
                        case FPT_FLOAT32:
                        {
                            float v = float(va_arg(args, double));
                            res = (isinf(v)) ? forge_inf(&message) : forge_float32(&message, v);
                            break;
                        }
                        case FPT_OSC_STRING:
                        {
                            const char *str = va_arg(args, const char *);
                            res = (str != NULL) ? forge_string(&message, str) : forge_null(&message);
                            break;
                        }
                        case FPT_INT64:
                            res = forge_int64(&message, va_arg(args, int64_t));
                            break;
                        case FPT_OSC_TIMETAG:
                            res = forge_time_tag(&message, va_arg(args, uint64_t));
                            break;
                        case FPT_DOUBLE64:
                        {
                            double v = va_arg(args, double);
                            res = (isinf(v)) ? forge_inf(&message) : forge_double64(&message, v);
                            break;
                        }
                        case FPT_TYPE:
                        {
                            const char *str = va_arg(args, const char *);
                            res = (str != NULL) ? forge_type(&message, str) : forge_null(&message);
                            break;
                        }
                        case FPT_ASCII_CHAR:
                            res = forge_ascii(&message, char(va_arg(args, int)));
                            break;
                        case FPT_RGBA_COLOR:
                            res = forge_rgba(&message, va_arg(args, uint32_t));
                            break;
                        case FPT_TRUE:
                        case FPT_FALSE:
                            res = forge_bool(&message, bool(va_arg(args, int)));
                            break;
                        case FPT_NULL:
                            res = forge_null(&message);
                            break;
                        case FPT_INF:
                            res = forge_inf(&message);
                            break;

                        case FPT_ARRAY_START:
                            res = forge_parameter(&message, FPT_ARRAY_START, NULL, 0);
                            if (res == STATUS_OK)
                                ++recursive;
                            break;
                        case FPT_ARRAY_END:
                            res = forge_parameter(&message, FPT_ARRAY_END, NULL, 0);
                            if (res == STATUS_OK)
                            {
                                if (--recursive < 0)
                                    res = STATUS_BAD_FORMAT;
                            }
                            break;

                        case FPT_OSC_BLOB:
                        {
                            size_t size     = va_arg(args, size_t);
                            const void *ptr = va_arg(args, const void *);
                            res = forge_blob(&message, ptr, size);
                            break;
                        }

                        case FPT_MIDI_MESSAGE:
                            res = STATUS_NOT_SUPPORTED;
                            break;

                        default:
                            res = STATUS_BAD_FORMAT;
                            break;
                    }

                    if (res != STATUS_OK)
                        break;
                }
            }

            if ((res == STATUS_OK) && (recursive != 0))
                res = STATUS_BAD_FORMAT;

            if (res == STATUS_OK)
                res = forge_end(&message);
            else
                forge_end(&message);

            return res;
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

        status_t forge_symbol(forge_frame_t *ref, const char *s)
        {
            return forge_parameter(ref, FPT_TYPE, s, ::strlen(s) + 1);
        }

        status_t forge_ascii(forge_frame_t *ref, char c)
        {
            // Needs clarification: OSC spec says "an ascii character, sent as 32 bits"
            // Do we need to send a single byte and pad it to 4 bytes, or convert byte to 32-bit
            // value and send it? More probable variant is the second
            //uint8_t x       = uint8_t(c);
            uint32_t x      = CPU_TO_BE(uint32_t(c & 0xff));
            return forge_parameter(ref, FPT_ASCII_CHAR, &x, sizeof(x));
        }

        status_t forge_rgba(forge_frame_t *ref, const uint32_t rgba)
        {
            uint32_t x      = CPU_TO_BE(rgba);
            return forge_parameter(ref, FPT_RGBA_COLOR, &x, sizeof(x));
        }

        status_t forge_midi(forge_frame_t *ref, const midi::event_t *event)
        {
            uint8_t x[4];
            size_t n        = midi::encode(x, event);
            if (n < 0)
                return -n;
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






