#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#include <expat.h>

#include <core/types.h>

#include <data/cvector.h>

#include <metadata/metadata.h>
#include <plugins/plugins.h>


namespace lsp
{
    typedef struct xml_word_t
    {
        size_t      length;
        size_t      offset;
        size_t      refs;
        char        text[];

        inline int compare(const xml_word_t *w) const
        {
            int h = length * refs - w->length * w->refs;
            if (h != 0)
                return h;

            return strcmp(text, w->text);
        }

    } xml_word_t;

    typedef struct xml_parser_t
    {
        FILE                       *out;
        size_t                      pad_left;
        bool                        new_line;
        cvector<xml_word_t>        *dict;
    } xml_parser_t;

    static xml_word_t *res_dict_get(cvector<xml_word_t> *dict, const char *key)
    {
        size_t items    = dict->size();
        size_t len      = strlen(key);
        for (size_t i=0; i<items; ++i)
        {
            xml_word_t *w   = dict->at(i);
            if (w->length != len)
                continue;
            if (!strcmp(w->text, key))
                return w;
        }

        return NULL;
    }

    static bool res_dict_add(cvector<xml_word_t> *dict, const char *item)
    {
        xml_word_t *w = res_dict_get(dict, item);
        if (w != NULL)
        {
            w->refs        ++;
            return true;
        }

        // Create new word for the dictionary and initialize it
        size_t length       = strlen(item);
        w                   = reinterpret_cast<xml_word_t *>(malloc(sizeof(xml_word_t) + length + 1));
        if (item == 0)
            return false;
        w->length           = length;
        w->offset           = 0;
        w->refs             = 1;
        memcpy(w->text, item, length + 1);

        // Add new word
        if (!dict->add(w))
        {
            free(w);
            return false;
        }

        return true;
    }

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

    static void encode_word(FILE *out, cvector<xml_word_t> *dict, const char *key)
    {
        xml_word_t *w   = res_dict_get(dict, key);
        fprintf(out, "\"");

        if (w != NULL)
        {
            size_t off = w->offset;
            do
            {
                size_t flag = (off > 0x7f) ? 0x80 : 0x00;
                fprintf(out, "\\x%02x", int((off & 0x7f) | flag));
                off >>= 7;
            } while (off > 0);
        }
        else
            fprintf(out, "\\x00");

        fprintf(out, "\"");
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

    static int preprocess_resource(const char *path, const resource_t *resource, cvector<xml_word_t> *dict)
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
        char filename[PATH_MAX];
        snprintf(filename, PATH_MAX, "%s" FILE_SEPARATOR_S "%s", path, resource->id);

        FILE *in = fopen(filename, "r");
        if (in == NULL)
        {
            fprintf(stderr, "Could not open file %s\n", filename);
            XML_ParserFree(parser);
            return -3;
        }

        printf("Preprocessing file %s\n", filename);

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

    static int serialize_resource(FILE *out, const char *path, const resource_t *resource, cvector<xml_word_t> *dict)
    {
        // Initialize context
        xml_parser_t context;
        context.out         = out;
        context.pad_left    = 2;
        context.new_line    = false;
        context.dict        = dict;

        // Output resource descriptor
        fprintf(out,    "\t// Contents of file %s\n", resource->id);
        fprintf(out,    "\tstatic const char *xml_resource%s =", resource->text);

        // Create XML parser
        XML_Parser parser = XML_ParserCreate(NULL);
        if (parser == NULL)
            return -2;

        // Initialize parser
        XML_SetUserData(parser, &context);
        XML_SetElementHandler(parser, xml_start_element_handler, xml_end_element_handler);

        // Open input file
        char filename[PATH_MAX];
        snprintf(filename, PATH_MAX, "%s/%s", path, resource->id);

        FILE *in = fopen(filename, "r");
        if (in == NULL)
        {
            fprintf(stderr, "Could not open file %s\n", filename);
            XML_ParserFree(parser);
            return -3;
        }

        printf("Processing file %s\n", filename);

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

    static resource_t *create_xml_resource(const char *path, const char *name, size_t id)
    {
        resource_t *res = new resource_t;
        if (res == NULL)
            return NULL;

        char *ptr = NULL;
        if (path != NULL)
            asprintf(&ptr, "%s" FILE_SEPARATOR_S "%s", path, name);
        else
            ptr     = strdup(name);
        if (ptr == NULL)
        {
            delete res;
            return NULL;
        }
        res->id     = ptr;

        char *dst   = NULL;
        asprintf(&dst, "%08x", int(id));
        if (dst == NULL)
        {
            free(const_cast<char *>(res->id));
            delete res;
            return NULL;
        }
        res->text   = dst;
        return res;
    }

    static void free_xml_resource(resource_t *resource)
    {
        if (resource == NULL)
            return;
        if (resource->id != NULL)
            free(const_cast<char *>(resource->id));
        if (resource->text != NULL)
            free(const_cast<char *>(resource->text));
        delete resource;
    }

    int emit_dictionary(FILE *out, cvector<xml_word_t> *dict)
    {
        size_t items    = dict->size();

        // Sort dictionary
        for (size_t i=0; i<(items-1); ++i)
            for (size_t j=i+1; j<items; ++j)
            {
                xml_word_t *w1  = dict->at(i);
                xml_word_t *w2  = dict->at(j);
                if (w1->compare(w2) < 0)
                    dict->swap_unsafe(i, j);
            }

        // Output resource descriptor
        fprintf(out,    "\t// XML Dictionary\n");
        fprintf(out,    "\textern const char *xml_dictionary;\n\n");
        fprintf(out,    "\tconst char *xml_dictionary =\n");

        // Emit data
        size_t offset = 0;
        for (size_t i=0; i<items; ++i)
        {
            xml_word_t *w   = dict->at(i);
            w->offset       = offset;
            fprintf(out, "\t\tK(\"");

            // Output string
            for (const char *p=w->text; *p != '\0'; ++p)
            {
                if (((*p) >= 0x20) && ((*p) <= 0x7f))
                    fputc(*p, out);
                else
                    fprintf(out, "\" \"\\x%02x\" \"", uint8_t(*p));
            }

            fprintf(out, "\") // offset: 0x%08x, refs=%d\n", int(w->offset), int(w->refs));
            offset         += w->length + 1; // 1 char separator
        }

        fprintf(out, "\t\t;\n\n");

        return STATUS_SUCCESS;
    }

    int scan_directory(const char *basedir, const char *path, cvector<resource_t> &resources)
    {
        char *realpath = NULL;
        if (path != NULL)
            asprintf(&realpath, "%s" FILE_SEPARATOR_S "%s", basedir, path);
        else
            realpath = strdup(basedir);

        // Try to scan directory
        DIR *dirhdl     = opendir(realpath);
        if (dirhdl == NULL)
        {
            fprintf(stderr, "Could not open directory %s\n", realpath);
            free(realpath);
            return -STATUS_IO_ERROR;
        }

        int result      = STATUS_OK;

        while (true)
        {
            // Read next entry
            struct dirent *ent  = readdir(dirhdl);
            if (ent == NULL)
                break;

            // Check file extension
            if (ent->d_type == DT_DIR)
            {
                // Skip dot and dot-dot
                if (!strcmp(ent->d_name, "."))
                    continue;
                else if (!strcmp(ent->d_name, ".."))
                    continue;
            #ifdef LSP_NO_EXPERIMENTAL
                if ((path == NULL) && (!strcmp(ent->d_name, "experimental")))
                    continue;
            #endif /* LSP_NO_EXPERIMENTAL */

                // Generate subdirectory name
                char *subdir = NULL;
                if (path != NULL)
                    asprintf(&subdir, "%s" FILE_SEPARATOR_S "%s", path, ent->d_name);
                else
                    subdir = strdup(ent->d_name);
                if (subdir == NULL)
                    return -STATUS_NO_MEM;

                result = scan_directory(basedir, subdir, resources);

                // Free the allocated resource
                free(subdir);
                if (result != STATUS_OK)
                    break;
            }
            else if (ent->d_type == DT_REG)
            {
                char *dot = strrchr(ent->d_name, '.');
                if (dot == NULL)
                    continue;
                else if (strcasecmp(dot, ".xml") != 0)
                    continue;

                // Generate resource descriptor
                resource_t *res = create_xml_resource(path, ent->d_name, resources.size());
                if (res == NULL)
                {
                    result = -STATUS_NO_MEM;
                    break;
                }

                // Add resource to list
                if (!resources.add(res))
                {
                    free_xml_resource(res);
                    result = -STATUS_NO_MEM;
                    break;
                }
            }
        }

        // Close directory
        closedir(dirhdl);
        free(realpath);

        return result;
    }

    int gen_xml_resource_file(const char *path, const char *fname)
    {
        FILE *out = fopen(fname, "w");
        if (out == NULL)
        {
            fprintf(stderr, "Error creating file %s\n", fname);
            return -1;
        }

        // Output information
        printf("Generating resource file %s\n", fname);

        // Write header
        fprintf(out,    "//------------------------------------------------------------------------------\n");
        fprintf(out,    "// File:            %s\n", fname);
        fprintf(out,    "// Description:     resource file containing parsed XML\n");
        fprintf(out,    "// \n");
        fprintf(out,    "// This is auto-generated file, do not edit!\n");
        fprintf(out,    "//------------------------------------------------------------------------------\n\n");

        fprintf(out,    "#include <core/types.h>\n\n");

        fprintf(out,    "// Control characters\n");
//        fprintf(out,    "#define X_TAG_OPEN      \"\\x55\"\n");
        fprintf(out,    "#define X_TAG_CLOSE     \"\\xff\"\n\n");
        fprintf(out,    "#define X_SEPARATOR     \"\\0\"\n\n");

        fprintf(out,    "// String encoders\n");
        fprintf(out,    "#define K(k)            k X_SEPARATOR\n");
        fprintf(out,    "#define O(x, count)     count x\n");
        fprintf(out,    "#define C(x)            X_TAG_CLOSE \n");
        fprintf(out,    "#define P(k, v)         k v \n\n");

        fprintf(out,    "// Resource definition\n");
        fprintf(out,    "namespace lsp\n");
        fprintf(out,    "{\n");

        // Convert XML files into CPP code
        cvector<resource_t> resources;
        cvector<xml_word_t> dictionary;
        int result      = 0;

        // Try to scan directory
        result          = scan_directory(path, NULL, resources);
        if (result != STATUS_SUCCESS)
            return result;

        // Preprocess resources
        for (size_t i=0, n=resources.size(); i<n; ++i)
        {
            resource_t *res = resources[i];
            if (res == NULL)
            {
                result = -STATUS_NOT_FOUND;
                break;
            }

            result = preprocess_resource(path, res, &dictionary);
            if (result != STATUS_SUCCESS)
                break;
        }
        if (result != STATUS_SUCCESS)
            return result;

        // Emit dictionary
        result = emit_dictionary(out, &dictionary);
        if (result != STATUS_SUCCESS)
            return result;

        // Pass 2: Generate XML resources body
        for (size_t i=0; i<resources.size(); ++i)
        {
            resource_t *res = resources[i];
            if (res == NULL)
            {
                result = -STATUS_NOT_FOUND;
                break;
            }

            result = serialize_resource(out, path, res, &dictionary);
            if (result != STATUS_SUCCESS)
                break;
        }

        // Write footer
        if (result == STATUS_OK)
        {
            fprintf(out,    "\textern const resource_t xml_resources[] =\n");
            fprintf(out,    "\t{\n");

            for (size_t i=0; i<resources.size(); ++i)
            {
                resource_t *res  = resources[i];
                fprintf(out,    "\t\t{ \"%s\", xml_resource%s },\n", res->id, res->text);
                // Delete resource
                free_xml_resource(res);
            }

            fprintf(out,    "\t\t{ NULL, NULL }\n");
            fprintf(out,    "\t};\n\n");

            fprintf(out,    "}\n"); // End of namespace
        }

        // Free vector
        resources.clear();

        // Free dictionary
        for (size_t i=0; i<dictionary.size(); ++i)
            free(dictionary.at(i));
        dictionary.clear();

        return fclose(out);
    }
}

#ifndef LSP_IDE_DEBUG
int main(int argc, const char **argv)
{
    if (argc < 3)
    {
        fprintf(stderr, "required resource path and destination file name");
        return -1;
    }
    return lsp::gen_xml_resource_file(argv[1], argv[2]);
}
#endif /* LSP_IDE_DEBUG */
