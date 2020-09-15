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
