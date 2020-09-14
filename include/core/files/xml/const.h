/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 24 окт. 2019 г.
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

#ifndef CORE_FILES_XML_CONST_H_
#define CORE_FILES_XML_CONST_H_

#include <core/types.h>

namespace lsp
{
    namespace xml
    {
        enum xml_version_t
        {
            XML_VERSION_1_0,
            XML_VERSION_1_1
        };

        enum xml_token_t
        {
            XT_ATTRIBUTE,
            XT_CDATA,
            XT_CHARACTERS,
            XT_COMMENT,
            XT_DTD,
            XT_END_DOCUMENT,
            XT_END_ELEMENT,
            XT_ENTITY_RESOLVE,
            XT_PROCESSING_INSTRUCTION,
            XT_START_DOCUMENT,
            XT_START_ELEMENT
        };

        bool is_valid_char(lsp_swchar_t c, xml_version_t version);
        bool is_name_first(lsp_swchar_t c);
        bool is_name_next(lsp_swchar_t c);
        bool is_whitespace(lsp_swchar_t c);
        bool is_pubid_char(lsp_swchar_t c);
        bool is_restricted_char(lsp_swchar_t c, xml_version_t version);
        bool is_encoding_first(lsp_swchar_t c);
        bool is_encoding_next(lsp_swchar_t c);
    }
}

#endif /* CORE_FILES_XML_CONST_H_ */
