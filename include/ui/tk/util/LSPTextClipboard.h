/*
 * LSPTextClipboard.h
 *
 *  Created on: 17 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPTEXTCLIPBOARD_H_
#define UI_TK_UTIL_LSPTEXTCLIPBOARD_H_

namespace lsp
{
    namespace tk
    {
        class LSPTextClipboard: public IClipboard
        {
            protected:
                LSPString       sString;

            protected:
                static const char *x_targets[];

            protected:
                class InputStream: public io::IInStream
                {
                    protected:
                        char           *pData;
                        size_t          nPosition;
                        size_t          nTotal;

                    public:
                        explicit InputStream(char *data, size_t total);
                        virtual ~InputStream();

                    public:
                        virtual wssize_t    avail();

                        virtual wssize_t    position();

                        virtual ssize_t     read(void *dst, size_t count);

                        virtual wssize_t    seek(wsize_t position);

                        virtual status_t    close();
                };

            public:
                explicit LSPTextClipboard();
                virtual ~LSPTextClipboard();

            public:
                virtual io::IInStream   *read(const char *ctype);
                virtual size_t              targets();
                virtual const char         *target(size_t i);

                status_t        update_text(const LSPString *text);
                status_t        update_text(const LSPString *text, ssize_t first);
                status_t        update_text(const LSPString *text, ssize_t first, ssize_t last);
        };
        
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPTEXTCLIPBOARD_H_ */
