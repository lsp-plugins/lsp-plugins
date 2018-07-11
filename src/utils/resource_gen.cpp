#include <core/types.h>

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dirent.h>

#include <expat.h>

#include <core/metadata.h>
#include <plugins/plugins.h>
#include <data/cvector.h>

namespace lsp
{
    typedef struct xml_parser_t
    {
        FILE   *out;
        size_t  pad_left;
        bool    new_line;
    } xml_parser_t;

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
        fprintf(parser->out,    "O(\"%s\", \"\\x%02x\")", name, int(count));

        // Now output parameters
        while (*atts != NULL)
        {
            const XML_Char *k   = *atts++;
            const XML_Char *v   = *atts++;
            if ((k != NULL) && (v != NULL))
                fprintf(parser->out,    " P(\"%s\", \"%s\")", k, v);
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
        fprintf(parser->out,    "C(\"%s\")", name);

        parser->new_line    = true;
    }

    static int serialize_resource(FILE *out, const char *path, const resource_t *resource)
    {
        // Output resource descriptor
        fprintf(out,    "\t// Contents of file %s\n", resource->id);
        fprintf(out,    "\tstatic const char *xml_resource%s =", resource->text);

        // Create XML parser
        XML_Parser parser = XML_ParserCreate(NULL);
        if (parser == NULL)
            return -2;

        // Initialize parser
        xml_parser_t context;
        context.out         = out;
        context.pad_left    = 2;
        context.new_line    = false;

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

        fprintf(out,    "\n\t\tX_SEPARATOR;\n\n");

        return 0;
    }

    static resource_t *create_xml_resource(const char *name, size_t id)
    {
        resource_t *res = new resource_t;
        if (res == NULL)
            return NULL;
        res->id     = strdup(name);
        if (res->id == NULL)
        {
            delete res;
            return NULL;
        }
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

    int gen_resource_file(const char *path, const char *fname)
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
        fprintf(out,    "//------------------------------------------------------------------------------\n\n");

        fprintf(out,    "#include <core/types.h>\n\n");

        fprintf(out,    "// Control characters\n");
        fprintf(out,    "#define X_TAG_OPEN      \"\\x55\"\n");
        fprintf(out,    "#define X_TAG_CLOSE     \"\\xaa\"\n\n");
        fprintf(out,    "#define X_SEPARATOR     \"\\0\"\n\n");

        fprintf(out,    "// String encoders\n");
        fprintf(out,    "#define O(x, count)     X_TAG_OPEN count x X_SEPARATOR\n");
        fprintf(out,    "#define C(x)            X_TAG_CLOSE \n");
        fprintf(out,    "#define P(k, v)         k X_SEPARATOR v X_SEPARATOR\n\n");

        fprintf(out,    "// Resource definition\n");
        fprintf(out,    "namespace lsp\n");
        fprintf(out,    "{\n");

        // Convert XML files into CPP code
        cvector<resource_t> resources;
        int result      = 0;

        // Try to scan directory
        DIR *dirhdl     = opendir(path);
        if (dirhdl == NULL)
        {
            fprintf(stderr, "Could not open directory %s\n", path);
            return -5;
        }

        while (true)
        {
            // Read next entry
            struct dirent *ent  = readdir(dirhdl);
            if (ent == NULL)
                break;
            // Check file extension
            char *dot = strrchr(ent->d_name, '.');
            if ((dot != NULL) && (strcasecmp(dot, ".xml") == 0))
            {
                resource_t *res = create_xml_resource(ent->d_name, resources.size());
                if (res == NULL)
                {
                    result = -6;
                    break;
                }

                result = serialize_resource(out, path, res);
                if (result != 0)
                    break;

                if (!resources.add(res))
                {
                    free_xml_resource(res);
                    result = -7;
                    break;
                }
            }
        }

        // Close directory
        closedir(dirhdl);

        // Write footer
        if (result == 0)
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

        return fclose(out);
    }
}

#ifndef LSP_IDE_DEBUG
int main(int argc, const char **argv)
{
    if (argc < 3)
        fprintf(stderr, "required resource path and destination file name");
    return lsp::gen_resource_file(argv[1], argv[2]);
}
#endif /* LSP_IDE_DEBUG */
