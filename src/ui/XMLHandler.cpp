/*
 * XMLHandler.cpp
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#include <ui/XMLHandler.h>

namespace lsp
{
    
    XMLHandler::XMLHandler(XMLNode *root)
    {
        pRoot   = root;
    }
    
    XMLHandler::~XMLHandler()
    {
        pRoot   = NULL;
        vHandlers.flush();
    }

    status_t XMLHandler::start_document(xml::xml_version_t xversion, const LSPString *version, const LSPString *encoding, bool standalone)
    {
        return (vHandlers.push(pRoot)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t XMLHandler::start_element(const LSPString *name, const LSPString * const *atts)
    {
        XMLNode *top        = vHandlers.last();
        XMLNode *child      = NULL;

        if (top != NULL)
        {
            status_t res = top->start_element(&child, name, atts);
            if ((res == STATUS_OK) && (child != NULL))
                res = child->enter();

            if (res != STATUS_OK)
                return res;
        }

        return (vHandlers.push(child)) ? STATUS_OK : STATUS_NO_MEM;
    }

    status_t XMLHandler::end_element(const LSPString *name)
    {
        status_t res;
        XMLNode *node = NULL, *top = NULL;

        if (!vHandlers.pop(&node))
            return STATUS_CORRUPTED;
        top     = vHandlers.last();

        // Call callbacks
        if (node != NULL)
        {
            if ((res = node->quit()) != STATUS_OK)
                return res;
        }
        if (top != NULL)
        {
            if ((res = top->completed(node)) != STATUS_OK)
                return res;
            if ((res = top->end_element(name)) != STATUS_OK)
                return res;
        }

        return STATUS_OK;
    }

} /* namespace lsp */
