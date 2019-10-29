/*
 * const.h
 *
 *  Created on: 24 окт. 2019 г.
 *      Author: sadko
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
