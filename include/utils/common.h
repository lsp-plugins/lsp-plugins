/*
 * common.h
 *
 *  Created on: 15 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UTILS_COMMON_H_
#define UTILS_COMMON_H_

#include <core/resource.h>

namespace lsp
{
#ifdef LSP_BUILTIN_RESOURCES
    const resource_t builtin_resources[] =
    {
        { NULL, NULL, RESOURCE_UNKNOWN }
    };

    const char *string_dictionary = "";
    const float float_dictionary[] = { 0.0f };
#endif /* LSP_BUILTIN_RESOURCES */

}

#endif /* UTILS_COMMON_H_ */
