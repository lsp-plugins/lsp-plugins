/*
 * init.cpp
 *
 *  Created on: 12 мар. 2019 г.
 *      Author: sadko
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


