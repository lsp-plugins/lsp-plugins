/*
 * BuiltinDictionary.cpp
 *
 *  Created on: 27 февр. 2020 г.
 *      Author: sadko
 */

#include <data/cvector.h>

#include <core/debug.h>
#include <core/stdlib/string.h>
#include <core/i18n/BuiltinDictionary.h>

namespace lsp
{
    
    BuiltinDictionary::BuiltinDictionary()
    {
    }
    
    BuiltinDictionary::~BuiltinDictionary()
    {
        // Free all used structures
        for (size_t i=0, n=vNodes.size(); i<n; ++i)
        {
            node_t *node = vNodes.at(i);
            if (node->pChild != NULL)
                delete node->pChild;
        }
        vNodes.flush();
    }

    status_t BuiltinDictionary::add_node(const node_t *src)
    {
        // Perform binary search, the item should not exist
        ssize_t first = 0, last = vNodes.size()-1;
        while (first <= last)
        {
            ssize_t curr = (first + last) >> 1;
            node_t *node = vNodes.at(curr);
            int cmp = ::strcmp(node->sKey, src->sKey);
            if (cmp > 0)
                last    = curr - 1;
            else if (cmp < 0)
                first   = curr + 1;
            else
                return STATUS_BAD_FORMAT;
        }

        // Add node
//        lsp_trace("Insert node %s at position %d", src->sKey, int(first));
        return (vNodes.insert(first, src)) ?  STATUS_OK : STATUS_NO_MEM;
    }

    status_t BuiltinDictionary::parse_dictionary(const resource::resource_t *r)
    {
        BuiltinDictionary *curr = NULL;
        cvector<BuiltinDictionary> stack;

        status_t res;
        node_t node;
        node.bBad       = false;

        for (const void *ptr = r->data; ; )
        {
            // Fetch the token
            uint8_t v = resource::fetch_byte(&ptr);
            if (v == 0)
                break;

            // Analyze token
            switch (v)
            {
                // Start of new object
                case '{':
//                    lsp_trace("Object start");
                    if (curr == NULL)
                    {
                        curr            = this;
                        break;
                    }

                    if (!stack.push(curr))
                        return STATUS_NO_MEM;

                    // Add current dictionary to stack
                    if ((node.pChild = new BuiltinDictionary()) == NULL)
                        return STATUS_NO_MEM;

                    node.sValue     = NULL;
                    if ((res = curr->add_node(&node)) != STATUS_OK)
                        return res;

                    curr            = node.pChild;
                    node.sKey       = NULL;
                    node.sValue     = NULL;
                    node.pChild     = NULL;
                    break;

                // End of current object
                case '}':
//                    lsp_trace("Object end");
                    if (!stack.pop(&curr))
                        curr = NULL;
                    else if (curr == NULL)
                        return STATUS_BAD_STATE;
                    break;

                // JSON Property key
                case ':':
//                    lsp_trace("Property key");
                    if (curr == NULL)
                        return STATUS_BAD_STATE;

                    node.sKey   = resource::fetch_dstring(&ptr);
//                    lsp_trace("  property key: %s", node.sKey);
                    if (!node.sKey)
                        return STATUS_CORRUPTED;
                    break;

                // JSON string value
                case '\"':
//                    lsp_trace("Property value");
                    if (curr == NULL)
                        return STATUS_BAD_STATE;

                    node.sValue     = resource::fetch_dstring(&ptr);
//                    lsp_trace("  property value: %s", node.sValue);
                    if (!node.sValue)
                        return STATUS_CORRUPTED;
                    node.pChild     = NULL;
                    if ((res = curr->add_node(&node)) != STATUS_OK)
                        return res;
                    node.sKey       = NULL;
                    node.sValue     = NULL;
                    break;

                // Other values are invalid
                default:
                    return STATUS_CORRUPTED;
            }
        }

//        lsp_trace("End of file");

        // Check final state
        if ((stack.size() > 0) || (curr != NULL))
            return STATUS_BAD_STATE;

        return STATUS_OK;
    }

#ifdef LSP_TRACE
    void BuiltinDictionary::dump(size_t offset)
    {
        LSPString pad;
        for (size_t i=0; i<offset; ++i)
            pad.append(' ');

        for (size_t i=0, n=vNodes.size(); i<n; ++i)
        {
            node_t *node = vNodes.get(i);
            if (node->pChild != NULL)
            {
                lsp_trace("%s%s={", pad.get_utf8(), node->sKey);
                node->pChild->dump(offset + 2);
                lsp_trace("%s}", pad.get_utf8());
            }
            else
                lsp_trace("%s%s=%s", pad.get_utf8(), node->sKey, node->sValue);
        }
    }
#endif

    status_t BuiltinDictionary::init(const LSPString *path)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;

        LSPString tpath;
        if (!tpath.set(path))
            return STATUS_NO_MEM;

        // Try to load JSON resource
        const resource::resource_t *rs = resource::get(path->get_utf8(), resource::RESOURCE_JSON);
        if (rs == NULL)
            return STATUS_NOT_FOUND;

        // Parse dictionary
        BuiltinDictionary tmp;
        status_t res = tmp.parse_dictionary(rs);
        if (res == STATUS_OK)
        {
            sPath.swap(&tpath);
            vNodes.swap(&tmp.vNodes);
        }

#ifdef LSP_TRACE
        lsp_trace("Dictionary %s dump:", path->get_utf8());
        lsp_trace("{");
        dump(2);
        lsp_trace("}");
#endif

        return STATUS_OK;
    }

    BuiltinDictionary::node_t *BuiltinDictionary::find_node(const char *key)
    {
        // Perform binary search
        ssize_t first = 0, last = vNodes.size()-1, idx = 0;
        while (first <= last)
        {
            idx = (first + last) >> 1;
            node_t *node = vNodes.at(idx);
            int cmp = ::strcmp(node->sKey, key);
//            lsp_trace("strcmp %s <-> %s -> %d", node->sKey, key, cmp);

            if (cmp > 0)
                last    = idx - 1;
            else if (cmp < 0)
                first   = idx + 1;
            else
                return node;
        }

        return NULL;
    }

    status_t BuiltinDictionary::lookup(const char *key, LSPString *value)
    {
        if (key == NULL)
            return STATUS_INVALID_VALUE;

//        lsp_trace("Lookup value: %s", key);
        node_t *node;
        BuiltinDictionary *curr = this;

        // Need to lookup sub-node?
        while (true)
        {
//            lsp_trace("Current path: %s", key);
            const char *split = ::strchr(key, '.');
            if (split == NULL)
                break;;

            // Allocate temporary string
            size_t len = split - key;
            char *tmp = reinterpret_cast<char *>(::malloc(len + 1));
            if (tmp == NULL)
                return STATUS_NO_MEM;
            ::memcpy(tmp, key, len);
            tmp[len] = '\0';

//            lsp_trace("Lookup child: \"%s\"", tmp);
            node = curr->find_node(tmp);
            ::free(tmp);

            if ((node == NULL) || (node->pChild == NULL))
            {
//                lsp_trace("Node not found: node=%p, pChild=%p", node, (node != NULL) ? node->pChild : NULL);
                return STATUS_NOT_FOUND;
            }

//            lsp_trace("Accessing to child dictionary %p with key: %s", node->pChild, &split[1]);
            key = &split[1];
            curr = node->pChild;
        }

        // No need to lookup
        node = curr->find_node(key);
        if ((node == NULL) || (node->pChild != NULL))
            return STATUS_NOT_FOUND;
        if ((value != NULL) && (!value->set_utf8(node->sValue)))
            return STATUS_NO_MEM;

        return STATUS_OK;
    }

    status_t BuiltinDictionary::lookup(const char *key, IDictionary **value)
    {
        if (key == NULL)
            return STATUS_INVALID_VALUE;

//        lsp_trace("Lookup dictionary: %s", key);

        node_t *node;
        BuiltinDictionary *curr = this;

        // Need to lookup sub-nodes?
        while (true)
        {
            const char *split = ::strchr(key, '.');
            if (split == NULL)
                break;

            // Allocate temporary string
            size_t len = split - key;
            char *tmp = reinterpret_cast<char *>(::malloc(len + 1));
            if (tmp == NULL)
                return STATUS_NO_MEM;
            ::memcpy(tmp, key, len);
            tmp[len] = '\0';

//            lsp_trace("Lookup child: %s", tmp);
            node = curr->find_node(tmp);
            ::free(tmp);

            if ((node == NULL) || (node->pChild == NULL))
                return STATUS_NOT_FOUND;

//            lsp_trace("Accessing to child dictionary %p with key: %s", node->pChild, &split[1]);
            key = &split[1];
            curr = node->pChild;
        }

        // No need to lookup
        node = curr->find_node(key);
        if ((node == NULL) || (node->pChild == NULL))
            return STATUS_NOT_FOUND;
        if (value != NULL)
            *value = node->pChild;

        return STATUS_OK;
    }

    status_t BuiltinDictionary::lookup(const LSPString *key, LSPString *value)
    {
        if (key == NULL)
            return STATUS_BAD_ARGUMENTS;

        return lookup(key->get_utf8(), value);
    }

    status_t BuiltinDictionary::lookup(const LSPString *key, IDictionary **value)
    {
        if (key == NULL)
            return STATUS_BAD_ARGUMENTS;

        return lookup(key->get_utf8(), value);
    }

    status_t BuiltinDictionary::get_value(size_t index, LSPString *key, LSPString *value)
    {
        node_t *node = vNodes.get(index);
        if ((node == NULL) || (node->pChild != NULL))
            return STATUS_NOT_FOUND;

        if ((key != NULL) && (!key->set_utf8(node->sKey)))
            return STATUS_NO_MEM;

        if ((value != NULL) && (!value->set_utf8(node->sValue)))
            return STATUS_NO_MEM;

        return STATUS_OK;
    }

    status_t BuiltinDictionary::get_child(size_t index, LSPString *key, IDictionary **dict)
    {
        node_t *node = vNodes.get(index);
        if ((node == NULL) || (node->pChild == NULL))
            return STATUS_NOT_FOUND;

        if ((key != NULL) && (!key->set_utf8(node->sKey)))
            return STATUS_NO_MEM;

        if (dict != NULL)
            *dict = node->pChild;

        return STATUS_OK;
    }

    size_t BuiltinDictionary::size()
    {
        return vNodes.size();
    }

} /* namespace lsp */
