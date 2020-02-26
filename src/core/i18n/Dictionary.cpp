/*
 * RootDictionary.cpp
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
 */

#include <core/i18n/Dictionary.h>

namespace lsp
{
    
    Dictionary::Dictionary()
    {
    }
    
    Dictionary::~Dictionary()
    {
        clear();
    }

    status_t Dictionary::lookup(const LSPString *key, LSPString *value)
    {
        if (key == NULL)
            return STATUS_INVALID_VALUE;

        ssize_t idx = key->index_of('.');
        if (idx < 0)
            return STATUS_NOT_FOUND;

        LSPString id, subkey;
        if (!id.set(key, 0, idx))
            return STATUS_NO_MEM;
        if (!subkey.set(key, idx+1))
            return STATUS_NO_MEM;

        // Perform binary search
        ssize_t first = 0, last = vNodes.size()-1, idx = 0;
        while (first <= last)
        {
            idx = (first + last) >> 1;
            node_t *node = vNodes.at(idx);
            int cmp = node->sKey.compare_to(&id);

            if (cmp > 0)
                last = first - 1;
            else if (cmp < 0)
                first = last + 1;
            else
                return node->pDict->lookup(&subkey, value);
        }

        // TODO: Dictionary not found: try to create new one

        return STATUS_NOT_FOUND;
    }

    status_t Dictionary::get_value(size_t index, LSPString *key, LSPString *value)
    {
        node_t *node = vNodes.get(index);
        return (node == NULL) ? STATUS_NOT_FOUND : STATUS_BAD_TYPE;
    }

    status_t Dictionary::get_child(size_t index, LSPString *key, IDictionary **dict)
    {
        node_t *node = vNodes.get(index);
        if (node == NULL)
            return STATUS_NOT_FOUND;

        if ((key != NULL) && (!key->set(&node->sKey)))
            return STATUS_NO_MEM;

        if (dict != NULL)
            *dict = node->pDict;

        return STATUS_OK;
    }

    size_t Dictionary::size()
    {
        return vNodes.size();
    }

    status_t Dictionary::Dictionary::init(const char *path)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        LSPString xpath;
        if (!xpath.set_utf8(path))
            return STATUS_NO_MEM;
        return init(&xpath);
    }

    status_t Dictionary::Dictionary::init(const io::Path *path)
    {
        if (path == NULL)
            return STATUS_BAD_ARGUMENTS;
        return init(path->as_string());
    }

    status_t Dictionary::Dictionary::init(const LSPString *path)
    {
        return (sPath.set(path)) ? STATUS_OK : STATUS_NO_MEM;
    }

    void  Dictionary::Dictionary::clear()
    {
        for (size_t i=0, n=vNodes.size(); i<n; ++i)
        {
            node_t *node = vNodes.at(i);
            if (node == NULL)
                continue;

            if (node->pDict != NULL)
                delete node->pDict;
            delete node;
        }

        vNodes.flush();
    }

} /* namespace lsp */
