/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 23 окт. 2017 г.
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


