/*
 * LSPStyle.cpp
 *
 *  Created on: 1 окт. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        
        LSPStyle::LSPStyle(LSPWidget *widget)
        {
            pParent     = NULL;
            pWidget     = widget;
        }
        
        LSPStyle::~LSPStyle()
        {
            do_destroy();
        }

        status_t LSPStyle::init()
        {
            return STATUS_OK;
        }

        void LSPStyle::destroy()
        {
            do_destroy();
        }

        void LSPStyle::do_destroy()
        {
            if (pParent != NULL)
            {
                pParent->vChildren.remove(this, true);
                pParent     = NULL;
            }
            pWidget     = NULL;
        }

        void LSPStyle::notify_all()
        {
            // Notify all child styles and widget
            for (size_t i=0, n=vChildren.size(); i<n; ++i)
            {
                LSPStyle *child = vChildren.at(i);
                if (child != NULL)
                    child->notify_all();
            }

            // Query widget (if present) for redraw
            if (pWidget != NULL)
                pWidget->query_draw();
        }

        status_t LSPStyle::add(LSPStyle *child)
        {
            if ((child == NULL) || (child == this))
                return STATUS_BAD_ARGUMENTS;
            if (child->pParent == this)
                return STATUS_ALREADY_EXISTS;

            if (!vChildren.add(child))
                return STATUS_NO_MEM;
            if (child->pParent != NULL)
                child->pParent->vChildren.remove(child, true);

            child->pParent  = this;
            child->notify_all();

            return STATUS_OK;
        }

        status_t LSPStyle::remove(LSPStyle *child)
        {
            if (child == NULL)
                return STATUS_BAD_ARGUMENTS;

            ssize_t idx = vChildren.index_of(child);
            if (idx < 0)
                return STATUS_NOT_FOUND;

            if (!vChildren.remove(idx, true))
                return STATUS_UNKNOWN_ERR;

            child->pParent = NULL;
            child->notify_all();

            return STATUS_OK;
        }

        status_t LSPStyle::set_parent(LSPStyle *parent)
        {
            if (parent == this)
                return STATUS_BAD_ARGUMENTS;
            else if (pParent == parent)
                return STATUS_OK;

            if (parent == NULL)
            {
                if (!pParent->vChildren.remove(this, true))
                    return STATUS_UNKNOWN_ERR;
            }
            else
            {
                if (!parent->vChildren.add(this))
                    return STATUS_NO_MEM;
                if (pParent != NULL)
                    pParent->vChildren.remove(this, true);
            }

            pParent     = parent;
            notify_all();

            return STATUS_OK;
        }

        LSPStyle *LSPStyle::root()
        {
            LSPStyle *curr = this;
            while (curr->pParent != NULL)
                curr = curr->pParent;
            return curr;
        }
    
    } /* namespace tk */
} /* namespace lsp */
