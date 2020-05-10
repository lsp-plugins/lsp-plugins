/*
 * parse.h
 *
 *  Created on: 29 мая 2019 г.
 *      Author: sadko
 */

#ifndef CORE_PROTOCOL_OSC_PARSE_H_
#define CORE_PROTOCOL_OSC_PARSE_H_

#include <core/protocol/osc/types.h>

namespace lsp
{
    namespace osc
    {
        typedef struct parser_t parser_t;

        typedef struct parse_frame_t
        {
            parser_t       *parser;
            parse_frame_t  *parent;
            parse_frame_t  *child;
            size_t          type;

            size_t          limit;
        } parser_frame_t;

        struct parser_t
        {
            const uint8_t  *data;
            size_t          offset;
            size_t          size;

            size_t          refs;
            const char     *args;
        };

        enum parse_token_t
        {
            PT_BUNDLE,
            PT_MESSAGE,
            PT_EOR,

            PT_INT32,
            PT_FLOAT32,
            PT_OSC_STRING,
            PT_OSC_BLOB,
            PT_INT64,
            PT_OSC_TIMETAG,
            PT_DOUBLE64,
            PT_TYPE,
            PT_ASCII_CHAR,
            PT_RGBA_COLOR,
            PT_MIDI_MESSAGE,
            PT_TRUE,
            PT_FALSE,
            PT_NULL,
            PT_INF,
            PT_ARRAY,

            PT_SYMBOL          = PT_TYPE
        };

        status_t parse_begin(parse_frame_t *ref, parser_t *parser, const void *data, size_t size);

        status_t parse_token(parse_frame_t *ref, parse_token_t *token);

        status_t parse_begin_message(parse_frame_t *child, parse_frame_t *ref, const char **address);
        status_t parse_begin_bundle(parse_frame_t *child, parse_frame_t *ref, uint64_t *time_tag);
        status_t parse_begin_array(parse_frame_t *child, parse_frame_t *ref);

        status_t parse_raw_message(parse_frame_t *ref, const void **start, size_t *size, const char **address);
        status_t parse_skip(parse_frame_t *ref);
        status_t parse_int32(parse_frame_t *ref, int32_t *value);
        status_t parse_float32(parse_frame_t *ref, float *value);
        status_t parse_string(parse_frame_t *ref, const char **s);
        status_t parse_blob(parse_frame_t *ref, const void **data, size_t *len);
        status_t parse_int64(parse_frame_t *ref, int64_t *value);
        status_t parse_double64(parse_frame_t *ref, double *value);
        status_t parse_time_tag(parse_frame_t *ref, uint64_t *value);
        status_t parse_type(parse_frame_t *ref, const char **s);
        status_t parse_symbol(parse_frame_t *ref, const char **s);
        status_t parse_ascii(parse_frame_t *ref, char *c);
        status_t parse_rgba(parse_frame_t *ref, uint32_t *rgba);
        status_t parse_midi(parse_frame_t *ref, midi::event_t *event);
        status_t parse_midi_raw(parse_frame_t *ref, const uint8_t **event, size_t *len);
        status_t parse_bool(parse_frame_t *ref, bool *value);
        status_t parse_null(parse_frame_t *ref);
        status_t parse_inf(parse_frame_t *ref);

        status_t parse_message(parse_frame_t *ref, const char *params, const char **address...);
        status_t parse_messagev(parse_frame_t *ref, const char *params, const char **address, va_list args);

        status_t parse_end(parse_frame_t *ref);

        status_t parse_destroy(parser_t *parser);
    }
}



#endif /* CORE_PROTOCOL_OSC_PARSE_H_ */
