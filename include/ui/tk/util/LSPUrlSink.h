/*
 * LSPUrlSink.h
 *
 *  Created on: 28 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPURLSINK_H_
#define UI_TK_UTIL_LSPURLSINK_H_

namespace lsp
{
    namespace tk
    {
        class LSPUrlSink: public IDataSink
        {
            protected:
                static const char * const acceptMime[];

            protected:
                enum ctype_t
                {
                    TEXT_URI_LIST,
                    TEXT_X_MOZ_URL,
                    APPLICATION_X_KDE4_URILIST,
                    TEXT_PLAIN
                };

            protected:
                io::OutMemoryStream    *pOS;
                char                   *sProtocol;
                ssize_t                 nCtype;

            protected:
                ssize_t     get_mime_index(const char *mime);

            public:
                explicit LSPUrlSink();
                explicit LSPUrlSink(const char *protocol);
                virtual ~LSPUrlSink();

            public:
                ssize_t             select_mime_type(const char * const *mime_types);

                inline const char  *get_protocol() const { return sProtocol; }

                virtual status_t    commit_url(const LSPString *url);

                status_t            set_protocol(const char *protocol);

            public:
                virtual ssize_t     open(const char * const *mime_types);
                virtual status_t    write(const void *buf, size_t count);
                virtual status_t    close(status_t code);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPURLSINK_H_ */
