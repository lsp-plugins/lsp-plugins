/*
 * Dictionary.cpp
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
 */

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
    }

    status_t JsonDictionary::init(const LSPString *path)
    {
        json::Parser p;
        status_t res = p.open(path, json::JSON_VERSION5);
        if (res == STATUS_OK)
            res = parse_json(&p);

        if (res != STATUS_OK)
            p.close();
        else
            res = p.close();

        return res;
    }

    status_t JsonDictionary::add_node(const node_t *node)
    {
        // TODO
        return STATUS_OK;
    }

    status_t JsonDictionary::parse_json(json::Parser *p)
    {
        status_t res;
        bool root       = false;
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
                    if (!root)
                    {
                        root    = true;
                        curr    = this;

                        if (!stack.push(curr))
                            return STATUS_NO_MEM;
                        break;
                    }
                    else if (curr == NULL)
                        return STATUS_BAD_STATE;

                    // Add current dictionary to stack
                    if (!stack.push(curr))
                        return STATUS_NO_MEM;

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
                        return STATUS_BAD_STATE;
                    if (curr == NULL)
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
        else if ((stack.size() > 0) || (curr != this))
            return STATUS_BAD_STATE;

        return STATUS_OK;
    }

    JsonDictionary::node_t *JsonDictionary::find_node(const LSPString *key)
    {
        // Perform binary search
        ssize_t first = 0, last = vNodes.size()-1, idx = 0;
        while (first <= last)
        {
            idx = (first + last) >> 1;
            node_t *node = vNodes.at(idx);
            int cmp = node->sKey.compare_to(key);

            if (cmp > 0)
                last    = first - 1;
            else if (cmp < 0)
                first   = last + 1;
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

        // Need to lookup sub-node?
        ssize_t idx = key->index_of('.');
        if (idx > 0)
        {
            LSPString id;
            if (!id.set(key, 0, idx))
                return STATUS_NO_MEM;

            node = find_node(&id);
            if ((node == NULL) || (node->pChild == NULL))
                return STATUS_NOT_FOUND;

            if (!id.set(key, idx+1))
                return STATUS_NO_MEM;

            return node->pChild->lookup(&id, value);
        }

        // No need to lookup
        node = find_node(key);
        if ((node == NULL) || (node->pChild != NULL))
            return STATUS_NOT_FOUND;

        if ((value != NULL) && (!value->set(&node->sValue)))
            return STATUS_NO_MEM;

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
