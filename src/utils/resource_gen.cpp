#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include <core/types.h>

#if defined(PLATFORM_WINDOWS)
    #include <msxml.h>
#else
    #include <expat.h>
#endif /* PLATFORM_WINDOWS */

#include <core/types.h>
#include <core/status.h>
#include <core/files/Model3DFile.h>

#include <data/cvector.h>
#include <data/cstorage.h>

#include <metadata/metadata.h>
#include <plugins/plugins.h>


namespace lsp
{
    typedef struct scan_resource_t
    {
        const char         *path;   // Real path to resource
        const char         *id;     // Resourse character identifier
        const char         *hex;    // Resource hexadecimal identifier
        int                 type;   // Resource type
        union
        {
            Scene3D        *scene;
            void           *ptr;
        };
    } scan_resource_t;

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

    typedef struct dict_float_t
    {
        float       value;
        size_t      refs;
        size_t      index;

        inline int compare(const dict_float_t *w) const
        {
            return refs - w->refs;
        }

        void swap(dict_float_t *b)
        {
            ::swap(value, b->value);
            ::swap(refs, b->refs);
            ::swap(index, b->index);
        }
    } dict_float_t;

    typedef struct xml_parser_t
    {
        FILE                       *out;
        size_t                      pad_left;
        bool                        new_line;
        cvector<xml_word_t>        *dict;
    } xml_parser_t;

    static void free_resource(scan_resource_t *resource)
    {
        if (resource == NULL)
            return;
        if (resource->path != NULL)
            free(const_cast<char *>(resource->path));
        if (resource->id != NULL)
            free(const_cast<char *>(resource->id));
        if (resource->hex != NULL)
            free(const_cast<char *>(resource->hex));

        if (resource->type == RESOURCE_3D_SCENE)
        {
            resource->scene->destroy();
            delete resource->scene;
            resource->scene = NULL;
        }
        delete resource;
    }

    static void free_resources(cvector<scan_resource_t> &resources)
    {
        for (size_t i=0, n=resources.size(); i<n; ++i)
            free_resource(resources[i]);
    }

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

    static dict_float_t *float_dict_get(cstorage<dict_float_t> *dict, float v)
    {
        for (size_t i=0, n=dict->size(); i<n; ++i)
        {
            dict_float_t *w   = dict->at(i);
            if (w->value == v)
                return w;
        }
        return NULL;
    }

    static bool float_dict_add(cstorage<dict_float_t> *dict, float value)
    {
        dict_float_t *item = float_dict_get(dict, value);
        if (item != NULL)
        {
            ++item->refs;
            return true;
        }

        if ((item = dict->add()) == NULL)
            return false;

        item->value = value;
        item->index = 0;
        item->refs  = 1;

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

    static void encode_string(FILE *out, const char *p)
    {
        bool quote = false;
        int tokens = 0;

        // Output string
        for (; *p != '\0'; ++p, ++tokens)
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

    static void encode_value(FILE *out, size_t value)
    {
        int tokens = 0;
        do
        {
            if ((tokens++) > 0)
                fputc(' ', out);
            size_t flag = (value > 0x7f) ? 0x80 : 0x00;
            fprintf(out, "\"\\x%02x\"", int((value & 0x7f) | flag));
            value >>= 7;
        } while (value > 0);
    }

    static void encode_word(FILE *out, cvector<xml_word_t> *dict, const char *key)
    {
        xml_word_t *w   = res_dict_get(dict, key);
        encode_value(out, (w != NULL) ? w->offset : 0);
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

    static int preprocess_3d_scene(scan_resource_t *resource, cstorage<dict_float_t> *dict)
    {
        Scene3D *pscene = NULL;
        Model3DFile file;

        // Load scene
        LSPString path;
        if (!path.set_native(resource->path))
            return STATUS_NO_MEM;

        status_t res = file.load(&pscene, &path);
        if (res == STATUS_OK)
        {
            // Vertices
            for (size_t i=0, n=pscene->num_vertexes(); i<n; ++i)
            {
                obj_vertex_t *v = pscene->vertex(i);
                if (!float_dict_add(dict, v->x))
                    return STATUS_NO_MEM;
                if (!float_dict_add(dict, v->y))
                    return STATUS_NO_MEM;
                if (!float_dict_add(dict, v->z))
                    return STATUS_NO_MEM;
            }

            // Normals
            for (size_t i=0, n=pscene->num_normals(); i<n; ++i)
            {
                obj_normal_t *v = pscene->normal(i);
                if (!float_dict_add(dict, v->dx))
                    return STATUS_NO_MEM;
                if (!float_dict_add(dict, v->dy))
                    return STATUS_NO_MEM;
                if (!float_dict_add(dict, v->dz))
                    return STATUS_NO_MEM;
            }

            resource->scene     = pscene;
        }

        return res;
    }

    static int preprocess_resource(scan_resource_t *resource,
            cvector<xml_word_t> *sdict,
            cstorage<dict_float_t> *fdict
        )
    {
        printf("Preprocessing resource file %s\n", resource->path);
        switch (resource->type)
        {
            case RESOURCE_XML: return preprocess_xml_resource(resource, sdict);
            case RESOURCE_3D_SCENE: return preprocess_3d_scene(resource, fdict);
            default: break;
        }

        return STATUS_OK;
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

    static int serialize_3d_scene(FILE *out, const scan_resource_t *resource, cstorage<dict_float_t> *fdict)
    {
        // Output resource descriptor
        fprintf(out,    "\t// Contents of file %s\n", resource->id);
        fprintf(out,    "\tstatic const char *builtin_resource%s =", resource->hex);

        dict_float_t *f;
        LSPString name;
        Scene3D *s      = resource->scene;

        // Encode vertices
        fprintf(out,    "\n\t\t");
        encode_value(out, s->num_vertexes());
        fprintf(out,    " // Vertex count");
        fprintf(out,    "\n\t\t\t// Vertex data { x, y, z }");
        for (size_t i=0, n=s->num_vertexes(); i<n; ++i)
        {
            obj_vertex_t *v = s->vertex(i);
            fprintf(out, "\n\t\t\t");

            f = float_dict_get(fdict, v->x);
            encode_value(out, f->index);
            fprintf(out, " ");

            f = float_dict_get(fdict, v->y);
            encode_value(out, f->index);
            fprintf(out, " ");

            f = float_dict_get(fdict, v->z);
            encode_value(out, f->index);
        }

        // Encode normals
        fprintf(out,    "\n\t\t");
        encode_value(out, s->num_normals());
        fprintf(out,    " // Normal count");
        fprintf(out,    "\n\t\t\t// Normal data { dx, dy, dz }");
        for (size_t i=0, n=s->num_normals(); i<n; ++i)
        {
            obj_normal_t *v = s->normal(i);
            fprintf(out, "\n\t\t\t");

            f = float_dict_get(fdict, v->dx);
            encode_value(out, f->index);
            fprintf(out, " ");

            f = float_dict_get(fdict, v->dy);
            encode_value(out, f->index);
            fprintf(out, " ");

            f = float_dict_get(fdict, v->dz);
            encode_value(out, f->index);
        }

        // Encode objects
        fprintf(out,    "\n\t\t");
        encode_value(out, s->num_objects());
        fprintf(out,    " // Objects count");
        for (size_t i=0, n=s->num_objects(); i<n; ++i)
        {
            Object3D *o = s->object(i);

            fprintf(out, "\n\t\t\t");
            o->get_name(&name);
            encode_string(out, name.get_utf8());
            fprintf(out, " // Object name");

            fprintf(out, "\n\t\t\t");
            encode_value(out, o->num_triangles());
            fprintf(out,    " // Object triangles { face_id, v1, v2, v3, n1, n2, n3 }");

            for (size_t j=0, m=o->num_triangles(); j<m; ++j)
            {
                obj_triangle_t *t = o->triangle(j);
                fprintf(out, "\n\t\t\t\t");

                encode_value(out, t->face);
                fprintf(out, " ");

                encode_value(out, t->v[0]->id);
                fprintf(out, " ");

                encode_value(out, t->v[1]->id);
                fprintf(out, " ");

                encode_value(out, t->v[2]->id);
                fprintf(out, " ");

                encode_value(out, t->n[0]->id);
                fprintf(out, " ");

                encode_value(out, t->n[1]->id);
                fprintf(out, " ");

                encode_value(out, t->n[2]->id);
                fprintf(out, " ");
            }
        }

        fprintf(out,    "\n\t\t;\n\n");

        return STATUS_OK;
    }

    static int serialize_resource(FILE *out,
            const scan_resource_t *resource,
            cvector<xml_word_t> *dict,
            cstorage<dict_float_t> *fdict
        )
    {
        printf("Serializing resource file %s\n", resource->path);
        switch (resource->type)
        {
            case RESOURCE_XML: return serialize_xml_resource(out, resource, dict);
            case RESOURCE_3D_SCENE: return serialize_3d_scene(out, resource, fdict);
            default: break;
        }

        return STATUS_OK;
    }

    static scan_resource_t *create_resource(const char *basedir, const char *path, const char *name, size_t type, size_t id)
    {
        // Allocate resource data and store ID
        scan_resource_t *res = new scan_resource_t;
        if (res == NULL)
            return NULL;

        res->id     = NULL;
        res->path   = NULL;
        res->hex    = NULL;
        res->type   = type;
        res->ptr    = NULL;

        char tmp_path[PATH_MAX + 1], *ptr;

        // Generate resource absolute name
        ptr         = tmp_path;
        *ptr        = '\0';
        if ((basedir != NULL) && (strlen(basedir) > 0))
        {
            ptr = stpcpy(ptr, basedir);
            ptr = stpcpy(ptr, FILE_SEPARATOR_S);
        }
        if ((path != NULL) && (strlen(path) > 0))
        {
            ptr = stpcpy(ptr, path);
            ptr = stpcpy(ptr, FILE_SEPARATOR_S);
        }
        ptr     = stpcpy(ptr, name);
        ptr     = strdup(tmp_path);
        if (ptr == NULL)
        {
            free_resource(res);
            return NULL;
        }
        res->path   = ptr;

        // Generate resource relative name
        ptr         = tmp_path;
        *ptr        = '\0';

        const char *group = strrchr(basedir, FILE_SEPARATOR_C);
        group = (group == NULL) ? basedir : &group[1];
        if ((group != NULL) && (strlen(group) > 0))
        {
            ptr = stpcpy(ptr, group);
            ptr = stpcpy(ptr, "/");
        }
        if ((path != NULL) && (strlen(path) > 0))
        {
            ptr = stpcpy(ptr, path);
            ptr = stpcpy(ptr, "/");
        }
        ptr     = stpcpy(ptr, name);
        for (ptr = tmp_path; *ptr != '\0'; ++ptr)
            if (*ptr == '\\')
                *ptr        = '/';
        ptr     = strdup(tmp_path);
        if (ptr == NULL)
        {
            free_resource(res);
            return NULL;
        }
        res->id     = ptr;

        // Allocate resource unique identifier
        ptr         = NULL;
        int n       = asprintf(&ptr, "%08x", int(id));
        if ((n < 0) || (ptr == NULL))
        {
            free(const_cast<char *>(res->id));
            delete res;
            return NULL;
        }
        res->hex    = ptr;

        // Store other fields
        return res;
    }

    int emit_string_dictionary(FILE *out, cvector<xml_word_t> *dict)
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
        for (size_t i=0; i<(items-1); ++i)
            for (size_t j=i+1; j<items; ++j)
            {
                dict_float_t *w1  = dict->at(i);
                dict_float_t *w2  = dict->at(j);
                if (w1->compare(w2) < 0)
                    w1->swap(w2);
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

    int get_resource_type(const char *fname)
    {
        const char *dot = strrchr(fname, '.');
        if (dot == NULL)
            return RESOURCE_UNKNOWN;

        if (strcasecmp(dot, ".xml") == 0)
            return RESOURCE_XML;
        if (strcasecmp(dot, ".obj") == 0)
            return RESOURCE_3D_SCENE;
        if (strcasecmp(dot, ".preset") == 0)
            return RESOURCE_PRESET;

        return RESOURCE_UNKNOWN;
    }

    int scan_directory(const char *basedir, const char *path, cvector<scan_resource_t> &resources, size_t *resource_id)
    {
        int n = 0;
        char *realpath = NULL;

        if (path != NULL)
            n = asprintf(&realpath, "%s" FILE_SEPARATOR_S "%s", basedir, path);
        else
            realpath = strdup(basedir);
        if ((n < 0) || (realpath == NULL))
            return -STATUS_NO_MEM;

        // Try to scan directory
        DIR *dirhdl     = opendir(realpath);
        if (dirhdl == NULL)
        {
            fprintf(stderr, "Could not open directory %s\n", realpath);
            free(realpath);
            return -STATUS_IO_ERROR;
        }

        int result      = STATUS_OK;
        struct stat st;

        while (true)
        {
            // Read next entry
            struct dirent *ent  = readdir(dirhdl);
            if (ent == NULL)
                break;

            // Skip dot and dot-dot
            if (!strcmp(ent->d_name, "."))
                continue;
            else if (!strcmp(ent->d_name, ".."))
                continue;

            // Obtain file type
            char *fname = NULL;
            n = asprintf(&fname, "%s" FILE_SEPARATOR_S "%s", realpath, ent->d_name);
            if ((n < 0) || (fname == NULL))
                continue;
            if (stat(fname, &st) < 0)
            {
                free(fname);
                continue;
            }
            free(fname);

            // Check file extension
            if (S_ISDIR(st.st_mode))
            {
            #ifdef LSP_NO_EXPERIMENTAL
                if ((path == NULL) && (!strcmp(ent->d_name, "experimental")))
                    continue;
            #endif /* LSP_NO_EXPERIMENTAL */

                // Generate subdirectory name
                char *subdir = NULL;
                if (path != NULL)
                    n = asprintf(&subdir, "%s" FILE_SEPARATOR_S "%s", path, ent->d_name);
                else
                    subdir = strdup(ent->d_name);
                if ((n < 0) || (subdir == NULL))
                    return -STATUS_NO_MEM;

                result = scan_directory(basedir, subdir, resources, resource_id);

                // Free the allocated resource
                free(subdir);
                if (result != STATUS_OK)
                    break;
            }
            else if (S_ISREG(st.st_mode))
            {
                int rtype = get_resource_type(ent->d_name);
                if (rtype == RESOURCE_UNKNOWN)
                    continue;

                // Generate resource descriptor
                scan_resource_t *res = create_resource(basedir, path, ent->d_name, rtype, (*resource_id)++);
                if (res == NULL)
                {
                    result = -STATUS_NO_MEM;
                    break;
                }

                // Add resource to list
                if (!resources.add(res))
                {
                    free_resource(res);
                    result = -STATUS_NO_MEM;
                    break;
                }
                printf("Found resource: %s" FILE_SEPARATOR_S "%s\n", path, ent->d_name);
            }
        }

        // Close directory
        closedir(dirhdl);
        free(realpath);

        return result;
    }

    int gen_resource_file(const char *fname, cvector<scan_resource_t> &resources)
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
        fprintf(out,    "// Description:     resource file containing builtin data\n");
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
        cvector<xml_word_t> sdict;
        cstorage<dict_float_t> fdict;
        int result      = 0;

        // Preprocess resources
        for (size_t i=0, n=resources.size(); i<n; ++i)
        {
            scan_resource_t *res = resources[i];
            if (res == NULL)
            {
                result = -STATUS_NOT_FOUND;
                break;
            }

            result = preprocess_resource(res, &sdict, &fdict);
            if (result != STATUS_OK)
                break;
        }
        if (result != STATUS_OK)
            return result;

        // Emit dictionary
        result = emit_string_dictionary(out, &sdict);
        if (result == STATUS_OK)
            result = emit_float_dictionary(out, &fdict);

        // Pass 2: Generate XML resources body
        if (result == STATUS_OK)
        {
            for (size_t i=0; i<resources.size(); ++i)
            {
                scan_resource_t *res = resources[i];
                if (res == NULL)
                {
                    result = -STATUS_NOT_FOUND;
                    break;
                }

                result = serialize_resource(out, res, &sdict, &fdict);
                if (result != STATUS_SUCCESS)
                    break;
            }
        }

        // Write footer
        if (result == STATUS_OK)
        {
            fprintf(out,    "\textern const resource_t builtin_resources[] =\n");
            fprintf(out,    "\t{\n");

            for (size_t i=0; i<resources.size(); ++i)
            {
                scan_resource_t *res  = resources[i];
                fprintf(out,    "\t\t{ \"%s\", builtin_resource%s, %d },\n", res->id, res->hex, res->type);
            }

            fprintf(out,    "\t\t{ NULL, NULL, %d }\n", RESOURCE_UNKNOWN);
            fprintf(out,    "\t};\n\n");

            fprintf(out,    "}\n"); // End of namespace
        }

        // Free dictionary
        for (size_t i=0; i<sdict.size(); ++i)
            free(sdict.at(i));
        sdict.flush();
        fdict.flush();

        return fclose(out);
    }

    status_t resource_gen_main(int argc, const char **argv)
    {
        dsp::init();

        const char *target = NULL;
        int i = 1;
        status_t res;

        // Get target file name
        if (i >= argc)
        {
            fprintf(stderr, "Required path to output file");
            return STATUS_UNSPECIFIED;
        }
        target = argv[i++];

        // Scan resources
        cvector<scan_resource_t> resources;
        size_t resource_id = 0;

        while (i < argc)
        {
            const char *path = argv[i++];
            printf("Scanning resources in path: %s\n", path);
            res = scan_directory(path, NULL, resources, &resource_id);
            if (res != STATUS_SUCCESS)
            {
                free_resources(resources);
                return res;
            }
        }

        res = gen_resource_file(target, resources);
        free_resources(resources);
        return res;
    }
}

#ifndef LSP_IDE_DEBUG
int main(int argc, const char **argv)
{
    return lsp::resource_gen_main(argc, argv);
}
#endif /* LSP_IDE_DEBUG */
