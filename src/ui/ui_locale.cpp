/*
 * ui_locale.cpp
 *
 *  Created on: 23 окт. 2017 г.
 *      Author: sadko
 */

#include <locale.h>
#include <stdlib.h>
#include <string.h>

#include <ui/ui_locale.h>

namespace lsp
{
    void init_locale()
    {
        char *var = getenv("LANG");
        if ((var == NULL) || (strlen(var) <= 0))
            return;

        setlocale(LC_ALL, var);
        setlocale(LC_COLLATE, var);
        setlocale(LC_CTYPE, var);
        setlocale(LC_MESSAGES, var);
        setlocale(LC_MONETARY, var);
        setlocale(LC_NUMERIC, var);
        setlocale(LC_TIME, var);

        // Linux-specific stuff
        #if defined(PLATFORM_LINUX)
            setlocale(LC_PAPER, var);
            setlocale(LC_TELEPHONE, var);

            setlocale(LC_NAME, var);
            setlocale(LC_ADDRESS, var);
            setlocale(LC_IDENTIFICATION, var);
            setlocale(LC_MEASUREMENT, var);
        #endif /* PLATFORM_LINUX */
    }
}


