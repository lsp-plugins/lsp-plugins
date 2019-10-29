/*
 * XMLNode.cpp
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#include <ui/XMLNode.h>

namespace lsp
{
    
    XMLNode::XMLNode()
    {
    }
    
    XMLNode::~XMLNode()
    {
    }

    const LSPString *XMLNode::find_attribute(const LSPString * const *atts, const LSPString *name)
    {
        for ( ; *atts != NULL; atts += 2)
        {
            if (atts[0]->equals(name))
                return atts[1];
        }

        return NULL;
    }

    const LSPString *XMLNode::find_attribute(const LSPString * const *atts, const char *name)
    {
        LSPString tmp;
        if (!tmp.set_utf8(name))
            return NULL;
        return find_attribute(atts, &tmp);
    }

    status_t XMLNode::enter()
    {
        return STATUS_OK;
    }

    status_t XMLNode::start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts)
    {
        return STATUS_OK;
    }

    status_t XMLNode::end_element(const LSPString *name)
    {
        return STATUS_OK;
    }

    status_t XMLNode::quit()
    {
        return STATUS_OK;
    }

    status_t XMLNode::completed(XMLNode *child)
    {
        return STATUS_OK;
    }

} /* namespace lsp */
