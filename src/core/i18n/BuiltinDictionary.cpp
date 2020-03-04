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

    status_t BuiltinDictionary::parse_dictionary(const resource_t *r)
    {
        BuiltinDictionary *curr = NULL;
        cvector<BuiltinDictionary> stack;

        node_t node;

        for (const void *ptr = r->data; ; )
        {
            // Fetch the token
            uint8_t v = resource_fetch_byte(&ptr);
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
                    if (!curr->vNodes.add(&node))
                        return STATUS_NO_MEM;

                    curr            = node.pChild;
                    node.sKey       = NULL;
                    node.pChild     = NULL;
                    node.sValue     = NULL;
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

                    node.sKey   = resource_fetch_dstring(&ptr);
//                    lsp_trace("  property key: %s", node.sKey);
                    if (!node.sKey)
                        return STATUS_CORRUPTED;
                    break;

                // JSON string value
                case '\"':
//                    lsp_trace("Property value");
                    if (curr == NULL)
                        return STATUS_BAD_STATE;

                    node.sValue     = resource_fetch_dstring(&ptr);
//                    lsp_trace("  property value: %s", node.sValue);
                    if (!node.sValue)
                        return STATUS_CORRUPTED;
                    node.pChild     = NULL;
                    if (!curr->vNodes.add(&node))
                        return STATUS_NO_MEM;
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

    status_t BuiltinDictionary::init(const LSPString *path)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;

        LSPString tpath;
        if (!tpath.set(path))
            return STATUS_NO_MEM;

        // Try to load JSON resource
        const resource_t *rs = resource_get(path->get_utf8(), RESOURCE_JSON);
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

//        lsp_trace("Lookup key: %s", key);
        node_t *node;
        BuiltinDictionary *curr = this;

        // Need to lookup sub-node?
        while (true)
        {
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

//        lsp_trace("Lookup key: %s", key);

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
