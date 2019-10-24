/*
 * const.h
 *
 *  Created on: 24 окт. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_XML_CONST_H_
#define CORE_FILES_XML_CONST_H_

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
            XT_ENTITY_DECLARATION,
            XT_ENTITY_REFERENCE,
            XT_NAMESPACE,
            XT_NOTATION_DECLARATION,
            XT_PROCESSING_INSTRUCTION,
            XT_SPACE,
            XT_START_DOCUMENT,
            XT_START_ELEMENT
        };
    }
}

#endif /* INCLUDE_CORE_FILES_XML_CONST_H_ */
