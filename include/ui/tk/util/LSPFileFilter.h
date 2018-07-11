/*
 * LSPFileFilter.h
 *
 *  Created on: 6 мая 2018 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPFILEFILTER_H_
#define UI_TK_UTIL_LSPFILEFILTER_H_

namespace lsp
{
    namespace tk
    {
        class LSPFileFilter
        {
            protected:
                typedef struct filter_t
                {
                    LSPFileMask     sPattern;
                    LSPString       sExtension;
                    LSPString       sTitle;
                } filter_t;

            protected:
//                size_t              nUIDGen;
                ssize_t             nDefault;
                cvector<filter_t>   vItems;

            protected:
                virtual status_t item_updated(size_t idx, filter_t *flt);

                virtual status_t item_removed(size_t idx, filter_t *flt);

                virtual status_t item_added(size_t idx, filter_t *flt);

                virtual void default_updated(ssize_t idx);

            public:
                LSPFileFilter();
                virtual ~LSPFileFilter();

//            protected:
//                filter_t    *find_by_uid(size_t id);
//                size_t      gen_next_uid();

            public:
                inline size_t   size() const        { return vItems.size(); }
                inline ssize_t  get_default() const { return nDefault; }
                status_t clear();

                status_t add(const LSPString *pattern, const LSPString *title, const LSPString *ext, size_t fiags = 0, bool dfl = false);
                status_t add(const char *pattern, const char *title, const char *ext,  size_t fiags = 0, bool dfl = false);
                status_t remove(size_t index);
                status_t set_default(size_t value);

                status_t get_pattern(size_t id, LSPString *pattern) const;
                const char *get_pattern(size_t id) const;

                LSPFileMask *get_mask(size_t id) const;

                status_t set_pattern(size_t id, const LSPString *pattern, size_t flags = 0);
                status_t set_pattern(size_t id, const char *pattern, size_t flags = 0);

                status_t get_title(size_t id, LSPString *title) const;
                const char *get_title(size_t id) const;

                status_t set_title(size_t id, const LSPString *title);
                status_t set_title(size_t id, const char *title);

                status_t get_extension(size_t id, LSPString *ext) const;
                const char *get_extension(size_t id) const;

                status_t set_extension(size_t id, const LSPString *ext);
                status_t set_extension(size_t id, const char *ext);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPFILEFILTER_H_ */
