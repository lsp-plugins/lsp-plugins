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

#ifndef UI_XMLHANDLER_H_
#define UI_XMLHANDLER_H_

#include <core/files/xml/IXMLHandler.h>
#include <ui/XMLNode.h>
#include <data/cvector.h>
#include <core/resource.h>

namespace lsp
{
    
    class XMLHandler: public xml::IXMLHandler
    {
        private:
            XMLHandler & operator = (const XMLHandler &);

        protected:
            cvector<XMLNode>    vHandlers;
            cvector<LSPString>  vElement;
            LSPString           sPath;

        protected:
            void            drop_element();
            LSPString      *fetch_element_string(const void **data);

        public:
            explicit XMLHandler();
            explicit XMLHandler(XMLNode *root);
            virtual ~XMLHandler();

        public:
            virtual status_t start_element(const LSPString *name, const LSPString * const *atts);

            virtual status_t end_element(const LSPString *name);

        public:
            /**
             * Parse resource from file
             * @param path path to the file
             * @param root root node that will handle XML data
             * @return status of operation
             */
            status_t parse_file(const LSPString *path, XMLNode *root);

            /**
             * Parse resource from resource descriptor
             * @param rs resource descriptor
             * @param root root node that will handle XML data
             * @return status of operation
             */
            status_t parse_resource(const resource::resource_t *rs, XMLNode *root);

            /**
             * Parse resource at specified URI. Depending on compilation flags,
             * the URI will point at builtin resource or at local filesystem resource
             * @param uri URI of the resource
             * @param root root node that will handle XML data
             * @return status of operation
             */
            status_t parse(const LSPString *uri, XMLNode *root);

            /**
             * Parse resource at specified URI. Depending on compilation flags,
             * the URI will point at builtin resource or at local filesystem resource
             * @param uri URI of the resource
             * @param root root node that will handle XML data
             * @return status of operation
             */
            status_t parse(const char *uri, XMLNode *root);
    };

} /* namespace lsp */

#endif /* UI_XMLHANDLER_H_ */
