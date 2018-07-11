/*
 * main.h
 *
 *  Created on: 31 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_VST_MAIN_H_
#define _CONTAINER_VST_MAIN_H_

#include <core/debug.h>
#include <container/vst/defs.h>

// System libraries
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <dlfcn.h>
#include <stdlib.h>

#define LSP_VST_CORE        LSP_ARTIFACT_ID "-vst-core.so"

namespace lsp
{
    static const char *vst_core_paths[] =
    {
        "/usr/local/lib64",
        "/usr/lib64",
        "/lib64",
        "/usr/local/lib",
        "/usr/lib",
        "/lib",
        NULL
    };

    static char bundle_path[PATH_MAX];

    static void *hInstance;

    // The factory for creating plugin instances
    static vst_create_instance_t factory = NULL;

    vst_create_instance_t lookup_factory(const char *path)
    {
        lsp_trace("Trying shared library %s", path);

        // Generate file name
        char fname[PATH_MAX];
        snprintf(fname, PATH_MAX, "%s/%s", path, LSP_VST_CORE);

        // Try to load library
        hInstance = dlopen (fname, RTLD_NOW);
        if (!hInstance)
        {
            lsp_trace("library %s not loaded: %s", fname, dlerror());
            return NULL;
        }

        // Fetch function
        vst_create_instance_t f = reinterpret_cast<vst_create_instance_t>(dlsym(hInstance, VST_CREATE_INSTANCE_STRNAME));
        if (!f)
        {
            lsp_trace("function %s not found: %s", VST_CREATE_INSTANCE_STRNAME, dlerror());

            // Close library
            dlclose(hInstance);
            hInstance   = NULL;
        }

        // Store bundle path
        strncpy(bundle_path, path, PATH_MAX);
        return f;
    }

    vst_create_instance_t get_factory()
    {
        if (factory != NULL)
            return factory;

        lsp_debug("Trying to find CORE library %s", LSP_VST_CORE);

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
            lsp_trace("home directory = %s", homedir);
            snprintf(path, PATH_MAX, "%s/.vst", homedir);
            factory     = lookup_factory(path);

            if (factory == NULL)
            {
                snprintf(path, PATH_MAX, "%s/vst/", homedir);
                factory     = lookup_factory(path);
            }
        }

        if (factory == NULL)
        {
            for (const char **p = vst_core_paths; (p != NULL) && (*p != NULL); ++p)
            {
                snprintf(path, PATH_MAX, "%s/vst", *p);
                factory     = lookup_factory(path);
                if (factory != NULL)
                    break;

                snprintf(path, PATH_MAX, "%s/lxvst", *p);
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

extern "C"
{
    // The main function
    VST_EXPORT AEffect* VSTPluginMain (audioMasterCallback callback)
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
        vst_create_instance_t f = get_factory();

        // Create effect
        AEffect *effect     = NULL;

        if (f != NULL)
            effect = f(bundle_path, VST_PLUGIN_UID, callback);
        else
            lsp_error("Could not find VST core library %s", LSP_VST_CORE);

        // Return VST AEffect structure
        return effect;
    }

    // support for old hosts not looking for VSTPluginMain
    #if (TARGET_API_MAC_CARBON && __ppc__)
    VST_EXPORT AEffect* main_macho (audioMasterCallback audioMaster) { return VSTPluginMain (audioMaster); }
    #elif WIN32
    VST_EXPORT AEffect* MAIN (audioMasterCallback audioMaster) { return VSTPluginMain (audioMaster); }
    #elif BEOS
    VST_EXPORT AEffect* main_plugin (audioMasterCallback audioMaster) { return VSTPluginMain (audioMaster); }
    #endif

    //------------------------------------------------------------------------
#if WIN32
    #include <windows.h>

    BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
    {
        using namespace lsp;
        hInstance = hInst;
        return 1;
    }
#endif
} // extern "C"

#endif /* _CONTAINER_VST_MAIN_H_ */
