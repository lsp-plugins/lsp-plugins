/*
 * charset.h
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#ifndef INCLUDE_CORE_IO_CHARSET_H_
#define INCLUDE_CORE_IO_CHARSET_H_

#include <core/types.h>
#include <iconv.h>

namespace lsp
{
    iconv_t init_iconv_to_wchar_t(const char *charset);

    iconv_t init_iconv_from_wchar_t(const char *charset);
}

#endif /* INCLUDE_CORE_IO_CHARSET_H_ */
