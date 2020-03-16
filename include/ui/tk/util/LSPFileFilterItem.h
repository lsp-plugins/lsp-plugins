/*
 * LSPFileFilterItem.h
 *
 *  Created on: 15 мар. 2020 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPFILEFILTERITEM_H_
#define UI_TK_UTIL_LSPFILEFILTERITEM_H_

namespace lsp
{
    namespace tk
    {
        /**
         * This class defines File filter item which can be used for
         * filtering files in dialogs
         */
        class LSPFileFilterItem
        {
            private:
                LSPFileFilterItem & operator = (const LSPFileFilterItem &);

            protected:
                class Title: public LSPLocalString
                {
                    protected:
                        LSPFileFilterItem *pItem;

                    protected:
                        virtual void        sync();

                    public:
                        inline Title(LSPFileFilterItem *item) { pItem = item; }
                };

            protected:
                LSPFileMask         sPattern;
                LSPString           sExtension;
                Title               sTitle;

            protected:
                virtual void sync();

            public:
                explicit LSPFileFilterItem();
                virtual ~LSPFileFilterItem();

            public:
                inline LSPLocalString          *title()         { return &sTitle; };
                inline const LSPLocalString    *title() const   { return &sTitle; };

                inline LSPFileMask             *pattern()       { return &sPattern; };
                inline const LSPFileMask       *pattern() const { return &sPattern; };

                inline status_t                 get_extension(LSPString *ext) const
                {
                    if (ext == NULL)
                        return STATUS_BAD_ARGUMENTS;
                    return (ext->set(&sExtension)) ? STATUS_OK : STATUS_NO_MEM;
                }
                inline const char              *get_extension() const { return sExtension.get_utf8(); };

            public:
                status_t set_extension(const LSPString *ext);
                status_t set_extension(const char *ext);

                status_t set(const LSPFileFilterItem *src);

                void swap(LSPFileFilterItem *src);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPFILEFILTERITEM_H_ */
