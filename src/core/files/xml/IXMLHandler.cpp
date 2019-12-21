/*
 * IXMLHandler.cpp
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#include <core/files/xml/IXMLHandler.h>

namespace lsp
{
    namespace xml
    {
        
        IXMLHandler::IXMLHandler()
        {
        }
        
        IXMLHandler::~IXMLHandler()
        {
        }

        status_t IXMLHandler::start_document(xml_version_t xversion, const LSPString *version, const LSPString *encoding, bool standalone)
        {
            return STATUS_OK;
        }

        status_t IXMLHandler::end_document()
        {
            return STATUS_OK;
        }

        status_t IXMLHandler::cdata(const LSPString *cdata)
        {
            return STATUS_OK;
        }

        status_t IXMLHandler::characters(const LSPString *text)
        {
            return STATUS_OK;
        }

        status_t IXMLHandler::comment(const LSPString *text)
        {
            return STATUS_OK;
        }

        status_t IXMLHandler::doctype(const LSPString *name, const LSPString *pub, const LSPString *sys)
        {
            return STATUS_OK;
        }

        status_t IXMLHandler::resolve(LSPString *value, const LSPString *name)
        {
            return STATUS_OK;
        }

        status_t IXMLHandler::start_element(const LSPString *name, const LSPString * const *atts)
        {
            return STATUS_OK;
        }

        status_t IXMLHandler::end_element(const LSPString *name)
        {
            return STATUS_OK;
        }

        status_t IXMLHandler::processing(const LSPString *name, const LSPString *args)
        {
            return STATUS_OK;
        }
    
    } /* namespace xml */
} /* namespace lsp */
