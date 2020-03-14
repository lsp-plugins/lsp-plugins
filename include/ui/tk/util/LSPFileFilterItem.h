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

            public:
                explicit LSPFileFilterItem();
                virtual ~LSPFileFilterItem();

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
                inline LSPLocalString          *title()         { return &sTitle; };
                inline const LSPLocalString    *title() const   { return &sTitle; };

                inline status_t get_pattern(LSPString *pattern) const { return sPattern.get_mask(pattern); }
                inline const char *get_pattern() const { return sPattern.mask(); }

                inline status_t get_extension(LSPString *ext) const;
                inline const char *get_extension() const;

            public:
                status_t set_pattern(const LSPString *pattern, size_t flags = 0);
                status_t set_pattern(const char *pattern, size_t flags = 0);

                status_t set_extension(const LSPString *ext);
                status_t set_extension(const char *ext);

                status_t set(const LSPFileFilterItem *src);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPFILEFILTERITEM_H_ */
