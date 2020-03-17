/*
 * LSPFileFilter.cpp
 *
 *  Created on: 6 мая 2018 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        void LSPFileFilter::FilterItem::sync()
        {
            if (pFilter == NULL)
                return;

            ssize_t index = pFilter->vItems.index_of(this);
            if (index < 0)
                return;

            pFilter->item_updated(index, this);
        }
        
        LSPFileFilter::LSPFileFilter()
        {
            nDefault    = -1;
        }
        
        LSPFileFilter::~LSPFileFilter()
        {
            clear();
        }

        status_t LSPFileFilter::item_updated(size_t idx, LSPFileFilterItem *flt)
        {
            return STATUS_OK;
        }

        status_t LSPFileFilter::item_removed(size_t idx, LSPFileFilterItem *flt)
        {
            return STATUS_OK;
        }

        status_t LSPFileFilter::item_added(size_t idx, LSPFileFilterItem *flt)
        {
            return STATUS_OK;
        }

        void LSPFileFilter::default_updated(ssize_t idx)
        {
        }

        status_t LSPFileFilter::clear()
        {
            status_t result = STATUS_OK;

            for (size_t i = 0, n = vItems.size(); i < n; ++i)
            {
                LSPFileFilterItem *f = vItems.get(i);
                if (f == NULL)
                    continue;
                status_t rem_result = item_removed(0, f);
                if (rem_result != STATUS_OK)
                    result = rem_result;
                delete f;
            }
            vItems.clear();

            if (nDefault != -1)
            {
                nDefault    = -1;
                default_updated(nDefault);
            }

            return result;
        }

        ssize_t LSPFileFilter::add(const LSPFileFilterItem *item)
        {
            FilterItem *x = new FilterItem();
            if (x == NULL)
                return -STATUS_NO_MEM;

            status_t res = x->set(item);
            if (res != STATUS_OK)
            {
                delete x;
                return -res;
            }

            ssize_t idx = vItems.size();
            if (!vItems.add(x))
            {
                delete x;
                return -STATUS_NO_MEM;
            }
            x->bind(this);

            item_added(idx, x);
            return idx;
        }

        status_t LSPFileFilter::set(size_t index, const LSPFileFilterItem *item)
        {
            FilterItem *f     = vItems.get(index);
            if (f == NULL)
                return STATUS_INVALID_VALUE;

            f->pFilter = NULL;
            status_t res = f->set(item);
            f->pFilter = this;
            if (res != STATUS_OK)
                return res;

            item_updated(index, f);
            return res;
        }

        status_t LSPFileFilter::insert(size_t index, const LSPFileFilterItem *item)
        {
            FilterItem *x = new FilterItem();
            if (x == NULL)
                return -STATUS_NO_MEM;

            status_t res = x->set(item);
            if (res != STATUS_OK)
            {
                delete x;
                return -res;
            }

            if (!vItems.insert(x, index))
            {
                delete x;
                return -STATUS_NO_MEM;
            }
            x->bind(this);

            item_added(index, x);
            return STATUS_OK;
        }

        status_t LSPFileFilter::remove(size_t index, LSPFileFilterItem *res)
        {
            FilterItem *f     = vItems.get(index);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            // Remove, unbind, notify
            vItems.remove(index, false);
            f->pFilter = NULL;
            item_removed(index, f);

            // Return as result if specified
            if (res != NULL)
                res->swap(f);
            delete f;

            if (nDefault == ssize_t(index))
            {
                nDefault    = -1;
                default_updated(nDefault);
            }

            return STATUS_OK;
        }

        status_t LSPFileFilter::set_default(size_t value)
        {
            if ((value < 0) || (value >= vItems.size()))
                return STATUS_BAD_ARGUMENTS;
            if (ssize_t(value) == nDefault)
                return STATUS_OK;

            nDefault    = value;
            default_updated(nDefault);

            return STATUS_OK;
        }

        LSPFileFilterItem *LSPFileFilter::get(size_t index)
        {
            return vItems.get(index);
        }

        const LSPFileFilterItem *LSPFileFilter::get(size_t index) const
        {
            return vItems.get(index);
        }
    
    } /* namespace tk */
} /* namespace lsp */
