/*
 * Dictionary.cpp
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/i18n/JsonDictionary.h>
#include <core/files/json/token.h>
#include <core/files/json/Parser.h>

namespace lsp
{
    
    JsonDictionary::JsonDictionary()
    {
    }
    
    JsonDictionary::~JsonDictionary()
    {
        // Recursively drop the dictionary tree
        for (size_t i=0, n = vNodes.size(); i<n; ++i)
        {
            node_t *node = vNodes.get(i);
            if (node == NULL)
                continue;
            if (node->pChild != NULL)
                delete node->pChild;
            delete node;
        }
        vNodes.flush();
    }

    status_t JsonDictionary::init(const LSPString *path)
    {
        json::Parser p;
        JsonDictionary tmp;

        status_t res = p.open(path, json::JSON_VERSION5);
        if (res == STATUS_OK)
            res = tmp.parse_json(&p);

        if (res != STATUS_OK)
            p.close();
        else
            res = p.close();

        if (res == STATUS_OK)
            vNodes.swap_data(&tmp.vNodes);

        return res;
    }

    status_t JsonDictionary::add_node(const node_t *src)
    {
        // Perform binary search, the item should not exist
        ssize_t first = 0, last = vNodes.size()-1;
        while (first <= last)
        {
            ssize_t curr = (first + last) >> 1;
            node_t *node = vNodes.at(curr);
            int cmp = node->sKey.compare_to(&src->sKey);

            if (cmp > 0)
                last    = curr - 1;
            else if (cmp < 0)
                first   = curr + 1;
            else
                return STATUS_BAD_FORMAT;
        }

        // Create new item and insert into list
        node_t *x = new node_t;
        if (x == NULL)
            return STATUS_NO_MEM;

        // Initialize key
        if (!x->sKey.set(&src->sKey))
        {
            delete x;
            return STATUS_NO_MEM;
        }

        // Initialize value
        if (src->pChild != NULL)
            x->pChild   = src->pChild;
        else if (x->sValue.set(&src->sValue))
            x->pChild   = NULL;
        else
        {
            delete x;
            return STATUS_NO_MEM;
        }

        // Add node
        if (!vNodes.insert(x, first))
        {
            delete x;
            return STATUS_NO_MEM;
        }

        return STATUS_OK;
    }

    status_t JsonDictionary::parse_json(json::Parser *p)
    {
        status_t res;
        JsonDictionary *curr = NULL;
        json::event_t ev;
        cvector<JsonDictionary> stack;

        node_t node;

        while ((res = p->read_next(&ev)) == STATUS_OK)
        {
            switch (ev.type)
            {
                // Start of new object
                case json::JE_OBJECT_START:
                    if (curr == NULL)
                    {
                        curr = this;
                        break;
                    }

                    if (!stack.push(curr))
                        return STATUS_NO_MEM;

                    // Add current dictionary to stack
                    if ((node.pChild = new JsonDictionary()) == NULL)
                        return STATUS_NO_MEM;

                    if ((res = curr->add_node(&node)) != STATUS_OK)
                    {
                        delete node.pChild;
                        return res;
                    }

                    curr = node.pChild;
                    node.pChild = NULL;
                    break;

                // End of current object
                case json::JE_OBJECT_END:
                    if (!stack.pop(&curr))
                        curr = NULL;
                    else if (curr == NULL)
                        return STATUS_BAD_STATE;
                    break;

                // JSON Property key
                case json::JE_PROPERTY:
                    if (curr == NULL)
                        return STATUS_BAD_STATE;
                    if (!node.sKey.set(&ev.sValue))
                        return STATUS_NO_MEM;
                    break;

                // JSON string value
                case json::JE_STRING:
                    if (curr == NULL)
                        return STATUS_BAD_STATE;
                    if (!node.sValue.set(&ev.sValue))
                        return STATUS_NO_MEM;

                    node.pChild = NULL;
                    if ((res = curr->add_node(&node)) != STATUS_OK)
                        return res;
                    break;

                // Other values are invalid
                default:
                    return STATUS_BAD_FORMAT;
            }
        }

        // Check final state
        if (res != STATUS_EOF)
            return res;
        else if ((stack.size() > 0) || (curr != NULL))
            return STATUS_BAD_STATE;

        return STATUS_OK;
    }

    JsonDictionary::node_t *JsonDictionary::find_node(const LSPString *key)
    {
        // Perform binary search
        ssize_t first = 0, last = vNodes.size()-1;
        while (first <= last)
        {
            ssize_t curr = (first + last) >> 1;
            node_t *node = vNodes.at(curr);
            int cmp = node->sKey.compare_to(key);

            if (cmp > 0)
                last    = curr - 1;
            else if (cmp < 0)
                first   = curr + 1;
            else
                return node;
        }

        return NULL;
    }

    status_t JsonDictionary::lookup(const LSPString *key, LSPString *value)
    {
        if (key == NULL)
            return STATUS_INVALID_VALUE;

        node_t *node;
        JsonDictionary *curr = this;
        size_t last = 0;

        // Need to lookup sub-nodes?
        LSPString id;
        while (true)
        {
            // Is there a path element?
            ssize_t idx = key->index_of(last, '.');
            if (idx <= 0)
                break;

            // Get a sub-string
            if (!id.set(key, last, idx))
                return STATUS_NO_MEM;

            // Try to find node
            node = curr->find_node(&id);
            if ((node == NULL) || (node->pChild == NULL))
                return STATUS_NOT_FOUND;

            last = idx+1;
            curr = node->pChild;
        }

        // Find last element
        if (last != 0)
        {
            if (!id.set(key, last))
                return STATUS_NO_MEM;
            node = curr->find_node(&id);
        }
        else
            node = curr->find_node(key);

        // Analyze node
        if ((node == NULL) || (node->pChild != NULL))
            return STATUS_NOT_FOUND;
        if ((value != NULL) && (!value->set(&node->sValue)))
            return STATUS_NO_MEM;

        return STATUS_OK;
    }

    status_t JsonDictionary::lookup(const LSPString *key, IDictionary **value)
    {
        if (key == NULL)
            return STATUS_INVALID_VALUE;

        node_t *node;
        JsonDictionary *curr = this;
        size_t last = 0;

        // Need to lookup sub-nodes?
        LSPString id;
        while (true)
        {
            // Is there a path element?
            ssize_t idx = key->index_of(last, '.');
            if (idx <= 0)
                break;

            // Get a sub-string
            if (!id.set(key, last, idx))
                return STATUS_NO_MEM;

            // Try to find node
            node = curr->find_node(&id);
            if ((node == NULL) || (node->pChild == NULL))
                return STATUS_NOT_FOUND;

            last = idx+1;
            curr = node->pChild;
        }

        // Find last element
        if (last != 0)
        {
            if (!id.set(key, last))
                return STATUS_NO_MEM;
            node = curr->find_node(&id);
        }
        else
            node = curr->find_node(key);

        // Analyze node
        if ((node == NULL) || (node->pChild == NULL))
            return STATUS_NOT_FOUND;
        if (value != NULL)
            *value = node->pChild;

        return STATUS_OK;
    }

    status_t JsonDictionary::get_value(size_t index, LSPString *key, LSPString *value)
    {
        node_t *node = vNodes.get(index);
        if ((node == NULL) || (node->pChild != NULL))
            return STATUS_NOT_FOUND;

        if ((key != NULL) && (!key->set(&node->sKey)))
            return STATUS_NO_MEM;

        if ((value != NULL) && (!value->set(&node->sValue)))
            return STATUS_NO_MEM;

        return STATUS_OK;
    }

    status_t JsonDictionary::get_child(size_t index, LSPString *key, IDictionary **dict)
    {
        node_t *node = vNodes.get(index);
        if ((node == NULL) || (node->pChild == NULL))
            return STATUS_NOT_FOUND;

        if ((key != NULL) && (!key->set(&node->sKey)))
            return STATUS_NO_MEM;

        if (dict != NULL)
            *dict = node->pChild;

        return STATUS_OK;
    }

    size_t JsonDictionary::size()
    {
        return vNodes.size();
    }

} /* namespace lsp */
