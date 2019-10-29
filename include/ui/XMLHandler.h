/*
 * XMLHandler.h
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#ifndef UI_XMLHANDLER_H_
#define UI_XMLHANDLER_H_

#include <core/files/xml/IXMLHandler.h>
#include <ui/XMLNode.h>
#include <data/cvector.h>

namespace lsp
{
    
    class XMLHandler: public xml::IXMLHandler
    {
        private:
            XMLHandler & operator = (const XMLHandler &);

        protected:
            cvector<XMLNode>    vHandlers;
            XMLNode            *pRoot;

        public:
            explicit XMLHandler(XMLNode *root);
            virtual ~XMLHandler();

        public:
            virtual status_t start_document(xml::xml_version_t xversion, const LSPString *version, const LSPString *encoding, bool standalone);

            virtual status_t start_element(const LSPString *name, const LSPString * const *atts);

            virtual status_t end_element(const LSPString *name);
    };

} /* namespace lsp */

#endif /* UI_XMLHANDLER_H_ */
