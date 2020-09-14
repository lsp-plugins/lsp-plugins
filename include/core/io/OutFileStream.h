/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 мар. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef CORE_IO_OUTFILESTREAM_H_
#define CORE_IO_OUTFILESTREAM_H_

#include <core/stdlib/stdio.h>
#include <core/LSPString.h>
#include <core/io/Path.h>
#include <core/io/File.h>
#include <core/io/IOutStream.h>

namespace lsp
{
    namespace io
    {
        class OutFileStream: public IOutStream
        {
            private:
                File       *pFD;
                size_t      nWrapFlags;

            private:
                OutFileStream & operator = (const OutFileStream &);

            public:
                explicit OutFileStream();
                virtual ~OutFileStream();

            public:
                status_t wrap(FILE *fd, bool close);

                status_t wrap_native(lsp_fhandle_t fd, bool close);

                status_t wrap(File *fd, size_t flags);

                status_t open(const char *path, size_t mode);

                status_t open(const LSPString *path, size_t mode);

                status_t open(const Path *path, size_t mode);

                virtual wssize_t    position();

                virtual ssize_t     write(const void *buf, size_t count);

                virtual wssize_t    seek(wsize_t position);

                virtual status_t    flush();

                virtual status_t    close();
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IO_OUTFILESTREAM_H_ */
