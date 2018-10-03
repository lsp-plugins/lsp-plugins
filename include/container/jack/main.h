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
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

namespace lsp
{
    static const char *jack_core_paths[] =
    {
        LSP_LIB_PREFIX("/lib"),
        LSP_LIB_PREFIX("/lib64"),
        LSP_LIB_PREFIX("/bin"),
        LSP_LIB_PREFIX("/sbin"),
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

    static jack_main_function_t lookup_jack_main(void **hInstance, const char *path)
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

                jack_main_function_t f = lookup_jack_main(hInstance, ptr);
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
                jack_get_version_t vf = reinterpret_cast<jack_get_version_t>(dlsym(inst, JACK_GET_VERSION_NAME));
                if (!vf)
                {
                    lsp_trace("version function %s not found: %s", JACK_GET_VERSION_NAME, dlerror());
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
                jack_main_function_t f = reinterpret_cast<jack_main_function_t>(dlsym(inst, JACK_MAIN_FUNCTION_NAME));
                if (!f)
                {
                    lsp_trace("function %s not found: %s", JACK_MAIN_FUNCTION_NAME, dlerror());
                    // Close library
                    dlclose(inst);
                    continue;
                }

                *hInstance = inst;
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

    static jack_main_function_t get_jack_main_function(void **hInstance, const char *binary_path)
    {
        lsp_debug("Trying to find CORE library");

        char path[PATH_MAX+1];
        jack_main_function_t jack_main  = NULL;

        // Try to find files in current directory
        if (binary_path != NULL)
        {
            strncpy(path, binary_path, PATH_MAX);
            char *rchr  = strrchr(path, FILE_SEPARATOR_C);
            if (rchr != NULL)
            {
                *rchr       = '\0';
                jack_main   = lookup_jack_main(hInstance, path);
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
            snprintf(path, PATH_MAX, "%s" FILE_SEPARATOR_S "lib", homedir);
            jack_main       = lookup_jack_main(hInstance, path);

            if (jack_main == NULL)
            {
                snprintf(path, PATH_MAX, "%s" FILE_SEPARATOR_S "lib64", homedir);
                jack_main       = lookup_jack_main(hInstance, path);

                if (jack_main == NULL)
                {
                    snprintf(path, PATH_MAX, "%s" FILE_SEPARATOR_S "bin", homedir);
                    jack_main       = lookup_jack_main(hInstance, path);
                }
            }
        }

        // Scan system directories
        if (jack_main == NULL)
        {
            for (const char **p = jack_core_paths; (p != NULL) && (*p != NULL); ++p)
            {
                jack_main       = lookup_jack_main(hInstance, *p);
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
    void *hInstance;

    jack_main_function_t jack_main = get_jack_main_function(&hInstance, argv[0]);
    if (jack_main == NULL)
    {
        lsp_error("Could not find LSP JACK core library");
        return -STATUS_NOT_FOUND;
    }

    int code = jack_main(JACK_PLUGIN_UID, argc, argv);

    if (hInstance != NULL)
        dlclose(hInstance);

    return code;
}

#endif /* _CONTAINER_JACK_MAIN_H_ */
