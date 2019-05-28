/*
 * osc.cpp
 *
 *  Created on: 21 мая 2019 г.
 *      Author: sadko
 */

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


        bool parse_check_child(parse_frame_t *child, parse_frame_t *ref)
        {
            if ((ref == NULL) || (child == NULL))
                return false;

            for ( ; ref != NULL; ref = ref->parent)
                if (ref == child)
                    return false;

            return true;
        }

        status_t parse_begin(parse_frame_t *ref, parser_t *parser, const void *data, size_t size)
        {
            if ((ref == NULL) || (parser == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;
            if ((size < sizeof(uint32_t)) || ((size % sizeof(uint32_t)) != 0))
                return STATUS_BAD_ARGUMENTS;

            ref->parser     = parser;
            ref->parent     = NULL;
            ref->child      = NULL;
            ref->type       = FRT_ROOT;
            ref->limit      = size;

            parser->data    = reinterpret_cast<const uint8_t *>(data);
            parser->offset  = 0;
            parser->size    = size;
            parser->refs    = 1;
            parser->args    = NULL;

            return STATUS_OK;
        }

        status_t parse_token(parse_frame_t *ref, parse_token_t *token)
        {
            if (ref == NULL)
                return STATUS_BAD_ARGUMENTS;
            if ((ref->child != NULL) || (ref->parser == NULL))
                return STATUS_BAD_STATE;

            parser_t *buf           = ref->parser;
            int tok                 = -1;
            ssize_t left            = ref->limit - buf->offset;

            switch (ref->type)
            {
                case FRT_ROOT:
                case FRT_BUNDLE:
                {
                    // End of record?
                    if (left == 0)
                    {
                        tok     = PT_EOR;
                        break;
                    }

                    union
                    {
                        const bundle_header_t  *hdr;
                        const uint32_t         *u32;
                        const uint8_t          *u8;
                    } xptr;

                    xptr.u8     = &buf->data[buf->offset];
                    size_t size = buf->size;

                    // End of record?
                    if (ref->type != FRT_ROOT)
                    {
                        size        = BE_TO_CPU(xptr.u32[0]);
                        ++xptr.u32;
                        left       -= 4;
                    }
                    else if (buf->offset > 0)
                        return STATUS_CORRUPTED;

                    // Check that size field is valid
                    if (size_t(left) < size)
                        return STATUS_CORRUPTED;

                    if ((size >= sizeof(uint32_t)) && (xptr.u8[0] == '/'))
                        tok     = PT_MESSAGE;
                    else if ((size >= sizeof(bundle_header_t)) && (xptr.hdr->sig == BUNDLE_SIG))
                        tok     = PT_BUNDLE;
                    break;
                }

                case FRT_MESSAGE:
                case FRT_ARRAY:
                    if (buf->args == NULL)
                        return STATUS_BAD_STATE;

                    switch (*(buf->args))
                    {
                        case FPT_INT32:         tok = PT_INT32; break;
                        case FPT_FLOAT32:       tok = PT_FLOAT32; break;
                        case FPT_OSC_STRING:    tok = PT_OSC_STRING; break;
                        case FPT_OSC_BLOB:      tok = PT_OSC_BLOB; break;
                        case FPT_INT64:         tok = PT_INT64; break;
                        case FPT_OSC_TIMETAG:   tok = PT_OSC_TIMETAG; break;
                        case FPT_DOUBLE64:      tok = PT_DOUBLE64; break;
                        case FPT_TYPE:          tok = PT_TYPE; break;
                        case FPT_ASCII_CHAR:    tok = PT_ASCII_CHAR; break;
                        case FPT_RGBA_COLOR:    tok = PT_RGBA_COLOR; break;
                        case FPT_MIDI_MESSAGE:  tok = PT_MIDI_MESSAGE; break;
                        case FPT_TRUE:          tok = PT_TRUE; break;
                        case FPT_FALSE:         tok = PT_FALSE; break;
                        case FPT_NULL:          tok = PT_NULL; break;
                        case FPT_INF:           tok = PT_INF; break;
                        case FPT_ARRAY_START:   tok = PT_ARRAY; break;

                        case FPT_ARRAY_END:
                            if (ref->type == FRT_ARRAY)
                                tok = PT_EOR;
                            break;

                        case 0:
                            if ((ref->type == FRT_MESSAGE) && (left <= 0))
                                tok = PT_EOR;
                            break;

                        default:
                            break;
                    }
                    break;

                default:
                    return STATUS_BAD_STATE;
            }

            if (tok == -1)
                return STATUS_CORRUPTED;

            if (token != NULL)
                *token      = parse_token_t(tok);
            return STATUS_OK;
        }

        status_t parse_begin_message(parse_frame_t *child, parse_frame_t *ref, const char **address)
        {
            // Check state and arguments
            if (!parse_check_child(child, ref))
                return STATUS_BAD_ARGUMENTS;
            if ((ref->child != NULL) || (ref->parser == NULL))
                return STATUS_BAD_STATE;
            if ((ref->type != FRT_ROOT) && (ref->type != FRT_BUNDLE))
                return STATUS_BAD_STATE;

            // Initialize lookup
            union
            {
                const uint8_t  *u8;
                const uint32_t *u32;
                const char     *ch;
            } xptr;

            parser_t *buf   = ref->parser;
            ssize_t left    = ref->limit - buf->offset;
            size_t size     = buf->size;
            xptr.u8         = &buf->data[buf->offset];

            // Need to read size?
            if (ref->type == FRT_BUNDLE)
            {
                if (size_t(left) <= sizeof(uint32_t))
                    return STATUS_CORRUPTED;
                size        = BE_TO_CPU(*(xptr.u32)) + sizeof(uint32_t);
                xptr.u8    += sizeof(uint32_t);

                // Analyze size
                if (size_t(left) < size)
                    return STATUS_CORRUPTED;
                left       -= sizeof(uint32_t);
            }

            // Is there enough size to read address?
            if (left <= ssize_t(sizeof(uint32_t)))
                return STATUS_CORRUPTED;
            else if ((xptr.ch[0] != '/'))
                return STATUS_BAD_TYPE;

            // Check address length
            const char *addr    = xptr.ch;
            size_t addr_len     = ::strnlen(addr, left);
            if (ssize_t(addr_len) >= left)
                return STATUS_CORRUPTED;

            // Estimate the padded data size and move pointer
            size_t padded   = ((addr_len + sizeof(uint32_t)) >> 2) << 2;
            left           -= padded;
            xptr.u8        += padded;

            // From OSC spec:
            // Note: some older implementations of OSC may omit the OSC Type Tag string.
            // Until all such implementations are updated, OSC implementations should be
            // robust in the case of a missing OSC Type Tag String.
            const char *args    = "";
            if (left > 0)
            {
                // Check that second argument is a type tag string
                if (xptr.ch[0] != ',')
                    return STATUS_CORRUPTED;

                // Ensure that type tag string is valid
                size_t arg_len  = ::strnlen(xptr.ch, left);
                if (ssize_t(arg_len) >= left)
                    return STATUS_CORRUPTED;

                args            = &xptr.ch[1];
                padded          = ((arg_len + sizeof(uint32_t)) >> 2) << 2;
                left           -= padded;
                xptr.u8        += padded;
            }

            // Store new frame data
            child->parser   = buf;
            child->parent   = ref;
            child->child    = NULL;
            child->type     = FRT_MESSAGE;
            child->limit    = buf->offset + size;
            ref->child      = child;

            buf->offset     = ref->limit - left;
            buf->args       = args;
            ++ buf->refs;

            if (address != NULL)
                *address    = addr;

            return STATUS_OK;
        }

        status_t parse_begin_bundle(parse_frame_t *child, parse_frame_t *ref, uint64_t *time_tag)
        {
            // Check state and arguments
            if (!parse_check_child(child, ref))
                return STATUS_BAD_ARGUMENTS;
            if ((ref->child != NULL) || (ref->parser == NULL))
                return STATUS_BAD_STATE;
            if ((ref->type != FRT_ROOT) && (ref->type != FRT_BUNDLE))
                return STATUS_BAD_STATE;

            // Initialize lookup
            union
            {
                const uint8_t  *u8;
                const uint32_t *u32;
                const bundle_header_t  *hdr;
            } xptr;

            parser_t *buf   = ref->parser;
            ssize_t left    = ref->limit - buf->offset;
            size_t size     = buf->size;
            xptr.u8         = &buf->data[buf->offset];

            // Need to read size?
            if (ref->type == FRT_BUNDLE)
            {
                if (left <= ssize_t(sizeof(uint32_t)))
                    return STATUS_CORRUPTED;
                size        = BE_TO_CPU(*(xptr.u32)) + sizeof(uint32_t);
                xptr.u8    += sizeof(uint32_t);

                // Analyze size
                if (size_t(left) < size)
                    return STATUS_CORRUPTED;
                left       -= sizeof(uint32_t);
            }

            // Is there enough size to read bundle header? Bundle header is valid
            if (left <= ssize_t(sizeof(bundle_header_t)))
                return STATUS_CORRUPTED;
            else if (xptr.hdr->sig != BUNDLE_SIG)
                return STATUS_BAD_TYPE;
            left           -= sizeof(bundle_header_t);

            // Store new frame data
            child->parser   = buf;
            child->parent   = ref;
            child->child    = NULL;
            child->type     = FRT_BUNDLE;
            child->limit    = buf->offset + size;
            ref->child      = child;

            buf->offset     = ref->limit - left;
            buf->args       = NULL;
            ++ buf->refs;

            if (time_tag != NULL)
                *time_tag       = BE_TO_CPU(xptr.hdr->tag);

            return STATUS_OK;
        }

        inline bool parse_check_msg(parse_frame_t *ref)
        {
            if ((ref->child != NULL) || (ref->parser == NULL))
                return false;
            if ((ref->type != FRT_ARRAY) && (ref->type != FRT_MESSAGE))
                return false;
            return (ref->parser->args != NULL);
        }

        status_t parse_begin_array(parse_frame_t *child, parse_frame_t *ref)
        {
            // Check state and arguments
            if (!parse_check_child(child, ref))
                return STATUS_BAD_ARGUMENTS;
            if (!parse_check_msg(ref))
                return STATUS_BAD_STATE;

            parser_t *buf = ref->parser;
            if (*(buf->args) != FPT_ARRAY_START)
                return STATUS_BAD_STATE;

            // Store new frame data
            child->parser   = buf;
            child->parent   = ref;
            child->child    = NULL;
            child->type     = FRT_ARRAY;
            child->limit    = ref->limit;
            ref->child      = child;

            ++ buf->args;
            ++ buf->refs;

            return STATUS_OK;
        }

        status_t parse_skip(parse_frame_t *ref)
        {
            if ((ref->child != NULL) || (ref->parser == NULL))
                return STATUS_BAD_STATE;

            if ((ref->type == FRT_ROOT) || (ref->type == FRT_BUNDLE))
            {
                if (ref->parser->offset == ref->limit)
                    return STATUS_EOF;
                else if ((ref->type == FRT_ROOT) && (ref->parser->offset > 0))
                    return STATUS_CORRUPTED;

                parse_frame_t frm;

                // Try to parse as message
                status_t res    = parse_begin_message(&frm, ref, NULL);
                if (res == STATUS_OK)
                    res     = parse_end(&frm);
                else
                {
                    // Try to parse as bundle
                    res     = parse_begin_bundle(&frm, ref, NULL);
                    if (res == STATUS_OK)
                        res     = parse_end(&frm);
                }
                return res;
            }
            else if ((ref->type == FRT_ARRAY) || (ref->type == FRT_MESSAGE))
            {
                parser_t *buf   = ref->parser;
                if (buf->args == NULL)
                    return STATUS_BAD_STATE;

                size_t skip     = 0;

                switch (*(buf->args))
                {
                    case FPT_INT32:         skip = sizeof(int32_t); break;
                    case FPT_FLOAT32:       skip = sizeof(float); break;
                    case FPT_INT64:         skip = sizeof(int64_t); break;
                    case FPT_OSC_TIMETAG:   skip = sizeof(uint64_t); break;
                    case FPT_DOUBLE64:      skip = sizeof(double); break;
                    case FPT_ASCII_CHAR:    skip = sizeof(uint32_t); break;
                    case FPT_RGBA_COLOR:    skip = sizeof(uint32_t); break;
                    case FPT_MIDI_MESSAGE:  skip = sizeof(uint32_t); break;

                    case FPT_TRUE:
                    case FPT_FALSE:
                    case FPT_NULL:
                    case FPT_INF:
                        break; // skip = 0

                    case FPT_OSC_STRING:
                    case FPT_TYPE:
                    {
                        ssize_t left    = ref->limit - buf->offset;
                        if (left <= 0)
                            return STATUS_CORRUPTED;
                        const char *ch  = reinterpret_cast<const char *>(&buf->data[buf->offset]);
                        size_t len      = ::strnlen(ch, left);
                        skip            = ((len + sizeof(uint32_t)) >> 2) << 2;
                        if (skip > size_t(left))
                            return STATUS_CORRUPTED;
                        break;
                    }

                    case FPT_OSC_BLOB:
                    {
                        ssize_t left    = ref->limit - buf->offset;
                        if (left < ssize_t(sizeof(uint32_t)))
                            return STATUS_CORRUPTED;
                        const uint32_t *u32 = reinterpret_cast<const uint32_t *>(&buf->data[buf->offset]);
                        size_t size     = BE_TO_CPU(*u32);
                        skip            = ((size + 2*sizeof(uint32_t) - 1) >> 2) << 2;
                        if (skip > size_t(left))
                            return STATUS_CORRUPTED;
                        break;
                    }

                    case FPT_ARRAY_START: // Skip the entire array
                    {
                        parse_frame_t frm;
                        status_t res = parse_begin_array(&frm, ref);
                        if (res == STATUS_OK)
                            res     = parse_end(&frm);
                        return res;
                    }

                    case FPT_ARRAY_END: // Do not allow to skip array end
                        return (ref->type == FRT_ARRAY) ? STATUS_EOF : STATUS_CORRUPTED;

                    case 0:
                        if ((ref->type == FRT_ARRAY) || (buf->offset != ref->limit))
                            return STATUS_CORRUPTED;
                        return STATUS_EOF;

                    default:
                        return STATUS_CORRUPTED;
                }

                // Update position
                buf->offset    += skip;
                ++buf->args;
            }
            else
                return STATUS_BAD_STATE;

            return STATUS_OK;
        }

        template <class T>
            inline status_t parse_int(parse_frame_t *ref, T *value, forge_param_type_t type)
            {
                // Check state and arguments
                if (!parse_check_msg(ref))
                    return STATUS_BAD_STATE;

                parser_t *buf   = ref->parser;
                char ch         = *(buf->args);
                if (ch == type)
                {
                    // Parse integer value
                    if ((ref->limit - buf->offset) < sizeof(T))
                        return STATUS_CORRUPTED;
                    if (value != NULL)
                        *value  = BE_TO_CPU(*(reinterpret_cast<const T *>(&buf->data[buf->offset])));
                    buf->offset    += sizeof(T);
                    ++buf->args;
                    return STATUS_OK;
                }

                if (!ch)
                    return (buf->offset == ref->limit) ? STATUS_EOF : STATUS_CORRUPTED;

                if (ch == FPT_NULL)
                {
                    ++buf->args;
                    return STATUS_NULL;
                }

                return STATUS_BAD_TYPE;
            }

        status_t parse_int32(parse_frame_t *ref, int32_t *value)
        {
            return parse_int(ref, value, FPT_INT32);
        }

        status_t parse_rgba(parse_frame_t *ref, uint32_t *rgba)
        {
            return parse_int(ref, rgba, FPT_RGBA_COLOR);
        }

        status_t parse_int64(parse_frame_t *ref, int64_t *value)
        {
            return parse_int(ref, value, FPT_INT64);
        }

        status_t parse_time_tag(parse_frame_t *ref, uint64_t *value)
        {
            return parse_int(ref, value, FPT_OSC_TIMETAG);
        }

        status_t parse_ascii(parse_frame_t *ref, char *c)
        {
            // Check state and arguments
            if (!parse_check_msg(ref))
                return STATUS_BAD_STATE;

            parser_t *buf   = ref->parser;
            char ch         = *(buf->args);

            if (ch == FPT_ASCII_CHAR)
            {
                // Parse as 32-bit value
                if ((ref->limit - buf->offset) < sizeof(uint32_t))
                    return STATUS_CORRUPTED;
                if (c != NULL)
                    *c  = buf->data[buf->offset + 3];

                buf->offset    += sizeof(uint32_t);
                ++buf->args;
                return STATUS_OK;
            }

            if (!ch)
                return (buf->offset == ref->limit) ? STATUS_EOF : STATUS_CORRUPTED;

            if (ch == FPT_NULL)
            {
                ++buf->args;
                return STATUS_NULL;
            }

            return STATUS_BAD_TYPE;
        }

        template <class T>
            inline status_t parse_float(parse_frame_t *ref, T *value, forge_param_type_t type)
            {
                // Check state and arguments
                if (!parse_check_msg(ref))
                    return STATUS_BAD_STATE;

                parser_t *buf   = ref->parser;
                char ch         = *(buf->args);
                if (ch == type)
                {
                    // Parse float value
                    if ((ref->limit - buf->offset) < sizeof(T))
                        return STATUS_CORRUPTED;
                    if (value != NULL)
                        *value  = BE_TO_CPU(*(reinterpret_cast<const T *>(&buf->data[buf->offset])));
                    buf->offset    += sizeof(T);
                    ++buf->args;
                    return STATUS_OK;
                }

                if (!ch)
                    return (buf->offset == ref->limit) ? STATUS_EOF : STATUS_CORRUPTED;

                if (ch == FPT_NULL)
                {
                    ++buf->args;
                    return STATUS_NULL;
                }

                if (ch == FPT_INF)
                {
                    if (value != NULL)
                        *value  = INFINITY;
                    ++buf->args;
                    return STATUS_OK;
                }

                return STATUS_BAD_TYPE;
            }

        status_t parse_float32(parse_frame_t *ref, float *value)
        {
            return parse_float(ref, value, FPT_FLOAT32);
        }

        status_t parse_double64(parse_frame_t *ref, double *value)
        {
            return parse_float(ref, value, FPT_DOUBLE64);
        }

        status_t parse_string_value(parse_frame_t *ref, const char **s, forge_param_type_t type)
        {
            // Check state and arguments
            if (!parse_check_msg(ref))
                return STATUS_BAD_STATE;

            parser_t *buf   = ref->parser;
            char ch         = *(buf->args);
            if (ch == type)
            {
                // Perform string read
                const char *str = reinterpret_cast<const char *>(&buf->data[buf->offset]);
                size_t left     = ref->limit - buf->offset;
                size_t len      = ::strnlen(str, left);

                if (s != NULL)
                    *s  = str;

                buf->offset    += ((len + sizeof(uint32_t)) >> 2) << 2;
                ++buf->args;
                return STATUS_OK;
            }

            if (!ch)
                return (buf->offset == ref->limit) ? STATUS_EOF : STATUS_CORRUPTED;

            if (ch == FPT_NULL)
            {
                ++buf->args;
                if (s == NULL)
                    return STATUS_NULL;
                *s      = NULL;
                return STATUS_OK;
            }

            return STATUS_BAD_TYPE;
        }

        status_t parse_string(parse_frame_t *ref, const char **s)
        {
            return parse_string_value(ref, s, FPT_OSC_STRING);
        }

        status_t parse_type(parse_frame_t *ref, const char **s)
        {
            return parse_string_value(ref, s, FPT_TYPE);
        }

        status_t parse_symbol(parse_frame_t *ref, const char **s)
        {
            return parse_string_value(ref, s, FPT_TYPE);
        }

        status_t parse_blob(parse_frame_t *ref, const void **data, size_t *len)
        {
            // Check state and arguments
            if (!parse_check_msg(ref))
                return STATUS_BAD_STATE;

            parser_t *buf = ref->parser;
            switch (*(buf->args))
            {
                case FPT_OSC_BLOB:
                {
                    size_t left     = ref->limit - buf->offset;
                    if (left < sizeof(uint32_t))
                        return STATUS_CORRUPTED;

                    size_t size     = BE_TO_CPU(*(reinterpret_cast<const uint32_t *>(&buf->data[buf->offset])));
                    size_t skip     = ((size + 2*sizeof(uint32_t) - 1) >> 2) << 2;
                    if (skip > left)
                        return STATUS_CORRUPTED;

                    if (data != NULL)
                        *data   = &buf->data[buf->offset + sizeof(uint32_t)];
                    if (len != NULL)
                        *len    = size;

                    buf->offset    += skip;
                    ++buf->args;
                    return STATUS_OK;
                }
                case FPT_NULL:
                {
                    ++buf->args;
                    if (data == NULL)
                        return STATUS_NULL;
                    *data   = NULL;
                    if (len != NULL)
                        *len    = 0;
                    return STATUS_OK;
                }
                case 0:
                    return (buf->offset == ref->limit) ? STATUS_EOF : STATUS_CORRUPTED;
                default:
                    break;
            }

            return STATUS_BAD_TYPE;
        }

        inline status_t parse_flag(parse_frame_t *ref, forge_param_type_t type)
        {
            // Check state and arguments
            if (!parse_check_msg(ref))
                return STATUS_BAD_STATE;

            parser_t *buf   = ref->parser;
            char ch         = *(buf->args);
            if (!ch)
                return (buf->offset == ref->limit) ? STATUS_EOF : STATUS_CORRUPTED;
            else if (ch != type)
                return STATUS_BAD_TYPE;

            ++buf->args;
            return STATUS_OK;
        }

        status_t parse_null(parse_frame_t *ref)
        {
            return parse_flag(ref, FPT_NULL);
        }

        status_t parse_inf(parse_frame_t *ref)
        {
            return parse_flag(ref, FPT_INF);
        }

        status_t parse_bool(parse_frame_t *ref, bool *value)
        {
            // Check state and arguments
            if (!parse_check_msg(ref))
                return STATUS_BAD_STATE;

            parser_t *buf   = ref->parser;
            switch (*(buf->args))
            {
                case FPT_TRUE:
                    ++buf->args;
                    if (value != NULL)
                        *value      = true;
                    return STATUS_OK;
                case FPT_FALSE:
                    ++buf->args;
                    if (value != NULL)
                        *value      = false;
                    return STATUS_OK;
                case FPT_NULL:
                    ++buf->args;
                    return STATUS_NULL;
                case 0:
                    return (buf->offset == ref->limit) ? STATUS_EOF : STATUS_CORRUPTED;
                default:
                    break;
            }

            return STATUS_BAD_TYPE;
        }

        status_t parse_midi(parse_frame_t *ref, midi_event_t *event)
        {
            // Check state and arguments
            if (!parse_check_msg(ref))
                return STATUS_BAD_STATE;

            parser_t *buf   = ref->parser;
            switch (*(buf->args))
            {
                case FPT_MIDI_MESSAGE:
                {
                    size_t left     = ref->limit - buf->offset;
                    if (left < sizeof(uint32_t))
                        return STATUS_CORRUPTED;

                    midi_event_t ev;
                    if (!decode_midi_message(&ev, &buf->data[buf->offset]))
                        return STATUS_CORRUPTED;
                    if (event != NULL)
                        *event  = ev;

                    buf->offset    += sizeof(uint32_t);
                    ++buf->args;
                    return STATUS_OK;
                }
                case FPT_NULL:
                    ++buf->args;
                    return STATUS_NULL;
                case 0:
                    return (buf->offset == ref->limit) ? STATUS_EOF : STATUS_CORRUPTED;
                default:
                    break;
            }

            return STATUS_BAD_TYPE;
        }

        status_t parse_midi_raw(parse_frame_t *ref, const uint8_t **event, size_t *len)
        {
            // Check state and arguments
            if (!parse_check_msg(ref))
                return STATUS_BAD_STATE;

            parser_t *buf   = ref->parser;
            switch (*(buf->args))
            {
                case FPT_MIDI_MESSAGE:
                {
                    size_t left     = ref->limit - buf->offset;
                    if (left < sizeof(uint32_t))
                        return STATUS_CORRUPTED;

                    midi_event_t ev;
                    if (!decode_midi_message(&ev, &buf->data[buf->offset]))
                        return STATUS_CORRUPTED;

                    if (event != NULL)
                        *event  = &buf->data[buf->offset];
                    if (len != NULL)
                        *len    = encoded_midi_message_size(&ev);

                    buf->offset    += sizeof(uint32_t);
                    ++buf->args;
                    return STATUS_OK;
                }
                case FPT_NULL:
                    ++buf->args;
                    return STATUS_NULL;
                case 0:
                    return (buf->offset == ref->limit) ? STATUS_EOF : STATUS_CORRUPTED;
                default:
                    break;
            }

            return STATUS_BAD_TYPE;
        }

        status_t parse_end(parse_frame_t *ref)
        {
            if (ref == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (ref->child != NULL)
                return STATUS_BAD_STATE;

            parser_t *buf = ref->parser;
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
                    if ((ref->parent == NULL) || (buf->args == NULL))
                        return STATUS_BAD_STATE;
                    while (*(buf->args) != FPT_ARRAY_END)
                    {
                        res = parse_skip(ref);
                        if (res != STATUS_OK)
                            return (res == STATUS_EOF) ? STATUS_CORRUPTED : res;
                    }
                    --buf->refs;
                    ++buf->args;
                    break;

                case FRT_BUNDLE:
                case FRT_MESSAGE:
                    if (ref->parent == NULL)
                        return STATUS_BAD_STATE;
                    else if (buf->offset > ref->limit)
                        return STATUS_CORRUPTED;

                    buf->offset     = ref->limit;
                    --buf->refs;
                    break;

                default:
                    return STATUS_CORRUPTED;
            }

            // Unlink frame
            ref->parent->child  = NULL;
            ref->parser         = NULL;
            ref->parent         = NULL;
            ref->type           = FRT_UNKNOWN;
            ref->limit          = buf->size;

            return res;
        }

        status_t parse_destroy(parser_t *parser)
        {
            if (parser == NULL)
                return STATUS_BAD_ARGUMENTS;

            parser->data    = NULL;
            parser->offset  = 0;
            parser->size    = 0;
            parser->refs    = 0;
            parser->args    = NULL;

            return STATUS_OK;
        }

        status_t parse_message(parse_frame_t *ref, const char *params, const char **address...)
        {
            va_list args;
            va_start(args, address);
            status_t res = parse_messagev(ref, params, address, args);
            va_end(args);
            return res;
        }

        status_t parse_messagev(parse_frame_t *ref, const char *params, const char **address, va_list args)
        {
            parse_frame_t message;
            status_t res = parse_begin_message(&message, ref, address);
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
                            res     = parse_int32(&message, va_arg(args, int32_t *));
                            break;
                        case FPT_FLOAT32:
                            res     = parse_float32(&message, va_arg(args, float *));
                            break;
                        case FPT_OSC_STRING:
                            res     = parse_string(&message, va_arg(args, const char **));
                            break;
                        case FPT_INT64:
                            res     = parse_int64(&message, va_arg(args, int64_t *));
                            break;
                        case FPT_OSC_TIMETAG:
                            res     = parse_time_tag(&message, va_arg(args, uint64_t *));
                            break;
                        case FPT_DOUBLE64:
                            res     = parse_double64(&message, va_arg(args, double *));
                            break;
                        case FPT_TYPE:
                            res     = parse_type(&message, va_arg(args, const char **));
                            break;
                        case FPT_ASCII_CHAR:
                            res     = parse_ascii(&message, va_arg(args, char *));
                            break;
                        case FPT_RGBA_COLOR:
                            res     = parse_rgba(&message, va_arg(args, uint32_t *));
                            break;
                        case FPT_TRUE:
                        case FPT_FALSE:
                            res     = parse_bool(&message, va_arg(args, bool *));
                            break;

                        case FPT_ARRAY_START:
                            if (message.parser->args[0] == FPT_ARRAY_START)
                            {
                                ++recursive;
                                ++message.parser->args;
                            }
                            else
                                res = STATUS_BAD_TYPE;

                            break;
                        case FPT_ARRAY_END:
                            if (message.parser->args[0] == FPT_ARRAY_START)
                            {
                                if (--recursive < 0)
                                    res = STATUS_BAD_FORMAT;
                                else
                                    ++ message.parser->args;
                            }
                            else
                                res = STATUS_BAD_TYPE;
                            break;

                        case FPT_OSC_BLOB:
                        {
                            size_t *size        = va_arg(args, size_t *);
                            const void **ptr    = va_arg(args, const void **);
                            res     = parse_blob(&message, ptr, size);
                            break;
                        }

                        case FPT_MIDI_MESSAGE:
                            res = STATUS_NOT_SUPPORTED;
                            break;

                        default:
                            res = STATUS_BAD_FORMAT;
                            break;
                    }

                    if (res == STATUS_NULL)
                        res     = STATUS_OK;
                    else if (res != STATUS_OK)
                        break;
                }
            }

            if ((res == STATUS_OK) && (recursive != 0))
                res = STATUS_BAD_FORMAT;

            if (res == STATUS_OK)
                res = parse_end(&message);
            else
                parse_end(&message);

            return res;
        }

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


