/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 29 окт. 2019 г.
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

#ifndef CORE_FILES_XML_IXMLHANDLER_H_
#define CORE_FILES_XML_IXMLHANDLER_H_

#include <core/LSPString.h>
#include <core/files/xml/const.h>

namespace lsp
{
    namespace xml
    {
        /**
         * Interface for handling XML data
         */
        class IXMLHandler
        {
            private:
                IXMLHandler & operator = (const IXMLHandler &);

            public:
                explicit IXMLHandler();
                virtual ~IXMLHandler();

            public:
                /**
                 * Handle start of the document
                 * @param xversion the actual XML parser version used
                 * @param version the document version, may be NULL
                 * @param encoding document encoding, may be NULL
                 * @param standalone standalone flag
                 * @return status of operation
                 */
                virtual status_t start_document(xml_version_t xversion, const LSPString *version, const LSPString *encoding, bool standalone);

                /**
                 * Handle end of the document
                 * @return status of operation
                 */
                virtual status_t end_document();

                /**
                 * Handle character data (CDATA)
                 * @param cdata character data
                 * @return status of operation
                 */
                virtual status_t cdata(const LSPString *cdata);

                /**
                 * Handle text data (characters)
                 * @param text character data
                 * @return status of operation
                 */
                virtual status_t characters(const LSPString *text);

                /**
                 * Handle comment
                 * @param text text of the comment
                 * @return status of operation
                 */
                virtual status_t comment(const LSPString *text);

                /**
                 * Handle doctype
                 * @param name document type name
                 * @param pub public literal
                 * @param sys system literal
                 * @return status of operation
                 */
                virtual status_t doctype(const LSPString *name, const LSPString *pub, const LSPString *sys);

                /**
                 * Resolve entity reference
                 * @param value pointer to store the reference value
                 * @param name reference name
                 * @return status of operation
                 */
                virtual status_t resolve(LSPString *value, const LSPString *name);

                /**
                 * Handle start of the element
                 * @param name tag name
                 * @param atts NULL-terminated list of attributes: even index is name, odd index is value
                 * @return status of operation
                 */
                virtual status_t start_element(const LSPString *name, const LSPString * const *atts);

                /**
                 * Handle end of the element
                 * @param name element name
                 * @return status of operation
                 */
                virtual status_t end_element(const LSPString *name);

                /**
                 * Processing instruction
                 * @param name name of instruction
                 * @param args arguments of instruction
                 * @return status of operation
                 */
                virtual status_t processing(const LSPString *name, const LSPString *args);
        };
    
    } /* namespace xml */
} /* namespace lsp */

#endif /* CORE_FILES_XML_IXMLHANDLER_H_ */
