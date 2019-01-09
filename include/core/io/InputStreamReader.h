/*
 * InputStreamReader.h
 *
 *  Created on: 26 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_INPUTSTREAMREADER_H_
#define CORE_IO_INPUTSTREAMREADER_H_

#include <core/io/charset.h>
#include <core/io/IInputStream.h>
#include <core/io/Reader.h>

namespace lsp
{
    namespace io
    {
        class InputStreamReader: public Reader
        {
            protected:
                uint8_t        *bBuf;
                lsp_wchar_t    *cBuf;
                size_t          bBufSize;
                size_t          bBufPos;
                size_t          cBufSize;
                size_t          cBufPos;
                IInputStream   *pIS;
                bool            bClose;
                status_t        nError;
                LSPString       sLine;
#if defined(PLATFORM_WINDOWS)
                UINT            nCodePage;
#else
                iconv_t         hIconv;
#endif /* PLATFORM_WINDOWS */

            protected:
                void            do_destroy();
                status_t        initialize(IInputStream *is, const char *charset, bool close);
                status_t        init_buffers();
                status_t        fill_char_buf();

            public:
                InputStreamReader();
                virtual ~InputStreamReader();

            public:
                /** Attach input stream to descriptor. When stream is closed, file descriptor
                 * keeps to be opened. Before attach currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param fd file descriptor
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t attach(IInputStream *is, const char *charset = NULL);

                /** Open input stream as wrapper of file descriptor. When stream is closed, it automatically
                 * closes file descriptor. Before open currently open stream is closed and it's
                 * state is reset.
                 *
                 * @param fd file descriptor
                 * @param charset character set to use, system charset if NULL
                 * @return status of operation
                 */
                status_t open(IInputStream *is, const char *charset = NULL);

                virtual ssize_t     read(lsp_wchar_t *dst, size_t count);

                virtual int         read();

                virtual status_t    read_line(LSPString *s, bool force = false);

                virtual ssize_t     skip(size_t count);

                virtual status_t    error();

                virtual status_t    close();
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IO_INPUTSTREAMREADER_H_ */
