/*
 * serialize.h
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UTILS_RESOURCE_GEN_SERIALIZE_H_
#define UTILS_RESOURCE_GEN_SERIALIZE_H_

#include <core/stdlib/stdio.h>
#include <utils/resource_gen/wdict.h>

namespace lsp
{
    namespace resgen
    {
        static void encode_bytes(FILE *out, const void *data, size_t len)
        {
            bool quote = false;
            int tokens = 0;
            const uint8_t *p = reinterpret_cast<const uint8_t *>(data);

            // Output string
            for (size_t i=0; i<len; ++p, ++tokens, ++i)
            {
                if (((*p) >= 0x20) && ((*p) <= 0x7f))
                {
                    if (!quote)
                    {
                        if (tokens > 0)
                            fputs(" \"", out);
                        else
                            fputc('\"', out);
                        quote = true;
                    }

                    switch (*p)
                    {
                        case '\"': fputs("\\\"", out); break;
                        case '\\': fputs("\\\\", out); break;
                        default: fputc(*p, out); break;
                    }
                }
                else
                {
                    if (quote)
                    {
                        fputs("\" ", out);
                        quote = false;
                    }
                    if (tokens > 0)
                        fputc(' ', out);
                    fprintf(out, "\"\\x%02x\"", uint8_t(*p));
                }
            }

            // Finalize string state
            if (tokens == 0)
            {
                fputc('\"', out);
                quote = true;
            }
            if (quote)
                fputc('\"', out);
        }

        static inline void encode_string(FILE *out, const char *p)
        {
            encode_bytes(out, p, strlen(p));
        }

        static void encode_value(FILE *out, uint64_t value)
        {
            int tokens = 0;
            do
            {
                if ((tokens++) > 0)
                    fputc(' ', out);
                int flag = (value > 0x7f) ? 0x80 : 0x00;
                fprintf(out, "\"\\x%02x\"", (int(value) & 0x7f) | flag);
                value >>= 7;
            } while (value > 0);
        }

        static void encode_signed(FILE *out, int64_t value)
        {
            size_t v = value & (0x7fffffffffffffffULL);

            // First iteration with sign
            int x = v & 0x3f;
            v >>= 6;
            if (v != 0)
                x |= 0x80;
            if (value < 0)
                x |= 0x40;
            fprintf(out, "\"\\x%02x\" ", x);

            // Next iterations without sign
            while (v != 0)
            {
                x = v & 0x7f;
                v >>= 7;
                if (v != 0)
                    x |= 0x80;
                fprintf(out, "\"\\x%02x\" ", x);
            }

            fputc('\n', out);
        }

        static void encode_word(FILE *out, cvector<xml_word_t> *dict, const char *key)
        {
            xml_word_t *w   = res_dict_get(dict, key);
            encode_value(out, (w != NULL) ? w->offset : 0);
        }

        int emit_string_dictionary(FILE *out, cvector<xml_word_t> *dict)
        {
            size_t items    = dict->size();

            // Sort dictionary
            if (items > 1)
            {
                for (size_t i=0; i<(items-1); ++i)
                    for (size_t j=i+1; j<items; ++j)
                    {
                        xml_word_t *w1  = dict->at(i);
                        xml_word_t *w2  = dict->at(j);
                        if (w1->compare(w2) < 0)
                            dict->swap_unsafe(i, j);
                    }
            }

            // Output resource descriptor
            fprintf(out,    "\t// String Dictionary\n");
            fprintf(out,    "\textern const char *string_dictionary;\n\n");
            fprintf(out,    "\tconst char *string_dictionary =\n");

            // Emit data
            size_t offset = 0;
            for (size_t i=0; i<items; ++i)
            {
                xml_word_t *w   = dict->at(i);
                w->offset       = offset;
                fprintf(out, "\t\tK(");

                encode_string(out, w->text);

                fprintf(out, ") // offset: 0x%08x, refs=%d\n", int(w->offset), int(w->refs));
                offset         += w->length + 1; // 1 char separator
            }

            fprintf(out, "\t\t;\n\n");

            return STATUS_OK;
        }

        int emit_float_dictionary(FILE *out, cstorage<dict_float_t> *dict)
        {
            size_t items    = dict->size();

            // Sort dictionary
            if (items > 1)
            {
                for (size_t i=0; i<(items-1); ++i)
                    for (size_t j=i+1; j<items; ++j)
                    {
                        dict_float_t *w1  = dict->at(i);
                        dict_float_t *w2  = dict->at(j);
                        if (w1->compare(w2) < 0)
                            w1->swap(w2);
                    }
            }

            // Output resource descriptor
            fprintf(out,    "\t// Floating-Point Dictionary\n");
            fprintf(out,    "\textern const float float_dictionary[];\n\n");
            fprintf(out,    "\tconst float float_dictionary[] =\n");
            fprintf(out,    "\t{\n");

            // Emit data
            for (size_t i=0; i<items; ++i)
            {
                dict_float_t *w     = dict->at(i);
                w->index            = i;
                fprintf(out, "\t\t%f, // index: 0x%08x, refs=%d\n", w->value, int(i), int(w->refs));
            }

            fprintf(out, "\t};\n\n");

            return STATUS_OK;
        }
    }
}


#endif /* UTILS_RESOURCE_GEN_SERIALIZE_H_ */
