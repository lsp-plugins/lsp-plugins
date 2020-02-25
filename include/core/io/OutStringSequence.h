/*
 * StringWriter.h
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_STRINWRITER_H_
#define CORE_IO_STRINWRITER_H_

#include <core/io/IOutSequence.h>

namespace lsp
{
    namespace io
    {
        class OutStringSequence: public IOutSequence
        {
            private:
                LSPString  *pOut;
                bool        bDelete;

            private:
                OutStringSequence & operator = (const OutStringSequence &);

            public:
                explicit OutStringSequence();
                explicit OutStringSequence(LSPString *out, bool del = false);
                virtual ~OutStringSequence();
    
            public:
                status_t            wrap(LSPString *out, bool del);

                virtual status_t    write(lsp_wchar_t c);

                virtual status_t    write(const lsp_wchar_t *c, size_t count);

                virtual status_t    write_ascii(const char *s);

                virtual status_t    write_ascii(const char *s, size_t count);

                virtual status_t    writeln_ascii(const char *s);

                virtual status_t    write(const LSPString *s);

                virtual status_t    write(const LSPString *s, ssize_t first);

                virtual status_t    write(const LSPString *s, ssize_t first, ssize_t last);

                virtual status_t    flush();

                virtual status_t    close();
        };
    }
} /* namespace lsp */

#endif /* CORE_IO_STRINWRITER_H_ */
