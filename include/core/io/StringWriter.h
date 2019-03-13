/*
 * StringWriter.h
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_STRINWRITER_H_
#define CORE_IO_STRINWRITER_H_

#include <core/io/Writer.h>

namespace lsp
{
    namespace io
    {
        class StringWriter: public Writer
        {
            private:
                LSPString  *pOut;
                bool        bDelete;

            private:
                StringWriter & operator = (const StringWriter &);

            public:
                explicit StringWriter(LSPString *out, bool del = false);
                virtual ~StringWriter();
    
            public:
                status_t wrap(LSPString *out, bool del);

                virtual status_t    write(lsp_wchar_t c);

                virtual status_t    write(const lsp_wchar_t *c, size_t count);

                virtual status_t    write_ascii(const char *s);

                virtual status_t    write(const LSPString *s);

                virtual status_t    write(const LSPString *s, ssize_t first);

                virtual status_t    write(const LSPString *s, ssize_t first, ssize_t last);

                virtual status_t    flush();

                virtual status_t    close();
        };
    }
} /* namespace lsp */

#endif /* CORE_IO_STRINWRITER_H_ */
