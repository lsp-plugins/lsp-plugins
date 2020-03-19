/*
 * RootDictionary.cpp
 *
 *  Created on: 26 февр. 2020 г.
 *      Author: sadko
 */

#include <core/debug.h>
#include <core/resource.h>
#include <core/i18n/Dictionary.h>
#include <core/i18n/JsonDictionary.h>
#include <core/i18n/BuiltinDictionary.h>

namespace lsp
{
    
    Dictionary::Dictionary()
    {
    }
    
    Dictionary::~Dictionary()
    {
        clear();
    }

    status_t Dictionary::init_dictionary(IDictionary *d, const LSPString *path)
    {
        LSPString xp;
        if (!xp.append(path))
            return STATUS_NO_MEM;
        if (!xp.append_ascii(".json"))
            return STATUS_NO_MEM;

        status_t res = d->init(&xp);
        if (res != STATUS_OK)
        {
            if (!xp.append('5'))
                res = STATUS_NO_MEM;
            else
                res = d->init(&xp);
        }
        return res;
    }

    status_t Dictionary::load_json(IDictionary **dict, const LSPString *path)
    {
        JsonDictionary *d = new JsonDictionary();
        status_t res = init_dictionary(d, path);
        if (res == STATUS_OK)
            *dict = d;
        else
            delete d;

        return res;
    }

    status_t Dictionary::load_builtin(IDictionary **dict, const LSPString *path)
    {
        BuiltinDictionary *d = new BuiltinDictionary();
        status_t res = init_dictionary(d, path);
        if (res == STATUS_OK)
            *dict = d;
        else
            delete d;
        return res;
    }

    status_t Dictionary::create_child(IDictionary **dict, const LSPString *path)
    {
        LSPString xp;
        if (!xp.append(&sPath))
            return STATUS_NO_MEM;
        if (!xp.append('/'))
            return STATUS_NO_MEM;
        if (!xp.append(path))
            return STATUS_NO_MEM;

        Dictionary *d = new Dictionary;
        status_t res = d->init(&xp);
        if (res != STATUS_OK)
            delete d;
        else
            *dict = d;
        return res;
    }

    status_t Dictionary::load_dictionary(const LSPString *id, IDictionary **dict)
    {
        status_t res;
        LSPString res_id;

        if (sPath.starts_with_ascii(LSP_BUILTIN_PREFIX))
        {
            if (!res_id.set(&sPath, ::strlen(LSP_BUILTIN_PREFIX)))
                return STATUS_NO_MEM;
            if (!res_id.append('/'))
                return STATUS_NO_MEM;
            if (!res_id.append(id))
                return STATUS_NO_MEM;

            lsp_debug("Trying to load builtin resource %s...", res_id.get_utf8());
            res = load_builtin(dict, &res_id);
        }
        else
        {
            if (!res_id.set(&sPath))
                return STATUS_NO_MEM;
            if (!res_id.append('/'))
                return STATUS_NO_MEM;
            if (!res_id.append(id))
                return STATUS_NO_MEM;

            // Prefer builtin over external
            lsp_debug("Trying to load builtin resource %s...", res_id.get_utf8());
            res = load_builtin(dict, &res_id);
            if (res == STATUS_NOT_FOUND)
            {
                lsp_debug("Trying to load file resource %s...", res_id.get_utf8());
                res = load_json(dict, &res_id);
            }
        }

        return res;
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

        // Perform binary search of the item
        ssize_t first = 0, last = vNodes.size()-1;
        while (first <= last)
        {
            ssize_t curr = (first + last) >> 1;
            node_t *node = vNodes.at(curr);
            int cmp = node->sKey.compare_to(&id);

            if (cmp > 0)
                last    = curr - 1;
            else if (cmp < 0)
                first   = curr + 1;
            else
                return (node->pDict != NULL) ? node->pDict->lookup(&subkey, value) : STATUS_NOT_FOUND;
        }

        // Dictionary not found, try to create new one
        IDictionary *dict = NULL;
        status_t res = load_dictionary(&id, &dict);
        if (res == STATUS_NOT_FOUND)
            res = create_child(&dict, &id);

        // Add node to list of nodes
        if (res != STATUS_OK)
            return res;

        node_t *child = new node_t;
        if ((child == NULL) || (!vNodes.insert(child, first)))
        {
            delete dict;
            return STATUS_NO_MEM;
        }

        child->sKey.swap(&id);
        child->pDict        = dict;

        return dict->lookup(&subkey, value);
    }

    status_t Dictionary::lookup(const LSPString *key, IDictionary **value)
    {
        if (key == NULL)
            return STATUS_INVALID_VALUE;

        ssize_t idx = key->index_of('.');
        LSPString id, subkey;

        if (idx > 0)
        {
            if (!id.set(key, 0, idx))
                return STATUS_NO_MEM;
            if (!subkey.set(key, idx+1))
                return STATUS_NO_MEM;
        }
        else if (!id.set(key))
            return STATUS_NO_MEM;

        // Perform binary search of the dictionary
        IDictionary *dict = NULL;
        ssize_t first = 0, last = vNodes.size()-1;
        while (first <= last)
        {
            ssize_t curr = (first + last) >> 1;
            node_t *node = vNodes.at(curr);
            int cmp = node->sKey.compare_to(&id);

            if (cmp > 0)
                last    = curr - 1;
            else if (cmp < 0)
                first   = curr + 1;
            else
            {
                if (node->pDict == NULL)
                    return STATUS_NOT_FOUND;
                if ((node->bRoot) && (idx < 0))
                    return STATUS_NOT_FOUND;
                dict = node->pDict;
                break;
            }
        }

        // Dictionary object not found?
        if (dict == NULL)
        {
            // Try to load/create node
            bool root    = false;
            status_t res = load_dictionary(&id, &dict);
            if (res == STATUS_NOT_FOUND)
            {
                res         = create_child(&dict, &id);
                root        = true;
            }
            if (res != STATUS_OK)
                return res;

            // Add node to list of nodes
            node_t *child = new node_t;
            if ((child == NULL) || (!vNodes.insert(child, first)))
            {
                delete dict;
                return STATUS_NO_MEM;
            }

            child->sKey.swap(&id);
            child->pDict        = dict;
            child->bRoot        = root;

            // There is no path element defined more?
            if ((child->bRoot) && (idx < 0))
                return STATUS_NOT_FOUND;
        }

        if (idx > 0)
            return dict->lookup(&subkey, value);

        *value = dict;
        return STATUS_OK;
    }

    status_t Dictionary::get_value(size_t index, LSPString *key, LSPString *value)
    {
        node_t *node = vNodes.get(index);
        return (node == NULL) ? STATUS_NOT_FOUND : STATUS_BAD_TYPE;
    }

    status_t Dictionary::get_child(size_t index, LSPString *key, IDictionary **dict)
    {
        node_t *node = vNodes.get(index);
        if ((node == NULL) || (node->pDict == NULL))
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

    status_t Dictionary::Dictionary::init(const LSPString *path)
    {
        lsp_trace("Init dictionary path: %s" , path->get_utf8());
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
