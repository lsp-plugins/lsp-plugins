#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

#include <core/types.h>

#include <core/types.h>
#include <core/status.h>
#include <core/resource.h>
#include <core/files/Model3DFile.h>

#include <data/cvector.h>
#include <data/cstorage.h>

#include <metadata/metadata.h>
#include <plugins/plugins.h>

#include <utils/resource_gen/resource.h>
#include <utils/resource_gen/wdict.h>
#include <utils/resource_gen/fdict.h>
#include <utils/resource_gen/serialize.h>

#include <utils/resource_gen/xml.h>
#include <utils/resource_gen/scene3d.h>
#include <utils/resource_gen/presets.h>

namespace lsp
{
    namespace resgen
    {
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
                case RESOURCE_PRESET: return preprocess_preset(resource, sdict, fdict);
                default: break;
            }

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
                case RESOURCE_PRESET: return serialize_preset(out, resource, dict, fdict);
                default: break;
            }

            return STATUS_OK;
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

            fprintf(out,    "#include <core/resource.h>\n\n");

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

        status_t main(int argc, const char **argv)
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
}

#ifndef LSP_IDE_DEBUG
int main(int argc, const char **argv)
{
    return lsp::resgen::main(argc, argv);
}
#endif /* LSP_IDE_DEBUG */
