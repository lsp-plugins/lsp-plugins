/*
 * StringReader.h
 *
 *  Created on: 14 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_STRINGREADER_H_
#define CORE_IO_STRINGREADER_H_

#include <core/io/Reader.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace io
    {
        class StringReader: public Reader
        {
            private:
                const LSPString    *pString;
                size_t              nOffset;
                bool                bDestroy;
                status_t            nError;

            protected:
                void    do_close();

            private:
                StringReader & operator = (const StringReader &);

            public:
                explicit StringReader(const LSPString *s, bool destroy = false);
                virtual ~StringReader();

            public:
                virtual ssize_t     read(lsp_wchar_t *dst, size_t count);

                virtual int         read();

                virtual status_t    read_line(LSPString *s, bool force = false);

                virtual ssize_t     skip(size_t count);

                virtual status_t    close();

                virtual status_t    error();
        };
    }
} /* namespace lsp */

#endif /* CORE_IO_STRINGREADER_H_ */
