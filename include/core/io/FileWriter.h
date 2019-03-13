/*
 * FileWriter.h
 *
 *  Created on: 18 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_IO_FILEWRITER_H_
#define CORE_IO_FILEWRITER_H_

#include <stdio.h>
#include <core/types.h>
#include <core/io/charset.h>
#include <core/io/Path.h>
#include <core/io/File.h>
#include <core/io/Writer.h>
#include <core/io/CharsetEncoder.h>

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
                File           *pFD;
                size_t          nWrapFlags;
                CharsetEncoder  sEncoder;
    
            protected:
                status_t        flush_buffer(bool force);
                status_t        flush_byte_buffer();

            private:
                FileWriter & operator = (const FileWriter &);

            public:
                explicit FileWriter();
                virtual ~FileWriter();

            public:
                status_t wrap(FILE *fd, bool close, const char *charset = NULL);

                status_t wrap(lsp_fhandle_t fd, bool close, const char *charset = NULL);

                status_t wrap(File *fd, size_t flags, const char *charset = NULL);

                status_t open(const char *path, size_t mode, const char *charset = NULL);

                status_t open(const LSPString *path, size_t mode, const char *charset = NULL);

                status_t open(const Path *path, size_t mode, const char *charset = NULL);

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
