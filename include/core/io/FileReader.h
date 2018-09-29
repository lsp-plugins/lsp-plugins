/*
 * FileCharInputStream.h
 *
 *  Created on: 16 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_FILEREADER_H_
#define CORE_IO_FILEREADER_H_

#include <stdio.h>
#include <core/io/Reader.h>
#include <core/types.h>

namespace lsp
{
    namespace io
    {
        class FileReader: public Reader
        {
            protected:
                uint8_t        *bBuf;
                lsp_wchar_t    *cBuf;
                size_t          bBufSize;
                size_t          bBufPos;
                size_t          cBufSize;
                size_t          cBufPos;
                FILE           *pFD;
                bool            bClose;
                status_t        nError;
                iconv_t         hIconv;
                LSPString       sLine;
    
            protected:
                void            do_destroy();
                status_t        initialize(FILE *fd, const char *charset, bool close);
                status_t        init_buffers();
                status_t        fill_char_buf();

            public:
                FileReader();
                virtual ~FileReader();

            public:
                /** Attach input stream to descriptor. When stream is closed, file descriptor
                 * keeps to be opened. Before attach currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param fd file descriptor
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t attach(FILE *fd, const char *charset = NULL);

                /** Open input stream as wrapper of file descriptor. When stream is closed, it automatically
                 * closes file descriptor. Before open currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param fd file descriptor
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t open(FILE *fd, const char *charset = NULL);

                /** Open input stream associated with file. Before open currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param path file location path
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t open(const char *path, const char *charset = NULL);

                virtual ssize_t     read(lsp_wchar_t *dst, size_t count);

                virtual int         read();

                virtual status_t    read_line(LSPString *s, bool force = false);

                virtual ssize_t     skip(size_t count);

                virtual status_t    error();

                virtual status_t    close();
        };
    }
} /* namespace lsp */

#endif /* CORE_IO_FILEREADER_H_ */
