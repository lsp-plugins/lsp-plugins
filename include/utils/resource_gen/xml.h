/*
 * xml.h
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UTILS_RESOURCE_GEN_XML_H_
#define UTILS_RESOURCE_GEN_XML_H_

#include <core/types.h>
#include <data/cvector.h>
#include <core/stdlib/stdio.h>

#include <core/files/xml/PushParser.h>

#include <utils/resource_gen/resource.h>
#include <utils/resource_gen/wdict.h>
#include <utils/resource_gen/serialize.h>

namespace lsp
{
    namespace resgen
    {

        class XMLPreparser: public xml::IXMLHandler
        {
            private:
                XMLPreparser & operator = (const XMLPreparser &);

            private:
                cvector<xml_word_t> *pDict;

            public:
                explicit XMLPreparser(cvector<xml_word_t> *dict): pDict(dict) {}
                virtual ~XMLPreparser() { pDict = NULL; }

            public:
                virtual status_t start_element(const LSPString *name, const LSPString * const *atts)
                {
                    // Preprocess element name
                    const char *s = name->get_utf8();
                    if ((s == NULL) || (!res_dict_add(pDict, s)))
                        return STATUS_NO_MEM;

                    // Preprocess parameters
                    for ( ; *atts != NULL; ++atts)
                    {
                        s = (*atts)->get_utf8();
                        if ((s == NULL) || (!res_dict_add(pDict, s)))
                            return STATUS_NO_MEM;
                    }
                    return STATUS_OK;
                }
        };

        class XMLTranslator: public xml::IXMLHandler
        {
            private:
                XMLTranslator & operator = (const XMLTranslator &);

            private:
                FILE                *pOut;
                cvector<xml_word_t> *pDict;
                size_t               nPadLeft;
                bool                 bNewLine;

            private:
                void xml_padding()
                {
                    for (size_t i=0; i<nPadLeft; ++i)
                        fputc('\t', pOut);
                }

            public:
                explicit XMLTranslator(FILE *out, cvector<xml_word_t> *dict)
                {
                    pOut        = out;
                    pDict       = dict;
                    nPadLeft    = 2;
                    bNewLine    = false;
                }
                virtual ~XMLTranslator() { pDict = NULL; }

            public:
                virtual status_t start_element(const LSPString *name, const LSPString * const *atts)
                {
                    // Calculate number of parameters
                    size_t count = 0;
                    for (const LSPString * const *p = atts; *p != NULL; p += 2, ++count) {}

                    // Output tag header
                    fputc('\n', pOut);
                    xml_padding();
                    fputs("O(", pOut);
                    const char *s = name->get_utf8();
                    if (s == NULL)
                        return STATUS_NO_MEM;
                    encode_word(pOut, pDict, s);
                    fprintf(pOut, ", \"\\x%02x\")", int(count));

                    // Now output parameters
                    for ( ; *atts != NULL; atts += 2)
                    {
                        const char *k = atts[0]->get_utf8();
                        const char *v = atts[1]->get_utf8();

                        if ((k != NULL) && (v != NULL))
                        {
                            fputs(" P(", pOut);
                            encode_word(pOut, pDict, k);
                            fputs(", ", pOut);
                            encode_word(pOut, pDict, v);
                            fputc(')', pOut);
                        }
                    }

                    nPadLeft    ++;
                    bNewLine    = false;

                    return STATUS_OK;
                }

                virtual status_t end_element(const LSPString *name)
                {
                    nPadLeft    --;

                    if (bNewLine)
                    {
                        fputc('\n', pOut);
                        xml_padding();
                    }
                    else
                        fputc(' ', pOut);

                    const char *s = name->get_utf8();
                    if (s == NULL)
                        return STATUS_NO_MEM;

                    fputs("C(", pOut);
                    encode_word(pOut, pDict, s);
                    fputc(')', pOut);
                    bNewLine = true;
                    return STATUS_OK;
                }
        };

        static int preprocess_xml_resource(const scan_resource_t *resource, cvector<xml_word_t> *dict)
        {
            // Create XML parser
            xml::PushParser parser;
            XMLPreparser h(dict);
            return parser.parse_file(&h, resource->path);
        }

        static int serialize_xml_resource(FILE *out, const scan_resource_t *resource, cvector<xml_word_t> *dict)
        {
            // Output resource descriptor
            fprintf(out,    "\t// Contents of file %s\n", resource->id);
            fprintf(out,    "\tstatic const char *builtin_resource%s =", resource->hex);

            // Create XML parser
            xml::PushParser parser;
            XMLTranslator h(out, dict);
            status_t res = parser.parse_file(&h, resource->path);

            // Trailing characters
            fputs("\n\t\t;\n\n", out);
            return res;
        }
    }
}


#endif /* UTILS_RESOURCE_GEN_XML_H_ */
