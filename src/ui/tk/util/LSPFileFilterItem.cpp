/*
 * LSPFileFilterItem.cpp
 *
 *  Created on: 15 мар. 2020 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        
        void LSPFileFilterItem::Title::sync()
        {
            if (pItem != NULL)
                pItem->sync();
        }

        LSPFileFilterItem::LSPFileFilterItem():
            sTitle(this)
        {
        }
        
        LSPFileFilterItem::~LSPFileFilterItem()
        {
        }

        void LSPFileFilterItem::sync()
        {
            // Callback for handling changes
        }

        status_t LSPFileFilterItem::set_extension(const LSPString *ext)
        {
            status_t res = (sExtension.set(ext)) ? STATUS_OK : STATUS_NO_MEM;
            if (res == STATUS_OK)
                sync();

            return res;
        }

        status_t LSPFileFilterItem::set_extension(const char *ext)
        {
            status_t res = (sExtension.set_utf8(ext)) ? STATUS_OK : STATUS_NO_MEM;
            if (res == STATUS_OK)
                sync();

            return res;
        }

        status_t LSPFileFilterItem::set(const LSPFileFilterItem *src)
        {
            if (src == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPFileMask fm;
            LSPString ext;

            // Copy data
            status_t res = fm.set(&src->sPattern);
            if (res != STATUS_OK)
                return res;
            if (!ext.set(&src->sExtension))
                return STATUS_NO_MEM;

            // Swap data
            sPattern.swap(&fm);
            sExtension.swap(&ext);
            if ((res = sTitle.set(&src->sTitle)) != STATUS_OK)
            {
                sPattern.swap(&fm);
                sExtension.swap(&ext);
            }

            return res;
        }

        void LSPFileFilterItem::swap(LSPFileFilterItem *src)
        {
            sPattern.swap(&src->sPattern);
            sExtension.swap(&src->sExtension);
            sTitle.swap(&src->sTitle);
        }
    
    } /* namespace tk */
} /* namespace lsp */
