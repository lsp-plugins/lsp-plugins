/*
 * resource.h
 *
 *  Created on: 13 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UTILS_RESOURCE_GEN_RESOURCE_H_
#define UTILS_RESOURCE_GEN_RESOURCE_H_

#include <core/types.h>
#include <core/3d/Scene3D.h>
#include <core/io/Path.h>

namespace lsp
{
    namespace resgen
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

            if (resource->type == resource::RESOURCE_3D_SCENE)
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

        static scan_resource_t *create_resource(const LSPString *basedir, const LSPString *path, const LSPString *name, size_t type, size_t id)
        {
            // Allocate resource data and store ID
            scan_resource_t *r = new scan_resource_t;
            if (r == NULL)
                return NULL;

            r->id       = NULL;
            r->path     = NULL;
            r->hex      = NULL;
            r->type     = type;
            r->ptr      = NULL;

            // Generate resource absolute name
            status_t res;
            io::Path tmp_path;
            if ((res = tmp_path.set(basedir)) != STATUS_OK)
                return NULL;

            if ((path != NULL) && (path->length() > 0))
            {
                if ((res = tmp_path.append_child(path)) != STATUS_OK)
                    return NULL;
            }
            if ((name != NULL) && (name->length() > 0))
            {
                if ((res = tmp_path.append_child(name)) != STATUS_OK)
                    return NULL;
            }

            r->path     = ::strdup(tmp_path.as_string()->get_utf8());
            if (r->path == NULL)
                return NULL;

            // Generate resource relative name
            io::Path base_path;
            if ((res = base_path.set(basedir)) != STATUS_OK)
                return NULL;
            if ((res = base_path.remove_last()) != STATUS_OK)
                return NULL;
            if ((res = tmp_path.remove_base(&base_path)) != STATUS_OK)
                return NULL;

            r->id       = ::strdup(tmp_path.as_string()->get_utf8());
            if (r->path == NULL)
            {
                ::free(const_cast<char *>(r->path));
                return NULL;
            }

            // Allocate resource unique identifier
            char *ptr   = NULL;
            int n       = asprintf(&ptr, "%08x", int(id));
            if ((n < 0) || (ptr == NULL))
            {
                free(const_cast<char *>(r->path));
                free(const_cast<char *>(r->id));
                delete r;
                return NULL;
            }
            r->hex      = ptr;

            // Store other fields
            return r;
        }

        int get_resource_type(const char *fname)
        {
            const char *dot = strrchr(fname, '.');
            if (dot == NULL)
                return resource::RESOURCE_UNKNOWN;

            if (strcasecmp(dot, ".xml") == 0)
                return resource::RESOURCE_XML;
            if (strcasecmp(dot, ".obj") == 0)
                return resource::RESOURCE_3D_SCENE;
            if (strcasecmp(dot, ".preset") == 0)
                return resource::RESOURCE_PRESET;
            if (strcasecmp(dot, ".json") == 0)
                return resource::RESOURCE_JSON;
            if (strcasecmp(dot, ".json5") == 0)
                return resource::RESOURCE_JSON;

            return resource::RESOURCE_UNKNOWN;
        }
    }
}



#endif /* UTILS_RESOURCE_GEN_RESOURCE_H_ */
