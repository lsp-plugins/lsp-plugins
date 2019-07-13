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
    }
}



#endif /* UTILS_RESOURCE_GEN_RESOURCE_H_ */
