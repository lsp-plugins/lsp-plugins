/*
 * i18n.h
 *
 *  Created on: 28 февр. 2020 г.
 *      Author: sadko
 */

#ifndef UTILS_RESOURCE_GEN_I18N_H_
#define UTILS_RESOURCE_GEN_I18N_H_

#include <core/files/json/Parser.h>

#include <utils/resource_gen/wdict.h>
#include <utils/resource_gen/fdict.h>
#include <utils/resource_gen/serialize.h>

namespace lsp
{
    namespace resgen
    {
        static status_t preprocess_json_resource(json::Parser &p, cvector<xml_word_t> *dict, cstorage<dict_float_t> *fdict)
        {
            using namespace json;
            json::event_t ev;
            status_t res;

            while ((res = p.read_next(&ev)) == STATUS_OK)
            {
                switch (ev.type)
                {
                    case JE_OBJECT_START:
                    case JE_OBJECT_END:
                    case JE_ARRAY_START:
                    case JE_ARRAY_END:
                    case JE_BOOL:
                    case JE_NULL:
                    case JE_INTEGER:
                        continue;
                    case JE_DOUBLE:
                        if (!float_dict_add(fdict, ev.fValue))
                            return STATUS_NO_MEM;
                        break;

                    case JE_PROPERTY:
                    case JE_STRING:
                        if (!res_dict_add(dict, ev.sValue.get_utf8()))
                            return STATUS_NO_MEM;
                        break;

                    default:
                        return STATUS_BAD_FORMAT;
                }
            }

            return (res == STATUS_EOF) ? STATUS_OK : res;
        }

        static void json_pad(bool &bpad, size_t pad, FILE *out)
        {
            if (!bpad)
            {
                bpad = true;
                return;
            }

            while (pad--)
                fputc('\t', out);
        }

        static status_t serialize_json_resource(FILE *out, json::Parser &p, cvector<xml_word_t> *dict, cstorage<dict_float_t> *fdict)
        {
            using namespace json;
            json::event_t ev;
            status_t res;

            size_t pad = 2;
            bool bpad = true;

            while ((res = p.read_next(&ev)) == STATUS_OK)
            {
                switch (ev.type)
                {
                    case JE_OBJECT_START:
                        json_pad(bpad, pad++, out);
                        fputs("\"{\"\n", out);
                        break;
                    case JE_OBJECT_END:
                        json_pad(bpad, --pad, out);
                        fputs("\"}\"\n", out);
                        break;
                    case JE_ARRAY_START:
                        json_pad(bpad, pad++, out);
                        fputs("\"[\"\n", out);
                        break;
                    case JE_ARRAY_END:
                        json_pad(bpad, pad--, out);
                        fputs("\"]\"\n", out);
                        break;
                    case JE_BOOL:
                        json_pad(bpad, pad, out);
                        fprintf(out, "\"%c\"\n", (ev.bValue) ? 'T' : 'F');
                        break;
                    case JE_NULL:
                        json_pad(bpad, pad, out);
                        fputs("\"N\"\n", out);
                        break;
                    case JE_INTEGER:
                        json_pad(bpad, pad, out);
                        fputs("\"I\" ", out);
                        encode_signed(out, ev.iValue);
                        fputs("\n", out);
                        continue;

                    case JE_DOUBLE:
                    {
                        json_pad(bpad, pad, out);
                        fputs("\"F\" ", out);
                        dict_float_t *f = float_dict_get(fdict, ev.fValue);
                        encode_value(out, f->index);
                        fputs("\n", out);
                        break;
                    }

                    case JE_PROPERTY:
                        json_pad(bpad, pad, out);
                        fputs("\":\" ", out);
                        encode_word(out, dict, ev.sValue.get_utf8());
                        bpad = false;
                        fputs(" ", out);
                        break;

                    case JE_STRING:
                        json_pad(bpad, pad, out);
                        fputs("\"\\\"\" ", out);
                        encode_word(out, dict, ev.sValue.get_utf8());
                        fputs("\n", out);
                        break;

                    default:
                        return STATUS_BAD_FORMAT;
                }
            }

            return (res == STATUS_EOF) ? STATUS_OK : res;
        }

        static status_t preprocess_json(const scan_resource_t *resource, cvector<xml_word_t> *dict, cstorage<dict_float_t> *fdict)
        {
            json::Parser p;
            status_t res = p.open(resource->path, json::JSON_VERSION5);
            if (res != STATUS_OK)
                return res;

            res = preprocess_json_resource(p, dict, fdict);
            if (res == STATUS_OK)
                res = p.close();
            else
                p.close();
            return res;
        }

        static status_t serialize_json(FILE *out, const scan_resource_t *resource, cvector<xml_word_t> *dict, cstorage<dict_float_t> *fdict)
        {
            // Output resource descriptor
            fprintf(out,    "\t// Contents of file %s\n", resource->id);
            fprintf(out,    "\tstatic const char *builtin_resource%s =\n", resource->hex);

            json::Parser p;
            status_t res = p.open(resource->path, json::JSON_VERSION5);
            if (res != STATUS_OK)
                return res;

            res = serialize_json_resource(out, p, dict, fdict);
            if (res == STATUS_OK)
                res = p.close();
            else
                p.close();

            // Trailing characters
            fputs("\t\t;\n\n", out);
            return res;
        }
    }
}



#endif /* UTILS_RESOURCE_GEN_I18N_H_ */
