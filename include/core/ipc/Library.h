/*
 * Library.h
 *
 *  Created on: 24 апр. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_IPC_LIBRARY_H_
#define CORE_IPC_LIBRARY_H_

#include <core/types.h>
#include <core/status.h>
#include <core/io/Path.h>

#if defined(PLATFORM_WINDOWS)
    #include <libloaderapi.h>
#else
    #include <dlfcn.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace ipc
    {
        /**
         * Class for loading dynamic libraries
         */
        class Library
        {
            private:
            #ifdef PLATFORM_WINDOWS
                HMODULE     hDlSym;
            #else
                void       *hDlSym;
            #endif
                status_t    nLastError;

                static  int hTag;

            private:
                Library & operator = (const Library &);

            public:
                explicit Library();
                ~Library();

            public:
                /**
                 * Check that library is opened
                 * @return true if library is opened
                 */
                inline bool id_opened() const { return hDlSym != NULL; }

                /**
                 * Return last error
                 * @return last error
                 */
                inline status_t last_error() const { return nLastError; }

                /**
                 * Load library at specified path
                 * @param path UTF-8 path
                 * @return status of operation
                 */
                status_t open(const char *path);

                /**
                 * Load library at specified path
                 * @param path path
                 * @return status of operation
                 */
                status_t open(const LSPString *path);

                /**
                 * Load library at specified path
                 * @param path UTF-8 path
                 * @return status of operation
                 */
                status_t open(const io::Path *path);

                /**
                 * Import symbol from library
                 * @param name symbol name
                 */
                void *import(const char *name);

                /**
                 * Import symbol from library
                 * @param name symbol name
                 */
                void *import(const LSPString *name);

                /**
                 * Unload loaded library
                 * @return status of operation
                 */
                status_t close();

                /**
                 * Swap contents with another library handle
                 * @param dst target library handle to perform swap
                 */
                void swap(Library *dst);

                /**
                 * Get path to the module by the specified address
                 * @param path variable to store path to the module
                 * @param ptr specified address
                 * @return status of operation
                 */
                static status_t get_module_file(LSPString *path, const void *ptr);

                /**
                 * Get path to the module by the specified address
                 * @param path variable to store path to the module
                 * @param ptr specified address
                 * @return status of operation
                 */
                static status_t get_module_file(io::Path *path, const void *ptr);

                /**
                 * Get path to the current module
                 * @param path variable to store path to the current module
                 * @return status of operation
                 */
                static inline status_t get_self_file(LSPString *path) { return get_module_file(path, &hTag); };

                /**
                 * Get path to the current module
                 * @param path variable to store path to the current module
                 * @return status of operation
                 */
                static inline status_t get_self_file(io::Path *path) { return get_module_file(path, &hTag); };
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IPC_LIBRARY_H_ */
