/*
 * main.h
 *
 *  Created on: 31 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_VST_MAIN_H_
#define _CONTAINER_VST_MAIN_H_

#include <container/vst/defs.h>
#include <core/metadata.h>
#include <core/debug.h>

// System libraries
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <dlfcn.h>

namespace lsp
{
    const char *vst_core_paths[] =
    {
        "/usr/local/lib64",
        "/usr/lib64",
        "/lib64",
        "/usr/local/lib",
        "/usr/lib",
        "/lib",
        NULL
    };

    static void *hInstance;
};

extern "C"
{
    #if defined (__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
        #define VST_EXPORT  __attribute__ ((visibility ("default")))
    #else
        #define VST_EXPORT
    #endif

    vst_create_instance_t lookup_factory(const char *path)
    {
        using namespace lsp;

        // Try to load library
        hInstance = dlopen (path, RTLD_NOW);
        if (!hInstance)
        {
            lsp_trace("library %s not loaded: %s", path, dlerror());
            return NULL;
        }

        vst_create_instance_t f = reinterpret_cast<vst_create_instance_t>(dlsym(hInstance, VST_CREATE_INSTANCE_STRNAME));
        if (!f)
        {
            lsp_trace("function %s not found: %s", VST_CREATE_INSTANCE_STRNAME, dlerror());

            // Close libraryb
            dlclose(hInstance);
            hInstance   = NULL;
        }

        return f;
    }

    // The main function
    VST_EXPORT AEffect* VSTPluginMain (audioMasterCallback callback)
    {
        // Get VST Version of the Host
        if (!callback (NULL, audioMasterVersion, 0, NULL, 0, 0.0f))
            return 0;  // old version

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
        vst_create_instance_t factory = NULL;
        char path[PATH_MAX];

        if (homedir != NULL)
        {
            snprintf(path, PATH_MAX, "%s/.vst/%s.so", homedir, LSP_VST_BINARY);
            factory     = lookup_factory(path);
        }

        if (! factory)
        {
            for (const char **p = vst_core_paths; *p != NULL; ++p)
            {
                snprintf(path, PATH_MAX, "%s/%s.so", p, LSP_VST_BINARY);
                factory     = lookup_factory(path);
                if (factory != NULL)
                    break;
            }
        }

        // Create effect
        AEffect *effect     = NULL;

        if (factory != NULL)
            effect = factory(VST_PLUGIN_UID, callback);
        else
            lsp_error("Could not find VST core library %s", LSP_VST_BINARY);

        // Delete buffer if allocated
        if (buf != NULL)
            delete [] buf;

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
