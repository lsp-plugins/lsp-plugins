/*
 * winpath.h
 *
 *  Created on: 14 февр. 2019 г.
 *      Author: sadko
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

        if (!::GetModuleHandleEx(
                GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                reinterpret_cast<LPCSTR>(&get_library_path),
                &hm)
           )
        {
            int ret = ::GetLastError();
            fprintf(stderr, "GetModuleHandle failed, error = %d\n", ret);
            return NULL;
        }

        if (::GetModuleFileNameW(hm, path, sizeof(path)) == 0)
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
