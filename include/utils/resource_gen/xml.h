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

#if defined(PLATFORM_WINDOWS)
    #include <msxml.h>
#else
    #include <expat.h>
#endif /* PLATFORM_WINDOWS */

#include <utils/resource_gen/resource.h>
#include <utils/resource_gen/wdict.h>
#include <utils/resource_gen/serialize.h>

namespace lsp
{
    namespace resgen
    {

        typedef struct xml_parser_t
        {
            FILE                       *out;
            size_t                      pad_left;
            bool                        new_line;
            cvector<xml_word_t>        *dict;
        } xml_parser_t;

        static void xml_pre_start_element_handler(void *userData, const XML_Char *name, const XML_Char **atts)
        {
            xml_parser_t *parser = reinterpret_cast<xml_parser_t *>(userData);

            // Preprocess name
            res_dict_add(parser->dict, name);

            // Preprocess parameters
            for (const XML_Char **p = atts; *p != NULL; p++)
                res_dict_add(parser->dict, *p);
        }

        static void xml_pre_end_element_handler(void *userData, const XML_Char *name)
        {
            xml_parser_t *parser = reinterpret_cast<xml_parser_t *>(userData);

            // Preprocess name
            res_dict_add(parser->dict, name);
        }

        static void xml_padding(const xml_parser_t *parser)
        {
            for (size_t i=0; i<parser->pad_left; ++i)
                fputc('\t', parser->out);
        }

        static void xml_start_element_handler(void *userData, const XML_Char *name, const XML_Char **atts)
        {
            xml_parser_t *parser = reinterpret_cast<xml_parser_t *>(userData);

            // Calculate number of parameters
            size_t count = 0;
            for (const XML_Char **p = atts; *p != NULL; p++)
            {
                if (*(++p) == NULL)
                    break;
                count++;
            }

            // Output tag header
            fputc('\n', parser->out);
            xml_padding(parser);
            fprintf(parser->out, "O(");
            encode_word(parser->out, parser->dict, name);
            fprintf(parser->out, ", \"\\x%02x\")", int(count));

            // Now output parameters
            while (*atts != NULL)
            {
                const XML_Char *k   = *atts++;
                const XML_Char *v   = *atts++;
                if ((k != NULL) && (v != NULL))
                {
                    fprintf(parser->out, " P(");
                    encode_word(parser->out, parser->dict, k);
                    fprintf(parser->out, ", ");
                    encode_word(parser->out, parser->dict, v);
                    fprintf(parser->out, ")");
                }
            }
            parser->pad_left    ++;
            parser->new_line    = false;
        }

        static void xml_end_element_handler(void *userData, const XML_Char *name)
        {
            xml_parser_t *parser = reinterpret_cast<xml_parser_t *>(userData);

            parser->pad_left    --;

            if (parser->new_line)
            {
                fputc('\n', parser->out);
                xml_padding(parser);
            }
            else
                fputc(' ', parser->out);
            fprintf(parser->out, "C(");
            encode_word(parser->out, parser->dict, name);
            fprintf(parser->out, ")");

            parser->new_line    = true;
        }

        static int preprocess_xml_resource(const scan_resource_t *resource, cvector<xml_word_t> *dict)
        {
            // Initialize context
            xml_parser_t context;
            context.out         = NULL;
            context.pad_left    = 2;
            context.new_line    = false;
            context.dict        = dict;

            // Create XML parser
            XML_Parser parser = XML_ParserCreate(NULL);
            if (parser == NULL)
                return -2;

            // Initialize parser
            XML_SetUserData(parser, &context);
            XML_SetElementHandler(parser, xml_pre_start_element_handler, xml_pre_end_element_handler);

            // Open input file
            FILE *in = fopen(resource->path, "r");
            if (in == NULL)
            {
                fprintf(stderr, "Could not open file %s\n", resource->path);
                XML_ParserFree(parser);
                return -3;
            }

            printf("Preprocessing file %s\n", resource->path);

            // Parse file
            char buf[4096];
            while (true)
            {
                size_t to_read  = sizeof(buf)/sizeof(char);
                size_t count    = fread(buf, sizeof(char), to_read, in);
                bool final      = count < to_read;

                if (!XML_Parse(parser, buf, count, final))
                {
                    XML_Error code = XML_GetErrorCode(parser);
                    fprintf(stderr, "XML parsing error: %s\n", XML_ErrorString(code));
                    XML_ParserFree(parser);
                    return -4;
                }

                if (final)
                    break;
            }

            // Free parser and close file
            XML_ParserFree(parser);
            fclose(in);

            return 0;
        }

        static int serialize_xml_resource(FILE *out, const scan_resource_t *resource, cvector<xml_word_t> *dict)
        {
            // Initialize context
            xml_parser_t context;
            context.out         = out;
            context.pad_left    = 2;
            context.new_line    = false;
            context.dict        = dict;

            // Output resource descriptor
            fprintf(out,    "\t// Contents of file %s\n", resource->id);
            fprintf(out,    "\tstatic const char *builtin_resource%s =", resource->hex);

            // Create XML parser
            XML_Parser parser = XML_ParserCreate(NULL);
            if (parser == NULL)
                return -2;

            // Initialize parser
            XML_SetUserData(parser, &context);
            XML_SetElementHandler(parser, xml_start_element_handler, xml_end_element_handler);

            // Open input file
            FILE *in = fopen(resource->path, "r");
            if (in == NULL)
            {
                fprintf(stderr, "Could not open file %s\n", resource->path);
                XML_ParserFree(parser);
                return -3;
            }

            // Parse file
            char buf[4096];
            while (true)
            {
                size_t to_read  = sizeof(buf)/sizeof(char);
                size_t count    = fread(buf, sizeof(char), to_read, in);
                bool final      = count < to_read;

                if (!XML_Parse(parser, buf, count, final))
                {
                    XML_Error code = XML_GetErrorCode(parser);
                    fprintf(stderr, "XML parsing error: %s\n", XML_ErrorString(code));
                    XML_ParserFree(parser);
                    return -4;
                }

                if (final)
                    break;
            }

            // Free parser and close file
            XML_ParserFree(parser);
            fclose(in);

            fprintf(out,    "\n\t\t;\n\n");

            return 0;
        }
    }
}


#endif /* UTILS_RESOURCE_GEN_XML_H_ */
