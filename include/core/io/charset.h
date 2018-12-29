/*
 * charset.h
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_IO_CHARSET_H_
#define INCLUDE_CORE_IO_CHARSET_H_

#include <core/types.h>

#if defined(PLATFORM_WINDOWS)
    #include <winnls.h>
#else
    #include <iconv.h>
    #include <locale.h>
#endif /* PLATFORM_WINDOWS */

#include <stdlib.h>
#include <stdio.h>

namespace lsp
{
#if defined(PLATFORM_WINDOWS)

    ssize_t get_codepage(LCID locale, bool ansi = true);

    ssize_t codepage_from_name(const char *charset);

#else

    iconv_t init_iconv_to_wchar_t(const char *charset);

    iconv_t init_iconv_from_wchar_t(const char *charset);

#endif /* PLATFORM_WINDOWS */
}

#endif /* INCLUDE_CORE_IO_CHARSET_H_ */
