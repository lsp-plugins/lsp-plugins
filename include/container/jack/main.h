/*
 * main.h
 *
 *  Created on: 31 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_JACK_MAIN_H_
#define _CONTAINER_JACK_MAIN_H_

// Do not use tracefile because this file does not use jack-core
#ifdef LSP_TRACEFILE
    #undef LSP_TRACEFILE
#endif /* LSP_TRACEFILE */

#include <core/types.h>
#include <core/status.h>
#include <core/debug.h>
#include <container/jack/defs.h>
#include <metadata/metadata.h>

// System libraries
#include <sys/types.h>
#include <unistd.h>
#include <pwd.h>
#include <dlfcn.h>
#include <stdlib.h>

#define LSP_VST_CORE       LSP_ARTIFACT_ID "-jack-core-" LSP_MAIN_VERSION "-" LSP_ARCHITECTURE ".so"

namespace lsp
{
    static const char *jack_core_paths[] =
    {
        "/usr/local/lib64",
        "/usr/lib64",
        "/lib64",
        "/usr/local/lib",
        "/usr/lib",
        "/lib",
        "/usr/local/bin",
        "/usr/bin",
        "/bin",
        "/usr/local/sbin",
        "/usr/sbin",
        "/sbin",
        NULL
    };

    static char bundle_path[PATH_MAX];

    static void *hInstance;

    static jack_main_function_t lookup_jack_main(const char *path)
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
        jack_main_function_t f = reinterpret_cast<jack_main_function_t>(dlsym(hInstance, JACK_MAIN_FUNCTION_NAME));
        if (!f)
        {
            lsp_trace("function %s not found: %s", JACK_MAIN_FUNCTION_NAME, dlerror());

            // Close library
            dlclose(hInstance);
            hInstance   = NULL;
        }

        // Store bundle path
        strncpy(bundle_path, path, PATH_MAX);
        return f;
    }

    static jack_main_function_t get_jack_main_function(const char *binary_path)
    {
        lsp_debug("Trying to find CORE library %s", LSP_VST_CORE);

        char path[PATH_MAX];
        jack_main_function_t jack_main  = NULL;

        // Try to find files in current directory
        if (binary_path != NULL)
        {
            strncpy(path, binary_path, PATH_MAX);
            char *rchr  = strrchr(path, '/');
            if (rchr != NULL)
            {
                *rchr       = '\0';
                jack_main   = lookup_jack_main(path);
            }
        }

        // Get the home directory
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
        if (homedir != NULL)
        {
            lsp_trace("home directory = %s", homedir);
            snprintf(path, PATH_MAX, "%s/lib", homedir);
            jack_main       = lookup_jack_main(path);

            if (jack_main == NULL)
            {
                snprintf(path, PATH_MAX, "%s/lib64", homedir);
                jack_main       = lookup_jack_main(path);

                if (jack_main == NULL)
                {
                    snprintf(path, PATH_MAX, "%s/bin", homedir);
                    jack_main       = lookup_jack_main(path);
                }
            }
        }

        // Scan system directories
        if (jack_main == NULL)
        {
            for (const char **p = jack_core_paths; (p != NULL) && (*p != NULL); ++p)
            {
                jack_main       = lookup_jack_main(*p);
                if (jack_main != NULL)
                    break;
            }
        }

        // Delete buffer if allocated
        if (buf != NULL)
            delete [] buf;

        // Return factory instance (if present)
        return jack_main;
    }
};

//------------------------------------------------------------------------
int main(int argc, const char **argv)
{
    using namespace lsp;

    jack_main_function_t jack_main = get_jack_main_function(argv[0]);
    if (jack_main == NULL)
    {
        lsp_error("Could not find LSP JACK core library");
        return -STATUS_NOT_FOUND;
    }

    return jack_main(JACK_PLUGIN_UID, argc, argv);
}

#endif /* _CONTAINER_JACK_MAIN_H_ */
