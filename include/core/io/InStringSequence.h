/*
 * StringReader.h
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_STRINGREADER_H_
#define CORE_IO_STRINGREADER_H_

#include <core/io/IInSequence.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace io
    {
        class InStringSequence: public IInSequence
        {
            private:
                LSPString          *pString;
                size_t              nOffset;
                bool                bDelete;

            protected:
                void    do_close();

            private:
                InStringSequence & operator = (const InStringSequence &);

            public:
                explicit InStringSequence();
                explicit InStringSequence(const LSPString *s);
                explicit InStringSequence(LSPString *s, bool del = false);
                virtual ~InStringSequence();

            public:
                status_t                wrap(const LSPString *in);
                status_t                wrap(LSPString *in, bool del);
                status_t                wrap(const char *s, const char *charset);
                status_t                wrap(const char *s);

                virtual ssize_t         read(lsp_wchar_t *dst, size_t count);

                virtual lsp_swchar_t    read();

                virtual status_t        read_line(LSPString *s, bool force = false);

                virtual ssize_t         skip(size_t count);

                virtual status_t        close();
        };
    }
} /* namespace lsp */

#endif /* CORE_IO_STRINGREADER_H_ */
