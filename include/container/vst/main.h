/*
 * main.h
 *
 *  Created on: 31 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_VST_MAIN_H_
#define _CONTAINER_VST_MAIN_H_

// Do not use tracefile because this file does not use jack-core
#ifdef LSP_TRACEFILE
    #undef LSP_TRACEFILE
#endif /* LSP_TRACEFILE */

#include <core/debug.h>
#include <core/types.h>
#include <container/vst/defs.h>

// System libraries
#include <core/debug.h>
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

namespace lsp
{
    static const char *vst_core_paths[] =
    {
        LSP_LIB_PREFIX("/lib"),
        LSP_LIB_PREFIX("/lib64"),
        LSP_LIB_PREFIX("/bin"),
        LSP_LIB_PREFIX("/sbin"),
        "/usr/local/lib64",
        "/usr/lib64",
        "/lib64",
        "/usr/local/lib",
        "/usr/lib" ,
        "/lib",
        NULL
    };

    static void *hInstance = NULL;
    static vst_create_instance_t factory = NULL;

    // The factory for creating plugin instances
    static vst_create_instance_t lookup_factory(const char *path)
    {
        lsp_trace("Searching core library at %s", path);

        // Try to open directory
        DIR *d = opendir(path);
        if (d == NULL)
            return NULL;

        struct dirent *de;
        char *ptr = NULL;

        while ((de = readdir(d)) != NULL)
        {
            // Free previously used string
            if (ptr != NULL)
                free(ptr);

            // Skip dot and dotdot
            ptr = de->d_name;
            if ((ptr[0] == '.') && ((ptr[1] == '\0') || ((ptr[1] == '.') || (ptr[2] == '\0'))))
            {
                ptr = NULL;
                continue;
            }

            // Allocate path string
            ptr = NULL;
            asprintf(&ptr, "%s" FILE_SEPARATOR_S "%s", path, de->d_name);
            if (ptr == NULL)
                continue;

            // Scan symbolic link if present
            if (de->d_type == DT_LNK)
            {
                struct stat st;
                if (stat(ptr, &st) != 0)
                    continue;

                if (S_ISDIR(st.st_mode))
                    de->d_type = DT_DIR;
                else if (S_ISREG(st.st_mode))
                    de->d_type = DT_REG;
            }

            // Analyze file
            if (de->d_type == DT_DIR)
            {
                // Skip directory if it doesn't contain 'lsp-plugins' in name
                if (strstr(de->d_name, LSP_ARTIFACT_ID) == NULL)
                    continue;

                vst_create_instance_t f = lookup_factory(ptr);
                if (f != NULL)
                {
                    free(ptr);
                    closedir(d);
                    return f;
                }
            }
            else if (de->d_type == DT_REG)
            {
                // Skip library if it doesn't contain 'lsp-plugins' in name
                if ((strstr(de->d_name, LSP_ARTIFACT_ID) == NULL) || (strstr(de->d_name, ".so") == NULL))
                    continue;

                lsp_trace("Trying library %s", ptr);

                // Try to load library
                void *inst = dlopen (ptr, RTLD_NOW);
                if (!inst)
                {
                    lsp_trace("library %s not loaded: %s", ptr, dlerror());
                    continue;
                }

                // Fetch version function
                vst_get_version_t vf = reinterpret_cast<vst_get_version_t>(dlsym(inst, VST_GET_VERSION_STRNAME));
                if (!vf)
                {
                    lsp_trace("version function %s not found: %s", VST_GET_VERSION_STRNAME, dlerror());
                    // Close library
                    dlclose(inst);
                    continue;
                }

                // Check package version
                if (strcmp(vf(), LSP_MAIN_VERSION) != 0)
                {
                    lsp_trace("wrong version %s returned, expected %s, ignoring binary", vf(), LSP_MAIN_VERSION);
                    // Close library
                    dlclose(inst);
                    continue;
                }

                // Fetch function
                vst_create_instance_t f = reinterpret_cast<vst_create_instance_t>(dlsym(inst, VST_CREATE_INSTANCE_STRNAME));
                if (!f)
                {
                    lsp_trace("function %s not found: %s", VST_CREATE_INSTANCE_STRNAME, dlerror());
                    // Close library
                    dlclose(inst);
                    continue;
                }

                hInstance = inst;
                free(ptr);
                closedir(d);
                return f;
            }
        }

        // Free previously used string, close directory and exit
        if (ptr != NULL)
            free(ptr);
        closedir(d);
        return NULL;
    }

    static vst_create_instance_t get_vst_main_function()
    {
        if (factory != NULL)
            return factory;

        lsp_debug("Trying to find CORE library");

        const char *homedir = getenv("HOME");
        char *buf = NULL;

        if (homedir == NULL)
        {
            struct passwd pwd, *result;
            size_t bufsize;

            bufsize = sysconf(_SC_GETPW_R_SIZE_MAX);
            if (bufsize <= 0)           // Value was indeterminate
                bufsize = 0x10000;          // Should be more than enough

            // Create buffer and fetch home directory
            buf = new char[bufsize];
            if (buf != NULL)
            {
                if (getpwuid_r(getuid(), &pwd, buf, bufsize, &result) == 0)
                    homedir = result->pw_dir;
            }
        }

        // Initialize factory with NULL
        char path[PATH_MAX];

        if (homedir != NULL)
        {
            if (factory == NULL)
            {
                lsp_trace("home directory = %s", homedir);
                snprintf(path, PATH_MAX, "%s" FILE_SEPARATOR_S ".vst", homedir);
                factory     = lookup_factory(path);
            }
            if (factory == NULL)
            {
                snprintf(path, PATH_MAX, "%s" FILE_SEPARATOR_S ".lxvst", homedir);
                factory     = lookup_factory(path);
            }
            if (factory == NULL)
            {
                snprintf(path, PATH_MAX, "%s" FILE_SEPARATOR_S "vst", homedir);
                factory     = lookup_factory(path);
            }
            if (factory == NULL)
            {
                snprintf(path, PATH_MAX, "%s" FILE_SEPARATOR_S "lxvst", homedir);
                factory     = lookup_factory(path);
            }
        }

        if (factory == NULL)
        {
            for (const char **p = vst_core_paths; (p != NULL) && (*p != NULL); ++p)
            {
                snprintf(path, PATH_MAX, "%s" FILE_SEPARATOR_S "vst", *p);
                factory     = lookup_factory(path);
                if (factory != NULL)
                    break;
                snprintf(path, PATH_MAX, "%s" FILE_SEPARATOR_S "lxvst", *p);
                factory     = lookup_factory(path);
                if (factory != NULL)
                    break;
            }
        }

        // Delete buffer if allocated
        if (buf != NULL)
            delete [] buf;

        // Return factory instance (if present)
        return factory;
    }
};

// The main function
VST_MAIN(callback)
{
    using namespace lsp;

    // Get VST Version of the Host
    if (!callback (NULL, audioMasterVersion, 0, 0, NULL, 0.0f))
    {
        lsp_error("audioMastercallback failed request");
        return 0;  // old version
    }

    // Check that we need to instantiate the factory
    lsp_trace("Getting factory");

    vst_create_instance_t f = get_vst_main_function();

    // Create effect
    AEffect *effect     = NULL;

    if (f != NULL)
        effect = f(VST_PLUGIN_UID, callback);
    else
        lsp_error("Could not find VST core library");

    // Return VST AEffect structure
    return effect;
}

// This should be included to generate other VST stuff
#include <3rdparty/steinberg/vst2main.h>

#endif /* _CONTAINER_VST_MAIN_H_ */
