/*
 * XMLHandler.cpp
 *
 *  Created on: 29 окт. 2019 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/files/xml/PushParser.h>
#include <ui/XMLHandler.h>
#include <data/cvector.h>

// In built-in resources, this byte indicates the close tag
// We assume that 254 attributes is pretty enough for any element
#define XML_CLOSE_TAG       0xff

namespace lsp
{
    
    XMLHandler::XMLHandler()
    {
    }
    
    XMLHandler::XMLHandler(XMLNode *root)
    {
        vHandlers.add(root);
    }

    XMLHandler::~XMLHandler()
    {
        vHandlers.flush();
        drop_element();
    }

    status_t XMLHandler::start_element(const LSPString *name, const LSPString * const *atts)
    {
        XMLNode *top        = vHandlers.last();
        XMLNode *child      = NULL;
        //lsp_trace("start: %s", name->get_utf8());

        // Analyze
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

        //lsp_trace("end: %s", name->get_utf8());

        // Obtain handlers
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

    void XMLHandler::drop_element()
    {
        for (size_t i=0, n=vElement.size(); i<n; ++i)
        {
            LSPString *s = vElement.at(i);
            if (s != NULL)
                delete s;
        }
        vElement.flush();
    }

    LSPString *XMLHandler::fetch_element_string(const void **data)
    {
        const char *s = resource::fetch_dstring(data);
        if (s == NULL)
            return NULL;

        LSPString *str = new LSPString();
        if (!str->set_utf8(s))
        {
            delete str;
            return NULL;
        }
        else if (!vElement.add(str))
        {
            delete str;
            return NULL;
        }
        return str;
    }

    status_t XMLHandler::parse_file(const LSPString *path, XMLNode *root)
    {
        // Initialize
        sPath.clear();
        drop_element();
        if (!vHandlers.push(root))
            return STATUS_NO_MEM;

        // Perform parse
        xml::PushParser parser;
        return parser.parse_file(this, path, "UTF-8");
    }

    status_t XMLHandler::parse_resource(const resource::resource_t *rs, XMLNode *root)
    {
        // Obtain resource
        status_t res;
        LSPString *s;
        lsp_trace("Resource id=%s, type=%d, data=%p", rs->id, int(rs->type), rs->data);

        // Initialize
        sPath.clear();
        drop_element();
        if (!vHandlers.push(root))
            return STATUS_NO_MEM;

        // Process resource data
        const void *data = rs->data;
        if ((res = root->enter()) != STATUS_OK)
            return res;

        do
        {
            size_t token = resource::fetch_byte(&data);
            //lsp_trace("token = 0x%02x, path=%s", int(token), sPath.get_utf8());

            if (token != XML_CLOSE_TAG)
            {
                // Get element name
                if ((s = fetch_element_string(&data)) == NULL)
                    return STATUS_NO_MEM;
                //lsp_trace("element = %s", s->get_utf8());

                // Update current path
                if (!sPath.append('/'))
                    return STATUS_NO_MEM;
                if (!sPath.append(s))
                    return STATUS_NO_MEM;

                // Fill list with attributes
                for (size_t i=0; i<token; ++i)
                {
                    if ((s = fetch_element_string(&data)) == NULL)
                        return STATUS_NO_MEM;
                    //lsp_trace("  attribute = %s", s->get_utf8());
                    if ((s = fetch_element_string(&data)) == NULL)
                        return STATUS_NO_MEM;
                    //lsp_trace("  value     = %s", s->get_utf8());
                }

                // Add NULL-terminating element
                if (!vElement.add(NULL))
                    return STATUS_NO_MEM;

                // Issue callback
                LSPString **items = vElement.get_array();
                if ((res = start_element(items[0], &items[1])) != STATUS_OK)
                    return res;

                // Drop last used element
                drop_element();
            }
            else
            {
                // Obtain real element name
                LSPString ename;
                ssize_t idx = sPath.rindex_of('/');
                if (idx < 0)
                    return STATUS_CORRUPTED;
                if (!ename.set(&sPath, idx+1))
                    return STATUS_NO_MEM;

                // Issue callback
                if ((res = end_element(&ename)) != STATUS_OK)
                    return res;

                // Update path
                sPath.set_length(idx);
            }
        }
        while (!sPath.is_empty());

        return root->quit();
    }

    status_t XMLHandler::parse(const char *uri, XMLNode *root)
    {
        LSPString tmp;
        if (!tmp.set_utf8(uri))
            return STATUS_NO_MEM;
        return parse(&tmp, root);
    }

    status_t XMLHandler::parse(const LSPString *uri, XMLNode *root)
    {
        // Check for directive of using built-in resource
        if (uri->starts_with_ascii(LSP_BUILTIN_PREFIX))
        {
            LSPString bpath;
            if (!bpath.set(uri, LSP_BUILTIN_PREFIX_LEN))
                return STATUS_NO_MEM;
            const resource::resource_t *rs = resource::get(uri->get_utf8(), resource::RESOURCE_XML);
            return (rs != NULL) ? parse_resource(rs, root) : STATUS_NOT_FOUND;
        }

#ifndef LSP_BUILTIN_RESOURCES
        // Form the proper path to resource and call file parsing
        status_t res;
        io::Path p;
        if ((res = p.set(LSP_RESOURCE_PATH)) != STATUS_OK)
            return res;
        if ((res = p.append_child(uri)) != STATUS_OK)
            return res;
        return parse_file(p.as_string(), root);
#else
        const resource::resource_t *rs = resource::get(uri->get_utf8(), resource::RESOURCE_XML);
        return (rs != NULL) ? parse_resource(rs, root) : STATUS_NOT_FOUND;
#endif /* LSP_BUILTIN_RESOURCES */
    }

} /* namespace lsp */
