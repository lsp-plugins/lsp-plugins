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

#include <core/io/Dir.h>

#include <data/cvector.h>
#include <data/cstorage.h>

#include <metadata/metadata.h>
#include <plugins/plugins.h>
#include <utils/common.h>

#include <utils/resource_gen/resource.h>
#include <utils/resource_gen/wdict.h>
#include <utils/resource_gen/fdict.h>
#include <utils/resource_gen/serialize.h>

#include <utils/resource_gen/xml.h>
#include <utils/resource_gen/scene3d.h>
#include <utils/resource_gen/presets.h>
#include <utils/resource_gen/json.h>

namespace lsp
{
    typedef struct dirent_t
    {
        LSPString       name;
        io::fattr_t     fattr;
        int             type;
    } dirent_t;

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
                case resource::RESOURCE_XML: return preprocess_xml_resource(resource, sdict);
                case resource::RESOURCE_3D_SCENE: return preprocess_3d_scene(resource, sdict, fdict);
                case resource::RESOURCE_PRESET: return preprocess_preset(resource, sdict, fdict);
                case resource::RESOURCE_JSON: return preprocess_json(resource, sdict, fdict);
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
                case resource::RESOURCE_XML: return serialize_xml_resource(out, resource, dict);
                case resource::RESOURCE_3D_SCENE: return serialize_3d_scene(out, resource, dict, fdict);
                case resource::RESOURCE_PRESET: return serialize_preset(out, resource, dict, fdict);
                case resource::RESOURCE_JSON: return serialize_json(out, resource, dict, fdict);
                default: break;
            }

            return STATUS_OK;
        }

        void destroy_list(cvector<dirent_t> &items)
        {
            for (size_t i=0, n=items.size(); i<n; ++i)
            {
                dirent_t *de = items.at(i);
                if (de != NULL)
                    delete de;
            }
            items.clear();
        }

        status_t get_list(const LSPString *path, cvector<dirent_t> &items)
        {
            io::Dir dirhdl;
            destroy_list(items);

            status_t res = dirhdl.open(path);
            if (res != STATUS_OK)
                return res;

            LSPString ipath;
            io::fattr_t fattr;

            while (true)
            {
                // Read item
                res = dirhdl.reads(&ipath, &fattr);

                if (res == STATUS_EOF)
                {
                    if (items.size() > 1)
                    {
                        for (size_t i=0, n=items.size(); i<(n-1); ++i)
                            for (size_t j=i+1; j<n; ++j)
                            {
                                dirent_t *a = items.at(i);
                                dirent_t *b = items.at(j);
                                if (a->name.compare_to(&b->name) > 0)
                                    items.swap_unsafe(i, j);
                            }
                    }
                    return STATUS_OK;
                }
                else if (res != STATUS_OK)
                {
                    destroy_list(items);
                    return res;
                }

                // Analyze item
                if ((ipath.equals_ascii(".")) ||
                    (ipath.equals_ascii("..")))
                    continue;

                // Check file type
                int rtype = resource::RESOURCE_UNKNOWN;
                if (fattr.type == io::fattr_t::FT_REGULAR)
                {
                    rtype = get_resource_type(ipath.get_utf8());
                    if (rtype == resource::RESOURCE_UNKNOWN)
                        continue;
                }
                else if (fattr.type != io::fattr_t::FT_DIRECTORY)
                    continue;

                // Allocate record
                dirent_t *dent = new dirent_t;
                if (dent == NULL)
                {
                    destroy_list(items);
                    return STATUS_NO_MEM;
                }

                dent->name.take(&ipath);
                dent->fattr     = fattr;
                dent->type      = rtype;

                if (!items.add(dent))
                {
                    delete dent;
                    destroy_list(items);
                    return STATUS_NO_MEM;
                }
            }
        }

        int scan_directory(const LSPString *basedir, const LSPString *path, cvector<scan_resource_t> &resources, size_t *resource_id)
        {
            // Form the path of file
            LSPString realpath;
            if (!realpath.set(basedir))
                return STATUS_NO_MEM;
            if (path != NULL)
            {
                if (!realpath.append_ascii(FILE_SEPARATOR_S))
                    return STATUS_NO_MEM;
                if (!realpath.append(path))
                    return STATUS_NO_MEM;
            }

            // Obtain list of files in directory
            cvector<dirent_t> items;
            status_t result = get_list(&realpath, items);
            if (result != STATUS_OK)
                return result;

            // Try to scan directory
            LSPString subdir;

            for (size_t i=0, n=items.size(); i<n; ++i)
            {
                dirent_t *de = items.at(i);

                // Generate resource name
                subdir.clear();
                if (path != NULL)
                {
                    if (!subdir.append(path))
                    {
                        result = STATUS_NO_MEM;
                        break;
                    }
                    if (!subdir.append_ascii(FILE_SEPARATOR_S))
                    {
                        result = STATUS_NO_MEM;
                        break;
                    }
                }
                if (!subdir.append(&de->name))
                {
                    result = STATUS_NO_MEM;
                    break;
                }

                if (de->fattr.type == io::fattr_t::FT_DIRECTORY)
                {
                #ifdef LSP_NO_EXPERIMENTAL
                    if ((path == NULL) && (de->name.equals_ascii("experimental")))
                        continue;
                #endif /* LSP_NO_EXPERIMENTAL */

                    // Scan sub-directory
                    result = scan_directory(basedir, &subdir, resources, resource_id);
                    if (result != STATUS_OK)
                        break;
                }
                else if (de->fattr.type == io::fattr_t::FT_REGULAR)
                {
                    // Generate resource descriptor
                    scan_resource_t *res = create_resource(basedir, path, &de->name, de->type, (*resource_id)++);
                    if (res == NULL)
                    {
                        result = STATUS_NO_MEM;
                        break;
                    }

                    // Add resource to list
                    if (!resources.add(res))
                    {
                        free_resource(res);
                        result = STATUS_NO_MEM;
                        break;
                    }
                    printf("Found resource: %s\n", res->path);
                }
            }

            destroy_list(items);
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

            fprintf(out,    "// String encoders\n");
            fprintf(out,    "#define K(k)            k \"\\0\"\n");
            fprintf(out,    "#define O(x, count)     count x\n");
            fprintf(out,    "#define C(x)            \"\\xff\" \n");
            fprintf(out,    "#define P(k, v)         k v \n\n");

            fprintf(out,    "// Resource definition\n");
            fprintf(out,    "namespace lsp\n");
            fprintf(out,    "{\n");
            fprintf(out,    "namespace resource\n");
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
                fprintf(out,    "\tconst resource_t builtin_resources[] =\n");
                fprintf(out,    "\t{\n");

                for (size_t i=0; i<resources.size(); ++i)
                {
                    scan_resource_t *res  = resources[i];
                    fprintf(out,    "\t\t{ \"%s\", builtin_resource%s, %d },\n", res->id, res->hex, res->type);
                }

                fprintf(out,    "\t\t{ NULL, NULL, %d }\n", resource::RESOURCE_UNKNOWN);
                fprintf(out,    "\t};\n\n");

                fprintf(out,    "} /* namespace resource */\n"); // End of namespace
                fprintf(out,    "} /* namespace lsp */\n"); // End of namespace
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

            LSPString path;
            while (i < argc)
            {
                if (!path.set_native(argv[i++]))
                    return STATUS_NO_MEM;

                printf("Scanning resources in path: %s\n", path.get_native());
                res = scan_directory(&path, NULL, resources, &resource_id);
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
