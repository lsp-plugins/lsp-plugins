/*
 * LSPFileMask.h
 *
 *  Created on: 9 окт. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPFILEMASK_H_
#define UI_TK_UTIL_LSPFILEMASK_H_

namespace lsp
{
    namespace tk
    {
        /**
         * This class defines mask of the file which can be used
         * for filtering files in dialogs
         */
        class LSPFileMask
        {
            private:
                LSPFileMask & operator = (const LSPFileMask &);

            public:
                enum flags
                {
                    INVERSIVE       = 1 << 0,
                    CASE_SENSITIVE  = 1 << 1,

                    NONE            = 0
                };

            protected:
                typedef struct simplemask_t
                {
                    const lsp_wchar_t      *pHead;
                    const lsp_wchar_t      *pTail;
                    bool                    bInvert;
                } simplemask_t;

                typedef struct biter_t
                {
                    lsp_wchar_t        *pHead;
                    lsp_wchar_t        *pTail;
                    bool                bInvert;
                } biter_t;

           protected:
                LSPString               sMask;
                cstorage<simplemask_t>  vMasks;
                lsp_wchar_t            *pBuffer;
                size_t                  nFlags;

            protected:
                static simplemask_t        *parse_simple(cstorage<simplemask_t> *dst, biter_t *bi);
                static bool                 check_simple_case(const lsp_wchar_t *head, const lsp_wchar_t *tail, const lsp_wchar_t *shead, const lsp_wchar_t *stail);
                static bool                 check_simple_nocase(const lsp_wchar_t *head, const lsp_wchar_t *tail, const lsp_wchar_t *shead, const lsp_wchar_t *stail);
                bool                        check_mask(simplemask_t *mask, const lsp_wchar_t *s, size_t len);

            public:
                explicit LSPFileMask();
                virtual ~LSPFileMask();

            public:
                inline const char *mask() const { return sMask.get_utf8(); }
                inline status_t get_mask(LSPString *mask) const { return (mask != NULL) ? mask->set(&sMask) : STATUS_BAD_ARGUMENTS; }
                inline size_t flags() const { return nFlags; }

            public:
                void        swap(LSPFileMask *mask);

                status_t    parse(const LSPString *pattern, size_t flags = NONE);
                status_t    parse(const char *pattern, size_t flags = NONE);

                status_t    set(const LSPFileMask *mask);
                status_t    set(const LSPString *pattern, size_t flags = NONE) { return parse(pattern, flags); };
                status_t    set(const char *pattern, size_t flags = NONE) { return parse(pattern, flags); };

                void        set_flags(size_t flags);

                bool        matched(const LSPString *str);
                bool        matched(const char *text);

                void        clear();

                static bool valid_file_name(const char *fname);
                static bool valid_file_name(const LSPString *fname);

                static bool valid_path_name(const char *fname);
                static bool valid_path_name(const LSPString *fname);

                static status_t append_path(LSPString *path, const char *fname);
                static status_t append_path(LSPString *path, const LSPString *fname);

                static status_t append_path(LSPString *dst, const LSPString *path, const char *fname);
                static status_t append_path(LSPString *dst, const LSPString *path, const LSPString *fname);

                static bool is_dot(const LSPString *fname);
                static bool is_dotdot(const LSPString *fname);
                static bool is_dots(const LSPString *fname);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPFILEMASK_H_ */
