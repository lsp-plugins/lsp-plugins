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

    void KVTListener::created(KVTStorage *storage, const char *id, const kvt_param_t *param, size_t pending)
    {
    }

    void KVTListener::rejected(KVTStorage *storage, const char *id, const kvt_param_t *rej, const kvt_param_t *curr, size_t pending)
    {
    }

    void KVTListener::changed(KVTStorage *storage, const char *id, const kvt_param_t *oval, const kvt_param_t *nval, size_t pending)
    {
    }

    void KVTListener::removed(KVTStorage *storage, const char *id, const kvt_param_t *param, size_t pending)
    {
    }

    void KVTListener::access(KVTStorage *storage, const char *id, const kvt_param_t *param, size_t pending)
    {
    }

    void KVTListener::commit(KVTStorage *storage, const char *id, const kvt_param_t *param, size_t pending)
    {
    }

    void KVTListener::missed(KVTStorage *storage, const char *id)
    {
    }

    
    KVTStorage::KVTStorage(char separator)
    {
        cSeparator  = separator;

        sValid.next         = NULL;
        sValid.prev         = NULL;
        sTx.next            = NULL;
        sTx.prev            = NULL;
        sTx.node            = NULL;
        sRx.next            = NULL;
        sRx.prev            = NULL;
        sRx.node            = NULL;
        sGarbage.next       = NULL;
        sGarbage.prev       = NULL;
        sGarbage.node       = NULL;
        pTrash              = NULL;
        pIterators          = NULL;
        nNodes              = 0;
        nValues             = 0;
        nTxPending          = 0;
        nRxPending          = 0;

        init_node(&sRoot, NULL, 0);
        ++sRoot.refs;
    }
    
    KVTStorage::~KVTStorage()
    {
        destroy();
    }

    void KVTStorage::destroy()
    {
        unbind_all();

        // Destroy trash
        while (pTrash != NULL)
        {
            kvt_gcparam_t *next = pTrash->next;
            destroy_parameter(pTrash);
            pTrash      = next;
        }

        // Destroy all iterators
        while (pIterators != NULL)
        {
            KVTIterator *next   = pIterators->pGcNext;
            delete pIterators;
            pIterators          = next;
        }

        // Destroy all nodes
        kvt_link_t *link = sValid.next;
        while (link != NULL)
        {
            kvt_link_t *next = link->next;
            destroy_node(link->node);
            link        = next;
        }
        link = sGarbage.next;
        while (link != NULL)
        {
            kvt_link_t *next = link->next;
            destroy_node(link->node);
            link        = next;
        }

        // Cleanup pointers
        sRoot.id            = NULL;
        sRoot.idlen         = 0;
        sRoot.parent        = NULL;
        sRoot.refs          = 0;
        sRoot.param         = NULL;
        sRoot.gc.next       = NULL;
        sRoot.gc.prev       = NULL;
        sRoot.gc.node       = NULL;
        sRoot.tx.next      = NULL;
        sRoot.tx.prev      = NULL;
        sRoot.tx.node      = NULL;
        if (sRoot.children != NULL)
        {
            ::free(sRoot.children);
            sRoot.children      = NULL;
        }
        sRoot.nchildren     = 0;
        sRoot.capacity      = 0;

        sValid.next         = NULL;
        sValid.prev         = NULL;
        sValid.node         = NULL;
        sTx.next            = NULL;
        sTx.prev            = NULL;
        sTx.node            = NULL;
        sRx.next            = NULL;
        sRx.prev            = NULL;
        sRx.node            = NULL;
        sGarbage.next       = NULL;
        sGarbage.prev       = NULL;
        sGarbage.node       = NULL;
        pTrash              = NULL;
        pIterators          = NULL;

        nNodes              = 0;
        nValues             = 0;
        nTxPending          = 0;
        nRxPending          = 0;
    }

    status_t KVTStorage::clear()
    {
        return do_remove_branch("/", &sRoot);
    }

    void KVTStorage::init_node(kvt_node_t *node, const char *name, size_t len)
    {
        node->id            = (name != NULL) ? reinterpret_cast<char *>(&node[1]) : NULL;
        node->idlen         = len;
        node->parent        = NULL;
        node->refs          = 0;
        node->param         = NULL;
        node->pending       = 0;
        node->gc.next       = NULL;
        node->gc.prev       = NULL;
        node->gc.node       = node;
        node->tx.next       = NULL;
        node->tx.prev       = NULL;
        node->tx.node       = node;
        node->rx.next       = NULL;
        node->rx.prev       = NULL;
        node->rx.node       = node;
        node->children      = NULL;
        node->nchildren     = 0;
        node->capacity      = 0;

        // Copy name
        if (node->id != NULL)
        {
            ::memcpy(node->id, name, len);
            node->id[len]       = '\0';
        }
    }

    size_t KVTStorage::listeners() const
    {
        return vListeners.size();
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

    size_t KVTStorage::set_pending_state(kvt_node_t *node, size_t flags)
    {
        // TX state changed?
        if ((node->pending ^ flags) & KVT_TX)
        {
            // Manage TX list
            if (flags & KVT_TX) // 0 -> 1
            {
                link_list(&sTx, &node->tx);
                node->pending  |= KVT_TX;
                ++nTxPending;
            }
            else // 1 -> 0
            {
                unlink_list(&node->tx);
                node->pending  &= ~KVT_TX;
                --nTxPending;
            }
        }

        // RX state changed?
        if ((node->pending ^ flags) & KVT_RX)
        {
            // Manage TX list
            if (flags & KVT_RX) // 0 -> 1
            {
                link_list(&sRx, &node->rx);
                node->pending  |= KVT_RX;
                ++nRxPending;
            }
            else // 1 -> 0
            {
                unlink_list(&node->rx);
                node->pending  &= ~KVT_RX;
                --nRxPending;
            }
        }

        return node->pending;
    }

    KVTStorage::kvt_node_t *KVTStorage::reference_up(kvt_node_t *node)
    {
        kvt_node_t *x = node;

        do
        {
            if ((x->refs++) > 0)
                break;

            // Move to valid
            unlink_list(&x->gc);
            link_list(&sValid, &x->gc);
            ++nNodes;

            // Move to parent
            x = x->parent;
        } while (x != NULL);

        return node;
    }

    KVTStorage::kvt_node_t *KVTStorage::reference_down(kvt_node_t *node)
    {
        kvt_node_t *x = node;

        do
        {
            // Decrement number of references
            if ((--x->refs) > 0)
                break;
//            if (x->refs < 0)
//                lsp_trace("Error");

            // Move to garbage
            unlink_list(&x->gc);
            link_list(&sGarbage, &x->gc);
            --nNodes;

            // Move to parent
            x = x->parent;
        } while (x != NULL);

        return node;
    }

    void KVTStorage::notify_created(const char *id, const kvt_param_t *param, size_t pending)
    {
        for (size_t i=0, n=vListeners.size(); i<n; ++i)
        {
            KVTListener *listener = vListeners.at(i);
            if (listener != NULL)
                listener->created(this, id, param, pending);
        }
    }

    void KVTStorage::notify_rejected(const char *id, const kvt_param_t *rej, const kvt_param_t *curr, size_t pending)
    {
        for (size_t i=0, n=vListeners.size(); i<n; ++i)
        {
            KVTListener *listener = vListeners.at(i);
            if (listener != NULL)
                listener->rejected(this, id, rej, curr, pending);
        }
    }

    void KVTStorage::notify_changed(const char *id, const kvt_param_t *oval, const kvt_param_t *nval, size_t pending)
    {
        for (size_t i=0, n=vListeners.size(); i<n; ++i)
        {
            KVTListener *listener = vListeners.at(i);
            if (listener != NULL)
                listener->changed(this, id, oval, nval, pending);
        }
    }

    void KVTStorage::notify_removed(const char *id, const kvt_param_t *param, size_t pending)
    {
        for (size_t i=0, n=vListeners.size(); i<n; ++i)
        {
            KVTListener *listener = vListeners.at(i);
            if (listener != NULL)
                listener->removed(this, id, param, pending);
        }
    }

    void KVTStorage::notify_access(const char *id, const kvt_param_t *param, size_t pending)
    {
        for (size_t i=0, n=vListeners.size(); i<n; ++i)
        {
            KVTListener *listener = vListeners.at(i);
            if (listener != NULL)
                listener->access(this, id, param, pending);
        }
    }

    void KVTStorage::notify_commit(const char *id, const kvt_param_t *param, size_t pending)
    {
        for (size_t i=0, n=vListeners.size(); i<n; ++i)
        {
            KVTListener *listener = vListeners.at(i);
            if (listener != NULL)
                listener->commit(this, id, param, pending);
        }
    }

    void KVTStorage::notify_missed(const char *id)
    {
        for (size_t i=0, n=vListeners.size(); i<n; ++i)
        {
            KVTListener *listener = vListeners.at(i);
            if (listener != NULL)
                listener->missed(this, id);
        }
    }

    void KVTStorage::destroy_parameter(kvt_gcparam_t *param)
    {
        // Destroy extra data
        if (param->type == KVT_STRING)
        {
            if (param->str != NULL)
                ::free(const_cast<char *>(param->str));
            param->u64      = 0;
        }
        else if (param->type == KVT_BLOB)
        {
            if (param->blob.ctype != NULL)
            {
                ::free(const_cast<char *>(param->blob.ctype));
                param->blob.ctype   = NULL;
            }
            if (param->blob.data != NULL)
            {
                ::free(const_cast<void *>(param->blob.data));
                param->blob.data    = NULL;
            }
            param->blob.size    = 0;
        }
        else
            param->u64      = 0;

        param->type         = KVT_ANY;
        ::free(param);
    }

    char *KVTStorage::build_path(char **path, size_t *capacity, const kvt_node_t *node)
    {
        // Estimate number of bytes required
        size_t bytes = 1;
        for (const kvt_node_t *n = node; n != &sRoot; n = n->parent)
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
        for (const kvt_node_t *n = node; n != &sRoot; n = n->parent)
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
            if (cmp < 0)
                last    = middle - 1;
            else if (cmp > 0)
                first   = middle + 1;
            else // Node does exist?
                return node;
        }

        // Create new node and add to the tree
        node        = allocate_node(name, len);
        if (node == NULL)
            return NULL;

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

        // Link node to parent
        ::memmove(&base->children[first + 1], &base->children[first], sizeof(kvt_node_t *) * (base->nchildren - first));
        base->children[first]   = node;
        node->parent            = base;
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
            if (cmp < 0)
                last    = middle - 1;
            else if (cmp > 0)
                first   = middle + 1;
            else
                return node;
        }

        return NULL;
    }

    KVTStorage::kvt_gcparam_t *KVTStorage::copy_parameter(const kvt_param_t *src, size_t flags)
    {
        kvt_gcparam_t *gcp  = reinterpret_cast<kvt_gcparam_t *>(::malloc(sizeof(kvt_gcparam_t)));
        gcp->flags          = flags & (KVT_PRIVATE | KVT_TRANSIENT);
        gcp->next           = NULL;

        kvt_param_t *dst    = gcp;
        *dst = *src;

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
                    return NULL;
                }
            }
            if (src->blob.data != NULL)
            {
                if (!(dst->blob.data = ::malloc(src->blob.size)))
                {
                    if (dst->blob.ctype != NULL)
                        ::free(const_cast<char *>(dst->blob.ctype));
                    ::free(gcp);
                    return NULL;
                }
                ::memcpy(const_cast<void *>(dst->blob.data), src->blob.data, src->blob.size);
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

            // Store pointer to the copy
            size_t pending  = set_pending_state(node, node->pending | flags);
            reference_up(node);
            node->param     = copy;
            ++nValues;

            notify_created(name, copy, pending);
            return STATUS_OK;
        }

        // The node already contains parameter, need to do some stuff for replacing it
        // Do we need to keep old value?
        if (flags & KVT_KEEP)
        {
            notify_rejected(name, value, curr, node->pending);
            return STATUS_ALREADY_EXISTS;
        }

        // Copy parameter
        if (!(copy = copy_parameter(value, flags)))
            return STATUS_NO_MEM;

        // Add previous value to trash, replace with new parameter
        size_t pending      = set_pending_state(node, node->pending | flags);
        curr->next          = pTrash;
        pTrash              = curr;
        node->param         = copy;

        notify_changed(name, curr, copy, pending);
        return STATUS_OK;
    }

    status_t KVTStorage::put(const char *name, const kvt_param_t *value, size_t flags)
    {
        if ((name == NULL) || (value == NULL))
            return STATUS_BAD_ARGUMENTS;

        const char *path    = name;
        if (!validate_type(value->type))
            return STATUS_BAD_TYPE;
        else if (*(path++) != cSeparator)
            return STATUS_INVALID_VALUE;

        kvt_node_t *curr = &sRoot;

        while (true)
        {
            const char *item = ::strchr(path, cSeparator);
            if (item != NULL) // It is a branch
            {
                // Estimate the length of the name
                size_t len  = item - path;
                if (!len) // Do not allow empty names
                    return STATUS_INVALID_VALUE;
                if (!(curr = create_node(curr, path, len)))
                    return STATUS_NO_MEM;
            }
            else // It is a leaf
            {
                size_t len  = ::strlen(path);
                if (!len) // Do not allow empty names
                    return STATUS_INVALID_VALUE;
                if (!(curr = create_node(curr, path, len)))
                    return STATUS_NO_MEM;

                // Commit the parameter
                return commit_parameter(name, curr, value, flags);
            }

            // Point to next after separator character
            path    = item + 1;
        }
    }

    status_t KVTStorage::walk_node(kvt_node_t **out, const char *name)
    {
        const char *path    = name;
        if (*(path++) != cSeparator)
            return STATUS_INVALID_VALUE;

        kvt_node_t *curr    = &sRoot;
        if (*path == '\0')
        {
            *out    = curr;
            return STATUS_OK;
        }

        while (true)
        {
            const char *item = ::strchr(path, cSeparator);
            if (item != NULL) // It is a branch
            {
                // Estimate the length of the name
                size_t len  = item - path;
                if (!len) // Do not allow empty names
                    return STATUS_INVALID_VALUE;

                curr = get_node(curr, path, len);
                if ((curr == NULL) || (curr->refs <= 0))
                    return STATUS_NOT_FOUND;
            }
            else // It is a leaf
            {
                size_t len  = ::strlen(path);
                if (!len) // Do not allow empty names
                    return STATUS_INVALID_VALUE;

                curr = get_node(curr, path, len);
                if ((curr == NULL) || (curr->refs <= 0))
                    return STATUS_NOT_FOUND;

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
        status_t res = walk_node(&node, name);
        if (res != STATUS_OK)
        {
            if (res != STATUS_NOT_FOUND)
                return res;

            notify_missed(name);
            return res;
        }
        else if (node == &sRoot)
            return STATUS_INVALID_VALUE;

        kvt_gcparam_t *param = node->param;
        if (param == NULL)
        {
            // Notify listeners
            notify_missed(name);
            return STATUS_NOT_FOUND;
        }

        if ((type != KVT_ANY) && (type != param->type))
            return STATUS_BAD_TYPE;

        // All seems to be OK
        if (value != NULL)
        {
            *value  = param;
            notify_access(name, param, node->pending);
        }
        return STATUS_OK;
    }

    bool KVTStorage::exists(const char *name, kvt_param_type_t type)
    {
        if (name == NULL)
            return false;

        // Find the leaf node
        kvt_node_t *node = NULL;
        status_t res = walk_node(&node, name);
        if (res != STATUS_OK)
        {
            if (res == STATUS_NOT_FOUND)
                notify_missed(name);
            return false;
        }
        if (node == &sRoot)
            return false;

        kvt_gcparam_t *param = node->param;
        if (param == NULL)
        {
            notify_missed(name);
            return false;
        }

        return ((type == KVT_ANY) || (type == param->type));
    }

    status_t KVTStorage::do_remove_node(const char *name, kvt_node_t *node, const kvt_param_t **value, kvt_param_type_t type)
    {
        kvt_gcparam_t *param = node->param;
        if (param == NULL)
        {
            notify_missed(name);
            return STATUS_NOT_FOUND;
        }

        if ((type != KVT_ANY) && (type != param->type))
            return STATUS_BAD_TYPE;

        // Add parameter to trash
        size_t pending      = node->pending;
        set_pending_state(node, 0);
        reference_down(node);
        param->next         = pTrash;
        pTrash              = param;
        node->param         = NULL;
        --nValues;

        notify_removed(name, param, pending);

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
        status_t res = walk_node(&node, name);
        if (res != STATUS_OK)
        {
            if (res == STATUS_NOT_FOUND)
                notify_missed(name);
            return res;
        }
        else if (node == &sRoot)
            return STATUS_INVALID_VALUE;

        return do_remove_node(name, node, value, type);
    }

    status_t KVTStorage::do_touch(const char *name, kvt_node_t *node, size_t flags)
    {
        // Parameter does exist?
        kvt_gcparam_t *param = node->param;
        if (param == NULL)
        {
            notify_missed(name);
            return STATUS_NOT_FOUND;
        }

        // Skip private parameters if KVT_PRIVATE flag was not set
        if ((param->flags & KVT_PRIVATE) && (!(flags & KVT_PRIVATE)))
            return STATUS_OK;

        // Add parameter to trash
        size_t op = node->pending;
        size_t np = set_pending_state(node, op | flags);

        if ((op ^ np) & KVT_TX) // TX flag has set?
            notify_changed(name, param, param, KVT_TX);
        if ((op ^ np) & KVT_RX) // RX flag has set?
            notify_changed(name, param, param, KVT_RX);

        return STATUS_OK;
    }

    status_t KVTStorage::do_commit(const char *name, kvt_node_t *node, size_t flags)
    {
        // Parameter does exist?
        kvt_gcparam_t *param = node->param;
        if (param == NULL)
        {
            notify_missed(name);
            return STATUS_NOT_FOUND;
        }

        // Add parameter to trash
        size_t op = node->pending;
        size_t np = set_pending_state(node, op & (~flags));

        if ((op ^ np) & KVT_TX) // TX flag has been reset?
            notify_commit(name, param, KVT_TX);
        if ((op ^ np) & KVT_RX) // RX flag has been reset?
            notify_commit(name, param, KVT_RX);

        return STATUS_OK;
    }

    status_t KVTStorage::touch(const char *name, size_t flags)
    {
        if (name == NULL)
            return STATUS_BAD_ARGUMENTS;
        else if (flags == 0)
            return STATUS_OK;

        // Find the leaf node
        kvt_node_t *node = NULL;
        status_t res = walk_node(&node, name);
        if (res != STATUS_OK)
        {
            if (res == STATUS_NOT_FOUND)
                notify_missed(name);
            return res;
        }
        else if (node == &sRoot)
            return STATUS_INVALID_VALUE;

        return do_touch(name, node, flags);
    }

    status_t KVTStorage::commit(const char *name, size_t flags)
    {
        if (name == NULL)
            return STATUS_BAD_ARGUMENTS;
        else if (flags == 0)
            return STATUS_OK;

        // Find the leaf node
        kvt_node_t *node = NULL;
        status_t res = walk_node(&node, name);
        if (res != STATUS_OK)
        {
            if (res == STATUS_NOT_FOUND)
                notify_missed(name);
            return res;
        }
        else if (node == &sRoot)
            return STATUS_INVALID_VALUE;

        return do_commit(name, node, flags);
    }

    status_t KVTStorage::touch_all(size_t flags)
    {
        kvt_node_t *node;
        char *str = NULL, *path;
        size_t capacity = 0;

//        lsp_trace("kvt items: %d", int(nValues));

        for (kvt_link_t *lnk = sValid.next; lnk != NULL; lnk = lnk->next)
        {
            node        = lnk->node;
            if (node->param == NULL) // Parameter does exist?
                continue;

            // Skip private parameters if KVT_PRIVATE flag was not set
            if ((node->param->flags & KVT_PRIVATE) && (!(flags & KVT_PRIVATE)))
                continue;

            size_t op   = node->pending;
            size_t np   = set_pending_state(node, op | flags);
//            lsp_trace("%s op=0x%x, np=0x%x", node->id, int(op), int(np));

            // State has changed?
            if (op != np)
            {
                // Build path to node
                path = build_path(&str, &capacity, node);
                if (path == NULL)
                {
                    if (str != NULL)
                        ::free(str);
                    return STATUS_NO_MEM;
                }

                // TX flag changed?
                if ((op ^ np) & KVT_TX) // TX flag has been set?
                    notify_changed(path, node->param, node->param, KVT_TX);
                if ((op ^ np) & KVT_RX) // RX flag has been set?
                    notify_changed(path, node->param, node->param, KVT_RX);
            }
        }

        if (str != NULL)
            ::free(str);

        return STATUS_OK;
    }

    status_t KVTStorage::commit_all(size_t flags)
    {
        kvt_node_t *node;
        char *str = NULL, *path;
        size_t capacity = 0;

        if (flags & KVT_TX)
        {
            while (sTx.next != NULL)
            {
                node        = sTx.next->node;
                if (node->param == NULL) // Parameter does exist?
                    continue;

                size_t op   = node->pending;
                size_t np   = set_pending_state(node, op & (~KVT_TX));

                if ((op ^ np) & KVT_TX) // TRX flag has been reset?
                {
                    // Build path to node
                    path = build_path(&str, &capacity, node);
                    if (path == NULL)
                    {
                        if (str != NULL)
                            ::free(str);
                        return STATUS_NO_MEM;
                    }
                    notify_commit(path, node->param, KVT_TX);
                }
            }
        }

        if (flags & KVT_RX)
        {
            while (sRx.next != NULL)
            {
                node        = sRx.next->node;
                if (node->param == NULL) // Parameter does exist?
                    continue;

                size_t op   = node->pending;
                size_t np   = set_pending_state(node, op & (~KVT_RX));

                if ((op ^ np) & KVT_RX) // TRX flag has been reset?
                {
                    // Build path to node
                    path = build_path(&str, &capacity, node);
                    if (path == NULL)
                    {
                        if (str != NULL)
                            ::free(str);
                        return STATUS_NO_MEM;
                    }

                    notify_commit(path, node->param, KVT_RX);
                }
            }
        }

        if (str != NULL)
            ::free(str);

        return STATUS_OK;
    }

    status_t KVTStorage::do_remove_branch(const char *name, kvt_node_t *node)
    {
        kvt_node_t *child;

        cvector<kvt_node_t> tasks;
        if (!tasks.add(node))
            return STATUS_NO_MEM;

        // Generate list of nodes for removal
        char *str = NULL, *path;
        size_t capacity = 0;

        while (tasks.size() > 0)
        {
            // Get the next task
            if (!tasks.pop(&node))
            {
                if (str != NULL)
                    ::free(str);
                return STATUS_UNKNOWN_ERR;
            }

            // Does node have a parameter?
            kvt_gcparam_t *param = node->param;
            if (param != NULL)
            {
                // Add parameter to trash
                size_t pending      = node->pending;
                set_pending_state(node, 0);
                reference_down(node);
                param->next         = pTrash;
                pTrash              = param;
                node->param         = NULL;
                --nValues;

                // Build path to node
                path = build_path(&str, &capacity, node);
                if (path == NULL)
                {
                    if (str != NULL)
                        ::free(str);
                    return STATUS_NO_MEM;
                }

                // Notify listeners
                notify_removed(path, param, pending);
            }

            // Generate tasks for recursive search
            for (size_t i=0; i<node->nchildren; ++i)
            {
                child = node->children[i];
                if (child->refs <= 0)
                    continue;

                // Add child to the analysis list
                if (!tasks.push(child))
                {
                    if (str != NULL)
                        ::free(str);
                    return STATUS_NO_MEM;
                }
            }
        }

        if (str != NULL)
            ::free(str);

        return STATUS_OK;
    }

    status_t KVTStorage::remove_branch(const char *name)
    {
        if (name == NULL)
            return STATUS_BAD_ARGUMENTS;

        // Find the branch node
        kvt_node_t *node = NULL;
        status_t res = walk_node(&node, name);
        if (res != STATUS_OK)
            return res;

        return do_remove_branch(name, node);
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
        param.type  = KVT_BLOB;
        param.blob  = *value;
        return put(name, &param, flags);
    }

    status_t KVTStorage::put(const char *name, size_t size, const char *type, const void *value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_BLOB;
        param.blob.size     = size;
        param.blob.ctype    = type;
        param.blob.data     = value;
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

    status_t KVTStorage::get_dfl(const char *name, uint32_t *value, uint32_t dfl)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_UINT32);

        if (res == STATUS_NOT_FOUND)
        {
            if (value != NULL)
                *value      = dfl;
            res         = STATUS_OK;
        }
        else if ((res == STATUS_OK) && (value != NULL))
            *value      = param->u32;
        return res;
    }

    status_t KVTStorage::get_dfl(const char *name, int32_t *value, int32_t dfl)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_INT32);
        if (res == STATUS_NOT_FOUND)
        {
            if (value != NULL)
                *value      = dfl;
            res         = STATUS_OK;
        }
        else if ((res == STATUS_OK) && (value != NULL))
            *value      = param->i32;
        return res;
    }

    status_t KVTStorage::get_dfl(const char *name, uint64_t *value, uint64_t dfl)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_UINT64);
        if (res == STATUS_NOT_FOUND)
        {
            if (value != NULL)
                *value      = dfl;
            res         = STATUS_OK;
        }
        else if ((res == STATUS_OK) && (value != NULL))
            *value      = param->u64;
        return res;
    }

    status_t KVTStorage::get_dfl(const char *name, int64_t *value, int64_t dfl)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_INT64);
        if (res == STATUS_NOT_FOUND)
        {
            if (value != NULL)
                *value      = dfl;
            res         = STATUS_OK;
        }
        else if ((res == STATUS_OK) && (value != NULL))
            *value      = param->i64;
        return res;
    }

    status_t KVTStorage::get_dfl(const char *name, float *value, float dfl)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_FLOAT32);
        if (res == STATUS_NOT_FOUND)
        {
            if (value != NULL)
                *value      = dfl;
            res         = STATUS_OK;
        }
        else if ((res == STATUS_OK) && (value != NULL))
            *value      = param->f32;
        return res;
    }

    status_t KVTStorage::get_dfl(const char *name, double *value, double dfl)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_FLOAT64);
        if (res == STATUS_NOT_FOUND)
        {
            if (value != NULL)
                *value      = dfl;
            res         = STATUS_OK;
        }
        else if ((res == STATUS_OK) && (value != NULL))
            *value      = param->f64;
        return res;
    }

    status_t KVTStorage::get_dfl(const char *name, const char **value, const char *dfl)
    {
        const kvt_param_t *param;
        status_t res        = get(name, &param, KVT_STRING);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->str;
        else if ((res == STATUS_NOT_FOUND) && (value != NULL))
            *value      = dfl;
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
        status_t res        = remove(name, &param, KVT_STRING);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->str;
        return res;
    }

    status_t KVTStorage::remove(const char *name, const kvt_blob_t **value)
    {
        const kvt_param_t *param;
        status_t res        = remove(name, &param, KVT_BLOB);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = &param->blob;
        return res;
    }

    void KVTStorage::destroy_node(kvt_node_t *node)
    {
        node->id        = NULL;
        node->idlen     = 0;
        node->parent    = NULL;

        if (node->param != NULL)
        {
            destroy_parameter(node->param);
            node->param     = NULL;
        }
        node->pending   = 0;

        if (node->children != NULL)
        {
            ::free(node->children);
            node->children = NULL;
        }
        node->nchildren = 0;
        node->capacity  = 0;

        ::free(node);
    }

    status_t KVTStorage::gc()
    {
        // Part 0: Destroy all iterators
        while (pIterators != NULL)
        {
            KVTIterator *next   = pIterators->pGcNext;
            delete pIterators;
            pIterators          = next;
        }

        // Part 1: Collect all garbage parameters
        while (pTrash != NULL)
        {
            kvt_gcparam_t *next = pTrash->next;
            destroy_parameter(pTrash);
            pTrash      = next;
        }

        // Part 2: Unlink all garbage nodes from valid parents
        for (kvt_link_t *lnk = sGarbage.next; lnk != NULL; lnk = lnk->next)
        {
            kvt_node_t *node = lnk->node;
            kvt_node_t *parent = node->parent;

            if ((parent == NULL) || (parent->refs <= 0))
                continue;

            // Remove all non-valid children from list
            kvt_node_t **dst = parent->children;
            kvt_node_t **src = parent->children;
            for (size_t i=0; i<parent->nchildren; ++src)
            {
                kvt_node_t *child = *src;
                if (child->refs <= 0)
                {
                    child->parent   = NULL;
                    --parent->nchildren;
                }
                else
                {
                    if (dst < src)
                        *dst = *src;
                    ++dst;
                    ++i;
                }
            }
            
            // Remove node from parent's list
//            for (size_t i=0; i<parent->nchildren; ++i)
//            {
//                if (parent->children[i] == node)
//                {
//                    --parent->nchildren;
//                    ::memmove(&parent->children[i],
//                            &parent->children[i+1],
//                            (parent->nchildren - i) * sizeof(kvt_node_t *));
//                    break;
//                }
//            }
        }

        // Part 3: Collect garbage nodes
        while (sGarbage.next != NULL)
        {
            kvt_node_t *node = sGarbage.next->node;
            unlink_list(&node->tx);
            unlink_list(&node->rx);
            unlink_list(&node->gc);

            destroy_node(node);
        }

        return STATUS_OK;
    }

    KVTIterator *KVTStorage::enum_tx_pending()
    {
        kvt_link_t *lnk = sTx.next;
        return new KVTIterator(this, (lnk != NULL) ? lnk->node : NULL, IT_TX_PENDING);
    }

    KVTIterator *KVTStorage::enum_rx_pending()
    {
        kvt_link_t *lnk = sRx.next;
        return new KVTIterator(this, (lnk != NULL) ? lnk->node : NULL, IT_RX_PENDING);
    }

    KVTIterator *KVTStorage::enum_all()
    {
        kvt_link_t *lnk = sValid.next;
        return new KVTIterator(this, (lnk != NULL) ? lnk->node : NULL, IT_ALL);
    }

    KVTIterator *KVTStorage::enum_branch(const char *name, bool recursive)
    {
        // Find the leaf node
        kvt_node_t *node = NULL;
        status_t res = walk_node(&node, name);
        if (res != STATUS_OK)
        {
            if (res == STATUS_NOT_FOUND)
                notify_missed(name);
        }

        return new KVTIterator(this, node, (recursive) ? IT_RECURSIVE : IT_BRANCH);
    }


    KVTIterator::KVTIterator(KVTStorage *storage, KVTStorage::kvt_node_t *node, KVTStorage::iterator_mode_t mode)
    {
        sFake.id        = NULL;
        sFake.idlen     = 0;
        sFake.parent    = node;
        sFake.refs      = 0;
        sFake.children  = NULL;
        sFake.param     = NULL;
        sFake.pending   = 0;
        sFake.gc.next   = (node != NULL) ? &node->gc : NULL;
        sFake.gc.prev   = NULL;
        sFake.gc.node   = NULL;
        sFake.tx.next   = (node != NULL) ? &node->tx : NULL;
        sFake.tx.prev   = NULL;
        sFake.tx.node   = NULL;
        sFake.rx.next   = (node != NULL) ? &node->rx : NULL;
        sFake.rx.prev   = NULL;
        sFake.rx.node   = NULL;
        sFake.children  = NULL;
        sFake.nchildren = 0;
        sFake.capacity  = 0;

        enMode          = mode;
        pCurr           = &sFake;
        pNext           = node;
        nIndex          = ~size_t(0);
        pPath           = NULL;
        pData           = NULL;
        nDataCap        = 0;
        pStorage        = storage;

        pGcNext         = storage->pIterators;
        storage->pIterators = this; // Link to the garbage
    }

    KVTIterator::~KVTIterator()
    {
        pCurr           = NULL;
        nIndex          = 0;
        vPath.flush();
        enMode          = KVTStorage::IT_INVALID;
        pPath           = NULL;

        if (pData != NULL)
        {
            ::free(pData);
            pData           = NULL;
        }

        nDataCap        = 0;
        pGcNext         = NULL;
        pStorage        = NULL;
    }

    bool KVTIterator::valid() const
    {
        return (pCurr != &sFake) && (pCurr != NULL) && (pCurr->refs > 0);
    }

    bool KVTIterator::tx_pending() const
    {
        return (valid()) && (pCurr->pending & KVT_TX);
    }

    bool KVTIterator::rx_pending() const
    {
        return (valid()) && (pCurr->pending & KVT_RX);
    }

    size_t KVTIterator::pending() const
    {
        return (valid()) && (pCurr->pending & (KVT_RX | KVT_TX));
    }

    size_t KVTIterator::flags() const
    {
        return ((valid()) && (pCurr->param != 0)) ? pCurr->param->flags : 0;
    }

    const char *KVTIterator::id() const
    {
        return (valid()) ? pCurr->id : NULL;
    }

    const char *KVTIterator::name() const
    {
        if (!valid())
            return NULL;

        if (pPath == NULL)
            pPath = pStorage->build_path(&pData, &nDataCap, pCurr);

        return pPath;
    }

    status_t KVTIterator::next()
    {
        // Invalidate current path
        pPath       = NULL;

        switch (enMode)
        {
            case KVTStorage::IT_TX_PENDING:
            {
                KVTStorage::kvt_link_t *lnk;

                pCurr       = pNext;
                if ((pCurr == NULL) || (!(pCurr->pending & KVT_TX)))
                {
                    enMode      = KVTStorage::IT_EOF;
                    return STATUS_NOT_FOUND;
                }

                lnk         = (pCurr != NULL) ? pCurr->tx.next : NULL;
                pNext       = (lnk != NULL) ? lnk->node : NULL;
                break;
            }

            case KVTStorage::IT_RX_PENDING:
            {
                KVTStorage::kvt_link_t *lnk;

                pCurr       = pNext;
                if ((pCurr == NULL) || (!(pCurr->pending & KVT_RX)))
                {
                    enMode      = KVTStorage::IT_EOF;
                    return STATUS_NOT_FOUND;
                }

                lnk         = (pCurr != NULL) ? pCurr->rx.next : NULL;
                pNext       = (lnk != NULL) ? lnk->node : NULL;
                break;
            }

            case KVTStorage::IT_ALL:
            {
                KVTStorage::kvt_link_t *lnk;

                pCurr       = pNext;
                if ((pCurr == NULL) || (pCurr->refs <= 0))
                {
                    enMode      = KVTStorage::IT_EOF;
                    return STATUS_NOT_FOUND;
                }

                lnk         = (pCurr != NULL) ? pCurr->gc.next : NULL;
                pNext       = (lnk != NULL) ? lnk->node : NULL;
                break;
            }

            case KVTStorage::IT_BRANCH:
            {
                if (pCurr->parent != NULL)
                {
                    do
                    {
                        if ((++nIndex) >= pCurr->parent->nchildren)
                        {
                            enMode      = KVTStorage::IT_EOF;
                            return STATUS_NOT_FOUND;
                        }
                        pCurr       = pCurr->parent->children[nIndex];
                    } while (pCurr->refs <= 0);
                }
                else
                {
                    enMode      = KVTStorage::IT_EOF;
                    return STATUS_NOT_FOUND;
                }
                break;
            }

            case KVTStorage::IT_RECURSIVE:
            {
                do
                {
                    if (pCurr->nchildren > 0)
                    {
                        kvt_path_t *path = vPath.push();
                        if (path == NULL)
                            return STATUS_NO_MEM;
                        path->index = nIndex + 1;
                        path->node  = pCurr;
                        pCurr       = pCurr->children[0];
                        nIndex      = 0;
                    }
                    else if (pCurr->parent != NULL)
                    {
                        if ((++nIndex) >= pCurr->parent->nchildren)
                        {
                            do {
                                kvt_path_t path;
                                if (!vPath.pop(&path))
                                {
                                    enMode      = KVTStorage::IT_EOF;
                                    return STATUS_NOT_FOUND;
                                }
                                nIndex      = path.index;
                                pCurr       = pCurr->parent;
                            } while (nIndex >= pCurr->parent->nchildren);
                        }
                        pCurr       = pCurr->parent->children[nIndex];
                    }
                    else
                    {
                        enMode      = KVTStorage::IT_EOF;
                        return STATUS_NOT_FOUND;
                    }
                } while (pCurr->refs <= 0);

                break;
            }

            case KVTStorage::IT_EOF:
                return STATUS_NOT_FOUND;

            default:
                return STATUS_BAD_STATE;
        }

        return STATUS_OK;
    }

    bool KVTIterator::exists(kvt_param_type_t type) const
    {
        if (!valid())
            return false;

        KVTStorage::kvt_gcparam_t *param = pCurr->param;
        if (param == NULL)
        {
            // Get parameter name
            const char *id = name();
            if (id == NULL)
                return false;

            pStorage->notify_missed(id);
            return false;
        }

        return ((type == KVT_ANY) || (type == param->type));
    }

    status_t KVTIterator::get(const kvt_param_t **value, kvt_param_type_t type)
    {
        if (!valid())
            return STATUS_BAD_STATE;

        const char *id = name();
        if (id == NULL)
            return STATUS_NO_MEM;

        KVTStorage::kvt_gcparam_t *param = pCurr->param;
        if (param == NULL)
        {
            pStorage->notify_missed(id);
            return STATUS_NOT_FOUND;
        }

        if ((type != KVT_ANY) && (type != param->type))
            return STATUS_BAD_TYPE;

        // All seems to be OK
        if (value != NULL)
        {
            *value  = param;
            pStorage->notify_access(id, param, pCurr->pending);
        }
        return STATUS_OK;
    }

    status_t KVTIterator::get(uint32_t *value)
    {
        const kvt_param_t *param;
        status_t res        = get(&param, KVT_UINT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->u32;
        return res;
    }

    status_t KVTIterator::get(int32_t *value)
    {
        const kvt_param_t *param;
        status_t res        = get(&param, KVT_INT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->i32;
        return res;
    }

    status_t KVTIterator::get(uint64_t *value)
    {
        const kvt_param_t *param;
        status_t res        = get(&param, KVT_UINT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->u64;
        return res;
    }

    status_t KVTIterator::get(int64_t *value)
    {
        const kvt_param_t *param;
        status_t res        = get(&param, KVT_INT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->i64;
        return res;
    }

    status_t KVTIterator::get(float *value)
    {
        const kvt_param_t *param;
        status_t res        = get(&param, KVT_FLOAT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->f32;
        return res;
    }

    status_t KVTIterator::get(double *value)
    {
        const kvt_param_t *param;
        status_t res        = get(&param, KVT_FLOAT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->f64;
        return res;
    }

    status_t KVTIterator::get(const char **value)
    {
        const kvt_param_t *param;
        status_t res        = get(&param, KVT_STRING);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = param->str;
        return res;
    }

    status_t KVTIterator::get(const kvt_blob_t **value)
    {
        const kvt_param_t *param;
        status_t res        = get(&param, KVT_BLOB);
        if ((res == STATUS_OK) && (value != NULL))
            *value      = &param->blob;
        return res;
    }

    status_t KVTIterator::put(const kvt_param_t *value, size_t flags)
    {
        if (!valid())
            return STATUS_BAD_STATE;
        if (!KVTStorage::validate_type(value->type))
            return STATUS_BAD_TYPE;

        const char *id = name();
        if (id == NULL)
            return STATUS_NO_MEM;

        return pStorage->commit_parameter(id, pCurr, value, flags);
    }

    status_t KVTIterator::put(uint32_t value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_UINT32;
        param.u32   = value;
        return put(&param, flags | KVT_DELEGATE);
    }

    status_t KVTIterator::put(int32_t value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_INT32;
        param.i32   = value;
        return put(&param, flags | KVT_DELEGATE);
    }

    status_t KVTIterator::put(uint64_t value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_UINT64;
        param.u64   = value;
        return put(&param, flags | KVT_DELEGATE);
    }

    status_t KVTIterator::put(int64_t value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_INT64;
        param.i64   = value;
        return put(&param, flags | KVT_DELEGATE);
    }

    status_t KVTIterator::put(float value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_FLOAT32;
        param.f32   = value;
        return put(&param, flags | KVT_DELEGATE);
    }

    status_t KVTIterator::put(double value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_FLOAT64;
        param.f64   = value;
        return put(&param, flags | KVT_DELEGATE);
    }

    status_t KVTIterator::put(const char *value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_STRING;
        param.str   = const_cast<char *>(value);
        return put(&param, flags);
    }

    status_t KVTIterator::put(const kvt_blob_t *value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_BLOB;
        param.blob  = *value;
        return put(&param, flags);
    }

    status_t KVTIterator::put(size_t size, const char *type, const void *value, size_t flags)
    {
        kvt_param_t param;
        param.type  = KVT_BLOB;
        param.blob.size     = size;
        param.blob.ctype    = type;
        param.blob.data     = value;
        return put(&param, flags);
    }

    status_t KVTIterator::remove(const kvt_param_t **value, kvt_param_type_t type)
    {
        if (!valid())
            return STATUS_BAD_STATE;

        const char *id = name();
        if (id == NULL)
            return STATUS_NO_MEM;

        return pStorage->do_remove_node(id, pCurr, value, type);
    }

    status_t KVTIterator::remove(uint32_t *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(&param, KVT_UINT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->u32;
        return res;
    }

    status_t KVTIterator::remove(int32_t *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(&param, KVT_INT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->i32;
        return res;
    }

    status_t KVTIterator::remove(uint64_t *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(&param, KVT_UINT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->u64;
        return res;
    }

    status_t KVTIterator::remove(int64_t *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(&param, KVT_INT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->i64;
        return res;
    }

    status_t KVTIterator::remove(float *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(&param, KVT_FLOAT32);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->f32;
        return res;
    }

    status_t KVTIterator::remove(double *value)
    {
        const kvt_param_t *param;
        status_t res        = remove(&param, KVT_FLOAT64);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->f64;
        return res;
    }

    status_t KVTIterator::remove(const char **value)
    {
        const kvt_param_t *param;
        status_t res        = remove(&param, KVT_STRING);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = param->str;
        return res;
    }

    status_t KVTIterator::remove(const kvt_blob_t **value)
    {
        const kvt_param_t *param;
        status_t res        = remove(&param, KVT_BLOB);
        if ((res == STATUS_OK) && (value != NULL))
            *value              = &param->blob;
        return res;
    }

    status_t KVTIterator::touch(size_t flags)
    {
        if (!valid())
            return STATUS_BAD_STATE;

        const char *id = name();
        if (id == NULL)
            return STATUS_NO_MEM;

        return pStorage->do_touch(id, pCurr, flags);
    }

    status_t KVTIterator::commit(size_t flags)
    {
        if (!valid())
            return STATUS_BAD_STATE;

        const char *id = name();
        if (id == NULL)
            return STATUS_NO_MEM;

        return pStorage->do_commit(id, pCurr, flags);
    }

    status_t KVTIterator::remove_branch()
    {
        if (!valid())
            return STATUS_BAD_STATE;

        const char *id = name();
        if (id == NULL)
            return STATUS_NO_MEM;

        return pStorage->do_remove_branch(id, pCurr);
    }

#ifdef LSP_DEBUG
    static void kvt_dump_parameterv(const char *fmt, const kvt_param_t *param, va_list args)
    {
        lsp_nvprintf(fmt, args);

        switch (param->type)
        {
            case KVT_INT32:     lsp_nprintf("i32(0x%lx)\n", long(param->i32)); break;
            case KVT_UINT32:    lsp_nprintf("u32(0x%lx)\n", (unsigned long)(param->u32)); break;
            case KVT_INT64:     lsp_nprintf("i64(0x%llx)\n", (long long)(param->i64)); break;
            case KVT_UINT64:    lsp_nprintf("i64(0x%llx)\n", (unsigned long long)(param->u64)); break;
            case KVT_FLOAT32:   lsp_nprintf("f32(%f)\n", param->f32); break;
            case KVT_FLOAT64:   lsp_nprintf("f64(%f)\n", param->f64); break;
            case KVT_STRING:    lsp_nprintf("str(%s)\n", param->str); break;
            case KVT_BLOB:
                lsp_nprintf("blob(size=%d, type=(%s), data=(", int(param->blob.size), param->blob.ctype);
                if (param->blob.data != NULL)
                {
                    const uint8_t *ptr = reinterpret_cast<const uint8_t *>(param->blob.data);
                    for (size_t i=0; i<param->blob.size; ++i)
                    {
                        if (i)
                            lsp_nprintf(" %02x", int(ptr[i]));
                        else
                            lsp_nprintf("%02x", int(ptr[i]));
                    }
                    lsp_nprintf(")\n");
                }
                else
                    lsp_nprintf("nil))\n");
                break;
            default:
                lsp_nprintf(" <unsupported parameter type %d>\n", int(param->type));
                break;
        }
    }

    void kvt_dump_parameter(const char *prefix, const kvt_param_t *param...)
    {
        va_list vlst;
        va_start(vlst, param);
        kvt_dump_parameterv(prefix, param, vlst);
        va_end(vlst);
    }
#endif /* LSP_TRACE */

} /* namespace lsp */
