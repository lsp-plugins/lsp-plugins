/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 14 нояб. 2017 г.
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

#ifndef CORE_IO_IOUTSTREAM_H_
#define CORE_IO_IOUTSTREAM_H_

#include <core/types.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        class IOutStream
        {
            private:
                IOutStream & operator = (const IOutStream &);

            protected:
                status_t        nErrorCode;

            protected:
                inline status_t set_error(status_t error) { return nErrorCode = error; }

            public:
                explicit IOutStream();
                virtual ~IOutStream();

            public:
                /** Get last I/O error code
                 *
                 * @return last I/O error code
                 */
                inline status_t     last_error() const { return nErrorCode; };

                /** The current read position
                 *
                 * @return current read position or error code
                 */
                virtual wssize_t    position();

                /** Write the data to output stream.
                 * The implementation should write the most possible amount
                 * of bytes before exit. Such behaviour will simplify the caller's
                 * implementation.
                 *
                 * @param buf buffer to write
                 * @param count number of bytes
                 * @return number of bytes actually written
                 */
                virtual ssize_t     write(const void *buf, size_t count);

                /** Seek the stream to the specified position from the beginning
                 *
                 * @param position the specified position
                 * @return real position or negative value on error
                 */
                virtual wssize_t    seek(wsize_t position);

                /**
                 * Flush buffers to underlying storage
                 * @return status of operation
                 */
                virtual status_t    flush();

                /** Close the clip data stream
                 *
                 * @return status of operation
                 */
                virtual status_t    close();
        };
    
    } /* namespace ws */
} /* namespace lsp */

#endif /* CORE_IO_IOUTSTREAM_H_ */
