/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 апр. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
