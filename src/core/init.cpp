/*
 * init.cpp
 *
 *  Created on: 12 мар. 2019 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/init.h>
#include <core/io/charset.h>
#include <core/stdlib/string.h>
#include <locale.h>

namespace lsp
{
    void init()
    {
#ifdef PLATFORM_WINDOWS
        char tmpbuf[80];

        // Get default codepage for the UI
        size_t cp = codepage_from_name(NULL);
        sprintf(tmpbuf, ".%d", cp);
        setlocale(LC_ALL, tmpbuf);
#endif /* PLATFORM_WINDOWS */
    }
}


