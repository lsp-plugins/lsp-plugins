/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 14 февр. 2019 г.
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

#ifndef INCLUDE_CONTAINER_COMMON_WINLIB_H_
#define INCLUDE_CONTAINER_COMMON_WINLIB_H_

#include <core/types.h>

#ifndef PLATFORM_WINDOWS
    #error "This header should be used for Windows-compatible OS only"
#endif /* PLATFORM_WINDOWS */

#include <stdio.h>
#include <libloaderapi.h>

namespace lsp
{
    WCHAR *get_library_path()
    {
        WCHAR *path = reinterpret_cast<WCHAR *>(::malloc((PATH_MAX + 1) * sizeof(WCHAR)));
        if (path == NULL)
            return NULL;

        HMODULE hm = NULL;

        if (!::GetModuleHandleExW(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCWSTR>(&get_library_path),
                &hm)
           )
        {
            int ret = ::GetLastError();
            fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
            return NULL;
        }

        if (::GetModuleFileNameW(hm, path, PATH_MAX) == 0)
        {
            int ret = ::GetLastError();
            fprintf(stderr, "GetModuleFileName failed, error = %d\n", ret);
            return NULL;
        }

        return path;
    }

    void free_library_path(WCHAR *path)
    {
        if (path != NULL)
            free(path);
    }
}

#endif /* INCLUDE_CONTAINER_COMMON_WINLIB_H_ */
