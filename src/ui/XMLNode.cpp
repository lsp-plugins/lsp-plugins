/*
 * XMLNode.cpp
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#include <ui/XMLNode.h>
#include <ui/XMLHandler.h>

namespace lsp
{
    //-------------------------------------------------------------------------
    // XML Node implementation
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

    //-------------------------------------------------------------------------
    // XML Playback Node implementation
    XMLPlaybackNode::xml_event_t::xml_event_t(event_t type)
    {
        nEvent      = type;
    }

    XMLPlaybackNode::xml_event_t::~xml_event_t()
    {
        for (size_t i=0, n=vData.size(); i<n; ++i)
        {
            LSPString *s = vData.at(i);
            if (s != NULL)
                delete s;
        }
        vData.flush();
    }

    status_t XMLPlaybackNode::xml_event_t::add_param(const LSPString *name)
    {
        LSPString *tmp;
        if ((tmp = name->clone()) == NULL)
            return STATUS_NO_MEM;
        else if (!vData.add(tmp))
        {
            delete tmp;
            return STATUS_NO_MEM;
        }
        return STATUS_OK;
    }

    XMLPlaybackNode::xml_event_t *XMLPlaybackNode::add_event(event_t ev)
    {
        xml_event_t *evt        = new xml_event_t(ev);
        if (evt == NULL)
            return NULL;
        else if (!vEvents.add(evt))
        {
            delete evt;
            evt = NULL;
        }
        return evt;
    }

    XMLPlaybackNode::XMLPlaybackNode(XMLNode *handler)
    {
        pHandler    = handler;
    }

    XMLPlaybackNode::~XMLPlaybackNode()
    {
        for (size_t i=0, n=vEvents.size(); i<n; ++i)
        {
            xml_event_t *ev = vEvents.at(i);
            if (ev != NULL)
                delete ev;
        }
        vEvents.flush();
    }

    status_t XMLPlaybackNode::init(const LSPString * const *atts)
    {
        return STATUS_OK;
    }

    status_t XMLPlaybackNode::playback_start_element(xml::IXMLHandler *handler, const LSPString *name, const LSPString * const *atts)
    {
        return handler->start_element(name, atts);
    }

    status_t XMLPlaybackNode::playback_end_element(xml::IXMLHandler *handler, const LSPString *name)
    {
        return handler->end_element(name);
    }

    status_t XMLPlaybackNode::playback()
    {
        status_t res = STATUS_OK;
        XMLHandler h(pHandler);

        for (size_t i=0, n=vEvents.size(); i<n; ++i)
        {
            // Fetch event
            xml_event_t *ev = vEvents.at(i);
            if (ev == NULL)
            {
                res = STATUS_CORRUPTED;
                break;
            }

            // Parse event
            LSPString **atts = ev->vData.get_array();
            switch (ev->nEvent)
            {
                case EVT_START_ELEMENT:
                    res = playback_start_element(&h, atts[0], &atts[1]);
                    break;
                case EVT_END_ELEMENT:
                    res = playback_end_element(&h, atts[0]);
                    break;
                default:
                    res = STATUS_CORRUPTED;
                    break;
            }

            // Check result
            if (res != STATUS_OK)
                break;
        }

        return res;
    }

    status_t XMLPlaybackNode::execute()
    {
        return playback();
    }

    status_t XMLPlaybackNode::start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts)
    {
        // Allocate event
        status_t res;
        xml_event_t *evt        = add_event(EVT_START_ELEMENT);
        if (evt == NULL)
            return STATUS_NO_MEM;

        // Clone element name
        if ((res = evt->add_param(name)) != STATUS_OK)
            return res;

        // Clone tag attributes
        for ( ; *atts != NULL; ++atts)
        {
            // Clone attribute
            if ((res = evt->add_param(*atts)) != STATUS_OK)
                return res;
        }

        // Add terminator
        if (!evt->vData.add(NULL))
            return STATUS_NO_MEM;

        // Increment level, set child to this
        *child = this;

        return STATUS_OK;
    }

    status_t XMLPlaybackNode::end_element(const LSPString *name)
    {
        // Allocate event and add parameter
        xml_event_t *evt        = add_event(EVT_END_ELEMENT);
        return (evt != NULL) ? evt->add_param(name) : STATUS_NO_MEM;
    }

} /* namespace lsp */
