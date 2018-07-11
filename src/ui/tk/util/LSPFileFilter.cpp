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
        
        LSPFileFilter::LSPFileFilter()
        {
            nDefault    = -1;
        }
        
        LSPFileFilter::~LSPFileFilter()
        {
            clear();
        }

        status_t LSPFileFilter::item_updated(size_t idx, filter_t *flt)
        {
            return STATUS_OK;
        }

        status_t LSPFileFilter::item_removed(size_t idx, filter_t *flt)
        {
            return STATUS_OK;
        }

        status_t LSPFileFilter::item_added(size_t idx, filter_t *flt)
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
                filter_t *f = vItems.get(i);
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

        status_t LSPFileFilter::add(const LSPString *pattern, const LSPString *title, const LSPString *ext, size_t flags, bool dfl)
        {
            filter_t *f = new filter_t();
            if (f == NULL)
                return STATUS_NO_MEM;

            status_t res = (pattern != NULL) ? f->sPattern.parse(pattern, flags) : f->sPattern.parse("*", flags);
            if (res != STATUS_OK)
            {
                delete f;
                return res;
            }

            bool set = (title != NULL) ? f->sTitle.set(title) : f->sTitle.set_native("");
            if (!set)
            {
                delete f;
                return STATUS_NO_MEM;
            }

            set = (title != NULL) ? f->sExtension.set(title) : f->sExtension.set_native("");
            if (!set)
            {
                delete f;
                return STATUS_NO_MEM;
            }

            ssize_t idx = vItems.size();
            if (!vItems.add(f))
            {
                delete f;
                return res;
            }

            res         = item_added(idx, f);
            if (res != STATUS_OK)
            {
                vItems.remove(f, false);
                delete f;
                return res;
            }

            if ((dfl) && (nDefault != idx))
            {
                nDefault    = idx;
                default_updated(nDefault);
            }

            return idx;
        }

        status_t LSPFileFilter::add(const char *pattern, const char *title, const char *ext, size_t flags, bool dfl)
        {
            filter_t *f = new filter_t();
            if (f == NULL)
                return STATUS_NO_MEM;

            status_t res = f->sPattern.parse((pattern != NULL) ? pattern : "*", flags);
            if (res != STATUS_OK)
            {
                delete f;
                return res;
            }

            if (!f->sTitle.set_native((title != NULL) ? title : ""))
            {
                delete f;
                return STATUS_NO_MEM;
            }

            if (!f->sExtension.set_native((ext != NULL) ? ext : ""))
            {
                delete f;
                return STATUS_NO_MEM;
            }

            ssize_t idx = vItems.size();
            if (!vItems.add(f))
            {
                delete f;
                return res;
            }

            res         = item_added(idx, f);
            if (res != STATUS_OK)
            {
                vItems.remove(f, false);
                delete f;
                return res;
            }

            if ((dfl) && (nDefault != idx))
            {
                nDefault    = idx;
                default_updated(nDefault);
            }

            return idx;
        }

        status_t LSPFileFilter::remove(size_t index)
        {
            filter_t *f     = vItems.get(index);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            status_t res    = item_removed(index, f);
            if (res != STATUS_OK)
                return res;

            vItems.remove(index, false);
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

        LSPFileMask *LSPFileFilter::get_mask(size_t id) const
        {
            filter_t *f     = vItems.get(id);
            return (f != NULL) ? &f->sPattern : NULL;
        }

        status_t LSPFileFilter::get_pattern(size_t id, LSPString *pattern) const
        {
            filter_t *f     = vItems.get(id);
            return (f != NULL) ? f->sPattern.get_mask(pattern) : STATUS_BAD_ARGUMENTS;
        }

        const char *LSPFileFilter::get_pattern(size_t id) const
        {
            filter_t *f     = vItems.get(id);
            return (f != NULL) ? f->sPattern.mask() : NULL;
        }

        status_t LSPFileFilter::set_pattern(size_t id, const LSPString *pattern, size_t flags)
        {
            filter_t *f     = vItems.get(id);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPFileMask fm;
            size_t res      = fm.parse(pattern, flags);
            if (res != STATUS_OK)
                return res;

            fm.swap(&f->sPattern);
            res             = item_updated(id, f);
            if (res != STATUS_OK)
            {
                fm.swap(&f->sPattern); // Swap it back
                return res;
            }

            return STATUS_OK;
        }

        status_t LSPFileFilter::set_pattern(size_t id, const char *pattern, size_t flags)
        {
            filter_t *f     = vItems.get(id);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPFileMask fm;
            size_t res      = fm.parse(pattern, flags);
            if (res != STATUS_OK)
                return res;

            fm.swap(&f->sPattern);
            res             = item_updated(id, f);
            if (res != STATUS_OK)
            {
                fm.swap(&f->sPattern); // Swap it back
                return res;
            }

            return STATUS_OK;
        }

        status_t LSPFileFilter::get_title(size_t id, LSPString *title) const
        {
            filter_t *f     = vItems.get(id);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            return title->set(&f->sTitle);
        }

        const char *LSPFileFilter::get_title(size_t id) const
        {
            filter_t *f     = vItems.get(id);
            return (f != NULL) ? f->sTitle.get_native() : NULL;
        }

        status_t LSPFileFilter::set_title(size_t id, const LSPString *title)
        {
            filter_t *f     = vItems.get(id);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString s;
            status_t res = s.set(title);
            if (res != STATUS_OK)
                return res;

            s.swap(&f->sTitle);
            res             = item_updated(id, f);
            if (res != STATUS_OK)
            {
                s.swap(&f->sTitle); // Swap it back
                return res;
            }

            return STATUS_OK;
        }

        status_t LSPFileFilter::set_title(size_t id, const char *title)
        {
            filter_t *f     = vItems.get(id);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString s;
            if (!s.set_native(title))
                return STATUS_NO_MEM;

            s.swap(&f->sTitle);
            status_t res        = item_updated(id, f);
            if (res != STATUS_OK)
            {
                s.swap(&f->sTitle); // Swap it back
                return res;
            }

            return STATUS_OK;
        }

        status_t LSPFileFilter::get_extension(size_t id, LSPString *ext) const
        {
            filter_t *f     = vItems.get(id);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            return (ext->set(&f->sExtension)) ? STATUS_OK : STATUS_NO_MEM;
        }

        const char *LSPFileFilter::get_extension(size_t id) const
        {
            filter_t *f     = vItems.get(id);
            return (f != NULL) ? f->sExtension.get_native() : NULL;
        }

        status_t LSPFileFilter::set_extension(size_t id, const LSPString *ext)
        {
            filter_t *f     = vItems.get(id);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString s;
            status_t res = s.set(ext);
            if (res != STATUS_OK)
                return res;

            s.swap(&f->sExtension);
            res             = item_updated(id, f);
            if (res != STATUS_OK)
            {
                s.swap(&f->sExtension); // Swap it back
                return res;
            }

            return STATUS_OK;
        }

        status_t LSPFileFilter::set_extension(size_t id, const char *ext)
        {
            filter_t *f     = vItems.get(id);
            if (f == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPString s;
            status_t res = s.set_native(ext);
            if (res != STATUS_OK)
                return res;

            s.swap(&f->sExtension);
            res             = item_updated(id, f);
            if (res != STATUS_OK)
            {
                s.swap(&f->sExtension); // Swap it back
                return res;
            }

            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
