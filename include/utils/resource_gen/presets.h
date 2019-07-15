/*
 * presets.h
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UTILS_RESOURCE_GEN_PRESETS_H_
#define UTILS_RESOURCE_GEN_PRESETS_H_

#include <core/stdlib/stdio.h>
#include <core/files/config.h>
#include <core/files/config/IConfigHandler.h>

#include <utils/resource_gen/wdict.h>
#include <utils/resource_gen/fdict.h>

namespace lsp
{
    namespace resgen
    {
        class PresetPreprocessor: public config::IConfigHandler
        {
            private:
                cvector<xml_word_t>        *wDict;
                cstorage<dict_float_t>     *fDict;

            public:
                explicit PresetPreprocessor(cvector<xml_word_t> *wdict, cstorage<dict_float_t> *fdict)
                {
                    wDict = wdict;
                    fDict = fdict;
                }

                virtual ~PresetPreprocessor()
                {
                    wDict = NULL;
                    fDict = NULL;
                }

            public:
                virtual status_t handle_parameter(const char *name, const char *value, size_t flags)
                {
                    if (!res_dict_add(wDict, name))
                        return STATUS_NO_MEM;
                    if (!res_dict_add(wDict, value))
                        return STATUS_NO_MEM;

                    return STATUS_OK;
                }

                virtual status_t handle_kvt_parameter(const char *name, const kvt_param_t *param, size_t flags)
                {
                    // Copy string, remove first '/' character
                    char *curr;
                    char *tmp = strdup(&name[1]);
                    if (tmp == NULL)
                        return STATUS_NO_MEM;

                    // Split KVT path into substrings
                    curr = tmp;
                    status_t res = STATUS_OK;

                    while (true)
                    {
                        char *next = ::strchr(curr, '/');
                        if (next == NULL)
                        {
                            if (!res_dict_add(wDict, curr))
                                res = STATUS_NO_MEM;
                            break;
                        }
                        else
                        {
                            *next   = '\0';
                            if (!res_dict_add(wDict, curr))
                            {
                                res = STATUS_NO_MEM;
                                break;
                            }
                            curr    = &next[1];
                        }
                    }

                    free(tmp);

                    // Analyze parameter type
                    switch (param->type)
                    {
                        case KVT_STRING:
                            if (!res_dict_add(wDict, (param->str != NULL) ? param->str : ""))
                                return STATUS_NO_MEM;
                            break;
                        case KVT_BLOB:
                            if (!res_dict_add(wDict, (param->blob.ctype != NULL) ? param->blob.ctype : ""))
                                return STATUS_NO_MEM;
                            break;
                        case KVT_FLOAT32:
                            if (!float_dict_add(fDict, param->f32))
                                return STATUS_NO_MEM;
                            break;
                        default:
                            break;
                    }

                    return res;
                }
        };

        class PresetSerializer: public config::IConfigHandler
        {
            private:
                cvector<xml_word_t>        *wDict;
                cstorage<dict_float_t>     *fDict;
                FILE                       *pOut;

                static size_t estimate(const char *name)
                {
                    size_t res = 0;
                    for (const char *p = name; *p != '\0'; ++p)
                        if (*p == '/')
                            ++res;
                    return res;
                }

            public:
                explicit PresetSerializer(cvector<xml_word_t> *wdict, cstorage<dict_float_t> *fdict, FILE *out)
                {
                    wDict   = wdict;
                    fDict   = fdict;
                    pOut    = out;
                }

                virtual ~PresetSerializer()
                {
                    wDict   = NULL;
                    fDict   = NULL;
                    pOut    = NULL;
                }

            public:
                virtual status_t handle_parameter(const char *name, const char *value, size_t flags)
                {
                    fputs("\t\t", pOut);
                    encode_value(pOut, 0);
                    fputc(' ', pOut);
                    encode_word(pOut, wDict, name);
                    fputc(' ', pOut);
                    encode_value(pOut, flags);
                    fputc(' ', pOut);
                    encode_word(pOut, wDict, value);
                    fputs("\n", pOut);
                    return STATUS_OK;
                }

                virtual status_t handle_kvt_parameter(const char *name, const kvt_param_t *param, size_t flags)
                {
                    // Copy string, remove first '/' character
                    char *curr;
                    char *tmp       = ::strdup(name);
                    if (tmp == NULL)
                        return STATUS_NO_MEM;

                    // Split KVT path into substrings and encode them
                    fputs("\t\t", pOut);
                    encode_value(pOut, estimate(name));
                    fputc(' ', pOut);

                    curr            = &tmp[1];
                    status_t res    = STATUS_OK;

                    while (true)
                    {
                        char *next = ::strchr(curr, '/');
                        if (next == NULL)
                        {
                            encode_word(pOut, wDict, curr);
                            fputc(' ', pOut);
                            break;
                        }
                        else
                        {
                            *next   = '\0';
                            encode_word(pOut, wDict, curr);
                            fputc(' ', pOut);
                            curr    = &next[1];
                        }
                    }

                    free(tmp);
                    if (res != STATUS_OK)
                        return res;

                    // Encode KVT value depending on the parameter
                    encode_value(pOut, flags);
                    fputc(' ', pOut);

                    switch (param->type)
                    {
                        case KVT_INT32:
                            encode_value(pOut, param->i32);
                            break;
                        case KVT_UINT32:
                            encode_value(pOut, param->u32);
                            break;
                        case KVT_INT64:
                            encode_value(pOut, param->i64);
                            break;
                        case KVT_UINT64:
                            encode_value(pOut, param->u64);
                            break;
                        case KVT_FLOAT32:
                        {
                            dict_float_t *f = float_dict_get(fDict, param->f32);
                            if (f == NULL)
                                return STATUS_CORRUPTED;
                            encode_value(pOut, f->index);
                            break;
                        }
                        case KVT_FLOAT64:
                            encode_bytes(pOut, &param->f64, sizeof(param->f64));
                            break;
                        case KVT_STRING:
                            encode_word(pOut, wDict, (param->str != NULL) ? param->str : "");
                            break;
                        case KVT_BLOB:
                            encode_value(pOut, param->blob.size);
                            fputc(' ', pOut);
                            encode_word(pOut, wDict, (param->blob.ctype != NULL) ? param->blob.ctype : "");
                            if (param->blob.data != NULL)
                            {
                                fputc(' ', pOut);
                                encode_bytes(pOut, param->blob.data, param->blob.size);
                            }
                            break;
                        default:
                            return STATUS_BAD_STATE;
                    }

                    fputs("\n", pOut);

                    return res;
                }
        };

        static int preprocess_preset(scan_resource_t *resource, cvector<xml_word_t> *wdict, cstorage<dict_float_t> *fdict)
        {
            if (!res_dict_add(wdict, "")) // Empty string is used to specify end of sequence
                return STATUS_NO_MEM;

            PresetPreprocessor pp(wdict, fdict);
            return config::load(resource->path, &pp);
        }

        static int serialize_preset(FILE *out, const scan_resource_t *resource,
                cvector<xml_word_t> *wdict, cstorage<dict_float_t> *fdict)
        {
            // Output resource descriptor
            fprintf(out,    "\t// Contents of file %s\n", resource->id);
            fprintf(out,    "\tstatic const char *builtin_resource%s =\n", resource->hex);

            PresetSerializer ps(wdict, fdict, out);
            status_t res = config::load(resource->path, &ps);

            fprintf(out,    "\t\t\"\\x00\"");
            encode_word(out, wdict, "");
            fprintf(out,    "\n\t\t;\n\n");

            return res;
        }

    }
}

#endif /* UTILS_RESOURCE_GEN_PRESETS_H_ */
