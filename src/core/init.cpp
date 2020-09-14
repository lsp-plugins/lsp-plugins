/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 12 мар. 2019 г.
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

#include <core/types.h>
#include <core/init.h>

#ifdef PLATFORM_WINDOWS
    #include <core/io/charset.h>
    #include <core/stdlib/string.h>
    #include <locale.h>

    #include <windows.h>
    #include <mfapi.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    void init()
    {
#ifdef PLATFORM_WINDOWS
        char tmpbuf[80];

        // Get default codepage for the UI
        size_t cp = codepage_from_name(NULL);
        ::sprintf(tmpbuf, ".%d", cp);
        ::setlocale(LC_ALL, tmpbuf);

        // Initialize the COM runtime.
        HRESULT hr = ::CoInitializeEx(0, COINIT_MULTITHREADED);
        if (SUCCEEDED(hr))
        {
            // Initialize the Media Foundation platform.
            ::MFStartup(MF_VERSION);
        }
#endif /* PLATFORM_WINDOWS */
    }

    void finalize()
    {
#ifdef PLATFORM_WINDOWS
        // Shut down Media Foundation.
        ::MFShutdown();

        // Shutdown COM runtime
        ::CoUninitialize();
#endif /* PLATFORM_WINDOWS */
    }
}


