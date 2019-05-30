/*
 * KVTStorage.cpp
 *
 *  Created on: 30 мая 2019 г.
 *      Author: sadko
 */

#include <core/KVTStorage.h>
#include <core/stdlib/string.h>

namespace lsp
{
    KVTListener::KVTListener()
    {
    }

    KVTListener::~KVTListener()
    {
    }

    void KVTListener::attached(KVTStorage *storage)
    {
    }

    void KVTListener::detached(KVTStorage *storage)
    {
    }

    void KVTListener::created(const char *id, const kvt_param_t *param)
    {
    }

    void KVTListener::rejected(const char *id, const kvt_param_t *rej, const kvt_param_t *curr)
    {
    }

    void KVTListener::changed(const char *id, const kvt_param_t *oval, const kvt_param_t *nval)
    {
    }

    void KVTListener::removed(const char *id, const kvt_param_t *param)
    {
    }

    void KVTListener::access(const char *id, const kvt_param_t *param)
    {
    }

    void KVTListener::missed(const char *id)
    {
    }

    void KVTListener::collected(const char *id, const kvt_param_t *param)
    {
    }


    
    KVTStorage::KVTStorage(char separator)
    {
        cSeparator  = separator;

        init_node(&sRoot, NULL, 0);
        ++sRoot.refs;

        sValid.next         = NULL;
        sValid.prev         = NULL;
        sDirty.next         = NULL;
        sDirty.prev         = NULL;
        sGarbage.next       = NULL;
        sGarbage.prev       = NULL;
        pTrash              = NULL;
    }
    
    KVTStorage::~KVTStorage()
    {
        unbind_all();
        destroy();
    }

    void KVTStorage::destroy()
    {
        destroy_nodes(&sGarbage);
        destroy_nodes(&sValid);
        unbind_all();

        sRoot.id            = NULL;
        sRoot.idlen         = 0;
        sRoot.parent        = NULL;
        sRoot.refs          = 1;
        sRoot.param         = NULL;
        sRoot.gc.next       = NULL;
        sRoot.gc.prev       = NULL;
        sRoot.gc.node       = NULL;
        sRoot.mod.next      = NULL;
        sRoot.mod.prev      = NULL;
        sRoot.mod.node      = NULL;
        sRoot.children      = NULL;
        sRoot.nchildren     = NULL;
        sRoot.capacity      = 0;
    }

    void KVTStorage::init_node(kvt_node_t *node, const char *name, size_t len)
    {
        node->id            = reinterpret_cast<char *>(&node[1]);
        node->idlen         = len;
        node->parent        = NULL;
        node->refs          = 0;
        node->param         = NULL;
        node->gc.next       = NULL;
        node->gc.prev       = NULL;
        node->gc.node       = node;
        node->mod.next      = NULL;
        node->mod.prev      = NULL;
        node->mod.node      = node;
        node->children      = NULL;
        node->nchildren     = 0;
        node->capacity      = 0;

        // Copy name
        if (len > 0)
            ::memcpy(node->id, name, len);
        node->id[len]       = '\0';
    }

    KVTStorage::kvt_node_t *KVTStorage::allocate_node(const char *name, size_t len)
    {
        size_t to_alloc     = ALIGN_SIZE(sizeof(kvt_node_t) + len + 1, DEFAULT_ALIGN);
        kvt_node_t *node    = reinterpret_cast<kvt_node_t *>(::malloc(to_alloc));
        if (node != NULL)
        {
            init_node(node, name, len);
            link_list(&sGarbage, &node->gc);    // By default, add to garbage
        }

        return node;
    }

    void KVTStorage::link_list(kvt_link_t *root, kvt_link_t *item)
    {
        item->next          = root->next;
        item->prev          = root;
        if (root->next != NULL)
            root->next->prev    = item;
        root->next          = item;
    }

    void KVTStorage::unlink_list(kvt_link_t *item)
    {
        if (item->prev != NULL)
            item->prev->next    = item->next;
        if (item->next != NULL)
            item->next->prev    = item->prev;
        item->next      = NULL;
        item->prev      = NULL;
    }

    void KVTStorage::mark_dirty(kvt_node_t *node)
    {
        if (node->mod.prev != NULL)
            return;

        // Link to dirty list
        node->mod.next      = sDirty.next;
        node->mod.prev      = &sDirty;
        if (sDirty.next != NULL)
            sDirty.next->prev   = node;
        sDirty.next         = node;
    }

    void KVTStorage::mark_clean(kvt_node_t *node)
    {
        if (node->mod.prev == NULL)
            return;

        // Unlink from dirty list
        node->mod.prev->next    = node->mod.next;
        if (node->mod.next != NULL)
            node->mod.next->prev    = node->mod.prev;
    }

    void KVTStorage::add_reference(kvt_node_t *node, ssize_t value)
    {
        if (node->refs > 0)
        {
            node->refs += value;
            if (node->refs > 0)
                return;

            // Move to garbage
            unlink_list(&node->gc);
            link_list(&sGarbage, &node->gc);
        }
        else
        {
            node->refs += value;
            if (node->refs <= 0)
                return;

            // Move to valid
            unlink_list(&node->gc);
            link_list(&sValid, &node->gc);
        }
    }

    void KVTStorage::gc_parameter(const char *path, kvt_param_t *param, size_t flags)
    {
        // Notify all listeners that the parameter has been garbage-collected
        if ((path != NULL) && (!(flags & KVT_SILENT)))
        {
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                KVTListener *listener = vListeners.at(i);
                if (listener != NULL)
                    listener->collected(path, param);
            }
        }

        // Destroy extra data
        if (param->type == KVT_STRING)
        {
            if (param->str != NULL)
                ::free(param->str);
            param->u64      = 0;
        }
        else if (param->type == KVT_BLOB)
        {
            if (param->blob.ctype != NULL)
            {
                ::free(param->blob.ctype);
                param->blob.ctype   = NULL;
            }
            if (param->blob.data != NULL)
            {
                ::free(param->blob.data);
                param->blob.data    = NULL;
            }
            param->blob.size    = 0;
        }
        else
            param->u64      = 0;

        param->type         = KVT_ANY;
    }

    char *KVTStorage::build_path(char **path, size_t *capacity, kvt_node_t *node)
    {
        // Estimate number of bytes required
        size_t bytes = 1;
        for (kvt_node_t *n = node; n != &sRoot; n = n->parent)
            bytes   += n->idlen + 1;

        char *dst   = *path;
        size_t rcap = (bytes + 0x1f) & (~size_t(0x1f));
        if (rcap > *capacity)
        {
            dst = reinterpret_cast<char *>(::realloc(*path, rcap));
            if (dst == NULL)
                return NULL;
            *capacity       = rcap;
            *path           = dst;
        }

        dst         = &dst[bytes];
        *(--dst)    = '\0';
        for (kvt_node_t *n = node; n != &sRoot; n = n->parent)
        {
            dst        -= n->idlen;
            ::memcpy(dst, n->id, n->idlen);
            *(--dst)   = cSeparator;
        }

        return dst;
    }

    KVTStorage::kvt_node_t *KVTStorage::create_node(kvt_node_t *base, const char *name, size_t len)
    {
        kvt_node_t *node;
        ssize_t first = 0, last = base->nchildren-1, middle = 0;

        // Seek for existing node
        while (first <= last)
        {
            middle              = (first + last) >> 1;
            node                = base->children[middle];

            // Compare strings
            ssize_t cmp         = len - node->idlen;
            if (cmp == 0)
                cmp                 = ::memcmp(name, node->id, len);

            // Check result
            if (cmp > 0)
                last    = middle - 1;
            else if (cmp < 0)
                first   = middle + 1;
            else
            {
                // Node does exist?
                add_reference(node, 1);
                return node;
            }
        }

        // Create new node and add to the tree
        node        = allocate_node(name, len);
        if (node == NULL)
            return NULL;

        // Link node to parent
        node->parent    = base;

        // Add to list of children
        if (base->nchildren >= base->capacity)
        {
            size_t ncap         = base->capacity + (base->capacity >> 1);
            if (ncap <= 0)
                ncap                = 0x10;
            kvt_node_t **rmem   = reinterpret_cast<kvt_node_t **>(::realloc(base->children, ncap * sizeof(kvt_node_t *)));
            if (rmem == NULL)
                return NULL;
            base->children      = rmem;
            base->capacity      = ncap;
        }

        // Insert record to the 'middle' position
        ::memmove(&base->children[middle + 1], &base->children[middle], sizeof(kvt_node_t *) * (base->nchildren - middle));
        ++base->nchildren;

        // Return node
        return node;
    }

    KVTStorage::kvt_node_t *KVTStorage::get_node(kvt_node_t *base, const char *name, size_t len)
    {
        kvt_node_t *node        = NULL;
        ssize_t first = 0, last = base->nchildren-1;

        // Seek for existing node
        while (first <= last)
        {
            ssize_t middle      = (first + last) >> 1;
            node                = base->children[middle];

            // Compare strings
            ssize_t cmp         = len - node->idlen;
            if (cmp == 0)
                cmp                 = ::memcmp(name, node->id, len);

            // Check result
            if (cmp > 0)
                last    = middle - 1;
            else if (cmp < 0)
                first   = middle + 1;
            else
                return node;
        }

        return NULL;
    }

    KVTStorage::kvt_gcparam_t *KVTStorage::copy_parameter(const kvt_param_t *src, size_t flags)
    {
        kvt_gcparam_t *gcp  = reinterpret_cast<kvt_gcparam_t *>(::malloc(sizeof(kvt_gcparam_t)));
        gcp->next           = NULL;
        gcp->node           = NULL;

        kvt_param_t *dst    = gcp;
        *dst                = *src;

        if (flags & KVT_DELEGATE)
            return gcp;

        // Need to override pointers
        if (src->type == KVT_STRING)
        {
            if (src->str != NULL)
            {
                if (!(dst->str = ::strdup(src->str)))
                {
                    ::free(gcp);
                    return NULL;
                }
            }
        }
        else if (src->type == KVT_BLOB)
        {
            if (src->blob.ctype != NULL)
            {
                if (!(dst->blob.ctype = ::strdup(src->blob.ctype)))
                {
                    ::free(gcp);
                    return false;
                }
            }
            if (src->blob.data != NULL)
            {
                if (!(dst->blob.data = ::malloc(src->blob.size)))
                {
                    if (dst->blob.ctype != NULL)
                        ::free(dst->blob.ctype);
                    ::free(gcp);
                    return false;
                }
                ::memcpy(dst->blob.data, src->blob.data, src->blob.size);
            }
        }

        return gcp;
    }

    status_t KVTStorage::commit_parameter(const char *name, kvt_node_t *node, const kvt_param_t *value, size_t flags)
    {
        kvt_gcparam_t *copy, *curr = node->param;

        // There is no current parameter?
        if (curr == NULL)
        {
            // Copy parameter
            if (!(copy = copy_parameter(value, flags)))
                return STATUS_NO_MEM;

            if (!(flags & KVT_SILENT))
            {
                // Mark dirty and notify listeners
                mark_dirty(node);
                for (size_t i=0, n=vListeners.size(); i<n; ++i)
                {
                    KVTListener *listener = vListeners.at(i);
                    if (listener != NULL)
                        listener->created(name, copy);
                }
            }

            // Store pointer to the copy
            node->param     = copy;
            add_reference(node, 1);     // Increment the number of references because we linked an existing parameter

            return STATUS_OK;
        }

        // The node already contains parameter, need to do some stuff for replacing it
        // Do we need to keep old value?
        if (flags & KVT_KEEP)
        {
            // Notify all listeners that parameter has been rejected
            if (!(flags & KVT_SILENT))
            {
                for (size_t i=0, n=vListeners.size(); i<n; ++i)
                {
                    KVTListener *listener = vListeners.at(i);
                    if (listener != NULL)
                        listener->rejected(name, value, curr);
                }
            }
            return STATUS_ALREADY_EXISTS;
        }

        // Copy parameter
        if (!(copy = copy_parameter(value, flags)))
            return STATUS_NO_MEM;

        // Add previous value to trash, replace with new parameter
        curr->next          = pTrash;
        pTrash              = curr;
        node->param         = copy;

        if (!(flags & KVT_SILENT))
        {
            // Mark dirty and notify listeners
            mark_dirty(node);
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                KVTListener *listener = vListeners.at(i);
                if (listener != NULL)
                    listener->changed(name, curr, copy);
            }
        }

        return STATUS_OK;
    }

    void KVTStorage::reduce_branches(kvt_node_t *node)
    {
        while ((node != &sRoot) && (!(node->flags & NF_PARAM)))
        {
            mark_removed(node);
            node    = node->parent;
        }
    }

    status_t KVTStorage::put(const char *name, const kvt_param_t *value, size_t flags)
    {
        if ((name == NULL) || (value == NULL))
            return STATUS_BAD_ARGUMENTS;

        const char *path    = name;
        if ((value->type == KVT_ANY) || (*(path++) != cSeparator))
            return STATUS_INVALID_VALUE;

        kvt_node_t *curr = &sRoot;
        add_reference(curr, 1);
        status_t res;

        while (true)
        {
            const char *item = ::strchr(path, cSeparator);
            if (item != NULL) // It is a branch
            {
                // Estimate the length of the name
                size_t len  = item - name;
                if (!len) // Do not allow empty names
                    return STATUS_BAD_ARGUMENTS;
                if (!(curr = create_node(curr, path, len)))
                    return STATUS_NO_MEM;
            }
            else // It is a leaf
            {
                size_t len  = ::strlen(path);
                if (!len) // Do not allow empty names
                    return STATUS_BAD_ARGUMENTS;
                if (!(curr = create_node(curr, path, len)))
                    return STATUS_NO_MEM;

                // Commit the parameter
                return commit_parameter(name, curr, value, flags);
            }

            // Point to next after separator character
            path    = item + 1;
        }
    }

    status_t KVTStorage::walk_node(kvt_node_t **out, const char *name, size_t flags)
    {
        const char *path    = name;
        if (*(path++) != cSeparator)
            return STATUS_INVALID_VALUE;

        kvt_node_t *curr    = &sRoot;
        if (*path == '\0')
        {
            add_reference(curr, 1);
            *out    = curr;
            return STATUS_OK;
        }

        while (true)
        {
            const char *item = ::strchr(path, cSeparator);
            if (item != NULL) // It is a branch
            {
                // Estimate the length of the name
                size_t len  = item - name;
                if (!len) // Do not allow empty names
                    return STATUS_BAD_ARGUMENTS;

                curr = get_node(curr, path, len);
                if ((curr == NULL) || (curr->refs <= 0))
                {
                    // Notify listeners and return error
                    if (!(flags & KVT_SILENT))
                    {
                        for (size_t i=0, n=vListeners.size(); i<n; ++i)
                        {
                            KVTListener *listener = vListeners.at(i);
                            if (listener != NULL)
                                listener->missed(name);
                        }
                    }
                    return STATUS_NOT_FOUND;
                }
            }
            else // It is a leaf
            {
                size_t len  = ::strlen(path);
                if (!len) // Do not allow empty names
                    return STATUS_BAD_ARGUMENTS;

                curr = get_node(curr, path, len);
                if ((curr == NULL) || (curr->refs <= 0))
                {
                    // Notify listeners and return error
                    if (!(flags & KVT_SILENT))
                    {
                        for (size_t i=0, n=vListeners.size(); i<n; ++i)
                        {
                            KVTListener *listener = vListeners.at(i);
                            if (listener != NULL)
                                listener->missed(name);
                        }
                    }
                    return STATUS_NOT_FOUND;
                }

                *out    = curr;
                return STATUS_OK;
            }

            // Point to next after separator character
            path    = item + 1;
        }
    }

    status_t KVTStorage::get(const char *name, const kvt_param_t **value, kvt_param_type_t type)
    {
        if (name == NULL)
            return STATUS_BAD_ARGUMENTS;

        // Find the leaf node
        kvt_node_t *node = NULL;
        status_t res = walk_node(&node, name, 0);
        if (res != STATUS_OK)
            return res;

        kvt_gcparam_t *param = node->param;
        if (param == NULL)
        {
            // Notify listeners
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                KVTListener *listener = vListeners.at(i);
                if (listener != NULL)
                    listener->missed(name);
            }
            return STATUS_NOT_FOUND;
        }

        if ((type != KVT_ANY) && (type != param->type))
            return STATUS_BAD_TYPE;

        // Notify listeners
        for (size_t i=0, n=vListeners.size(); i<n; ++i)
        {
            KVTListener *listener = vListeners.at(i);
            if (listener != NULL)
                listener->access(name, param);
        }

        // All seems to be OK
        if (value != NULL)
            *value  = param;
        return STATUS_OK;
    }

    status_t KVTStorage::remove(const char *name, const kvt_param_t **value, kvt_param_type_t type)
    {
        if (name == NULL)
            return STATUS_BAD_ARGUMENTS;

        // Find the leaf node
        kvt_node_t *node = NULL;
        status_t res = walk_node(&node, name, 0);
        if (res != STATUS_OK)
            return res;

        kvt_gcparam_t *param = node->param;
        if (param == NULL)
        {
            // Notify listeners
            for (size_t i=0, n=vListeners.size(); i<n; ++i)
            {
                KVTListener *listener = vListeners.at(i);
                if (listener != NULL)
                    listener->missed(name);
            }
            return STATUS_NOT_FOUND;
        }

        if ((type != KVT_ANY) && (type != param->type))
            return STATUS_BAD_TYPE;

        // Add parameter to trash
        param->next         = pTrash;
        pTrash              = param;
        node->param         = NULL;
        add_reference(node, -1);

        for (size_t i=0, n=vListeners.size(); i<n; ++i)
        {
            KVTListener *listener = vListeners.at(i);
            if (listener != NULL)
                listener->removed(name, param);
        }

        // All seems to be OK
        if (value != NULL)
            *value  = param;
        return STATUS_OK;
    }

    status_t KVTStorage::remove_branch(const char *name)
    {
        if (name == NULL)
            return STATUS_BAD_ARGUMENTS;

        // Find the branch node
        kvt_node_t *node = NULL;
        status_t res = walk_node(&node, name, KVT_SILENT);
        if (res != STATUS_OK)
            return res;

        cvector<kvt_node_t> tasks, removal;
        if (!tasks.add(node))
            return STATUS_NO_MEM;

        // Generate list of nodes for removal
        while (tasks.size() > 0)
        {
            if (!tasks.pop(&node))
                return STATUS_UNKNOWN_ERR;
            // Do we need to process this node?
            if ((node != &sRoot) && (!(node->flags & NF_REMOVED)))
            {
                if (!removal.add(node))
                    return STATUS_NO_MEM;
            }

            // Generate tasks for recursive search
            kvt_node_t **child = node->children.get_array();
            for (size_t i=0, n=node->children.size(); i<n; ++i)
            {
                node = *(child++);
                if (node->flags & NF_REMOVED)
                    continue;
                if (!tasks.add(node))
                    return STATUS_NO_MEM;
            }
        }

        // Perform removal in direct order (will be stored in reverse order to the list)
        kvt_node_t **nodes  = removal.get_array();
        size_t n            = removal.size();
        for (size_t i=0; i<n; ++i)
        {
            node    = *(nodes++);
            mark_removed(node);
        }

        // Notify in reverse order
        char *str = NULL;
        size_t capacity = 0;

        for (size_t i=0; i<n; ++i)
        {
            node    = *(--nodes);
            if (!(node->flags & NF_PARAM))
                continue;

            // Notify listeners
            char *path = build_path(&str, &capacity, node);
            if (path != NULL)
            {
                for (size_t i=0, n=vListeners.size(); i<n; ++i)
                {
                    KVTListener *listener = vListeners.at(i);
                    if (listener != NULL)
                        listener->removed(path, &node->param);
                }
            }
        }

        // Free string buffer
        if (str != NULL)
            ::free(str);

        return STATUS_OK;
    }

    status_t KVTStorage::bind(KVTListener *listener)
    {
        if (vListeners.index_of(listener) >= 0)
            return STATUS_ALREADY_BOUND;
        if (!vListeners.add(listener))
            return STATUS_NO_MEM;

        listener->attached(this);

        return STATUS_OK;
    }

    status_t KVTStorage::unbind(KVTListener *listener)
    {
        if (!vListeners.remove(listener))
            return STATUS_NOT_BOUND;

        listener->detached(this);

        return STATUS_OK;
    }

    status_t KVTStorage::is_bound(KVTListener *listener)
    {
        return vListeners.index_of(listener) >= 0;
    }

    status_t KVTStorage::unbind_all()
    {
        cvector<KVTListener> old;
        vListeners.swap_data(&old);

        for (size_t i=0, n=old.size(); i<n; ++i)
        {
            KVTListener *listener = old.at(i);
            if (listener != NULL)
                listener->detached(this);
        }

        return STATUS_OK;
    }

    status_t KVTStorage::put(const char *name, uint32_t value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_UINT32;
        param.u32   = value;
        return put(name, &param, flags | KVT_DELEGATE);
    }

    status_t KVTStorage::put(const char *name, int32_t value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_INT32;
        param.i32   = value;
        return put(name, &param, flags | KVT_DELEGATE);
    }

    status_t KVTStorage::put(const char *name, uint64_t value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_UINT64;
        param.u64   = value;
        return put(name, &param, flags | KVT_DELEGATE);
    }

    status_t KVTStorage::put(const char *name, int64_t value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_INT64;
        param.i64   = value;
        return put(name, &param, flags | KVT_DELEGATE);
    }

    status_t KVTStorage::put(const char *name, float value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_FLOAT32;
        param.f32   = value;
        return put(name, &param, flags | KVT_DELEGATE);
    }

    status_t KVTStorage::put(const char *name, double value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_FLOAT64;
        param.f64   = value;
        return put(name, &param, flags | KVT_DELEGATE);
    }

    status_t KVTStorage::put(const char *name, const char *value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_STRING;
        param.str   = const_cast<char *>(value);
        return put(name, &param, flags);
    }

    status_t KVTStorage::put(const char *name, const kvt_blob_t *value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_STRING;
        param.blob  = *value;
        return put(name, &param, flags);
    }

    status_t KVTStorage::get(const char *name, uint32_t *value)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_UINT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->u32;
        return res;
    }

    status_t KVTStorage::get(const char *name, int32_t *value)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_INT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->i32;
        return res;
    }

    status_t KVTStorage::get(const char *name, uint64_t *value)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_UINT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->u64;
        return res;
    }

    status_t KVTStorage::get(const char *name, int64_t *value)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_INT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->i64;
        return res;
    }

    status_t KVTStorage::get(const char *name, float *value)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_FLOAT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->f32;
        return res;
    }

    status_t KVTStorage::get(const char *name, double *value)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_FLOAT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->f64;
        return res;
    }

    status_t KVTStorage::get(const char *name, const char **value)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_STRING);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->str;
        return res;
    }

    status_t KVTStorage::get(const char *name, const kvt_blob_t **value)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_BLOB);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = &param->blob;
        return res;
    }

    status_t KVTStorage::remove(const char *name, uint32_t *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(name, &param, KVT_UINT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->u32;
        return res;
    }

    status_t KVTStorage::remove(const char *name, int32_t *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(name, &param, KVT_INT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->i32;
        return res;
    }

    status_t KVTStorage::remove(const char *name, uint64_t *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(name, &param, KVT_UINT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->u64;
        return res;
    }

    status_t KVTStorage::remove(const char *name, int64_t *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(name, &param, KVT_INT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->i64;
        return res;
    }

    status_t KVTStorage::remove(const char *name, float *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(name, &param, KVT_FLOAT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->f32;
        return res;
    }

    status_t KVTStorage::remove(const char *name, double *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(name, &param, KVT_FLOAT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->f64;
        return res;
    }

    status_t KVTStorage::remove(const char *name, const char **value)
    {
        const kvt_param_t *param;
        status_t res        = remove(name, &param, KVT_UINT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->str;
        return res;
    }

    status_t KVTStorage::remove(const char *name, const kvt_blob_t **value)
    {
        const kvt_param_t *param;
        status_t res        = remove(name, &param, KVT_UINT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = &param->blob;
        return res;
    }

    void KVTStorage::destroy_node(const char *path, kvt_node_t *node)
    {
        // First, perform garbage-collection of parameter
        gc_parameter(path, &node->param, 0);

        // Now, remove node from lists
        unlink_list(&node->dirty);
        unlink_list(&node->gc);

        // Unlink node from parents
        kvt_node_t *parent = node->parent;
        if (parent != NULL)
        {
            parent->children.remove(node);
            node->parent    = NULL;
        }

        // Unlink node from children
        kvt_node_t **cnode  = node->children.get_array();
        for (size_t i=0, n=node->children.)
    }

    void destroy_nodes(kvt_link_t *list)
    {

    }

    void KVTStorage::gc()
    {
        char *str = NULL;
        size_t capacity = 0;

        while (true)
        {
            kvt_link_t *lnk = sGarbage.next;
            if (lnk == NULL)
                break;

            while (lnk != NULL)
            {
                kvt_link_t *next = lnk->next;
                if (next->node
            }
        }

        if (str != NULL)
            ::free(str);

        for (kvt_link_t *lnk = &sGarbage; lnk != NULL; )
        {
            kvt_link_t *next = lnk->next;

            // Collect garbage
            char *path = build_path(&str, &capacity, lnk->node);



            lnk = lnk->next;
        }
    }


} /* namespace lsp */
