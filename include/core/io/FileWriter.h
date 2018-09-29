/*
 * FileWriter.h
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_FILEWRITER_H_
#define CORE_IO_FILEWRITER_H_

#include <stdio.h>
#include <core/io/Writer.h>
#include <core/types.h>

namespace lsp
{
    namespace io
    {
        class FileWriter: public Writer
        {
            protected:
                uint8_t        *bBuf;
                lsp_wchar_t    *cBuf;
                size_t          bBufPos;
                size_t          cBufPos;
                FILE           *pFD;
                bool            bClose;
                iconv_t         hIconv;
    
            protected:
                void            do_destroy();
                status_t        initialize(FILE *fd, const char *charset, bool close);
                status_t        init_buffers();
                status_t        flush_buffer(bool force);
                status_t        flush_byte_buffer();

            public:
                FileWriter();
                virtual ~FileWriter();

            public:
                status_t attach(FILE *fd, const char *charset = NULL);

                status_t open(FILE *fd, const char *charset = NULL);

                status_t open(const char *path, const char *charset = NULL);

                status_t append(const char *path, const char *charset = NULL);

                virtual status_t write(lsp_wchar_t c);

                virtual status_t write(const lsp_wchar_t *c, size_t count);

                virtual status_t write_ascii(const char *s);

                virtual status_t write(const LSPString *s);

                virtual status_t write(const LSPString *s, ssize_t first);

                virtual status_t write(const LSPString *s, ssize_t first, ssize_t last);

                virtual status_t flush();

                virtual status_t close();
        };
    }
} /* namespace lsp */

#endif /* CORE_IO_FILEWRITER_H_ */
