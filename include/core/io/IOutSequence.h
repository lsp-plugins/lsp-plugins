/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 14 июн. 2018 г.
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

#ifndef CORE_IO_IOUTSEQUENCE_H_
#define CORE_IO_IOUTSEQUENCE_H_

#include <core/types.h>
#include <core/status.h>
#include <core/LSPString.h>

namespace lsp
{
    namespace io
    {
        class IOutSequence
        {
            protected:
                status_t        nErrorCode;

            protected:
                inline status_t set_error(status_t error) { return nErrorCode = error; }

            private:
                IOutSequence & operator = (const IOutSequence &);

            public:
                explicit IOutSequence();
                virtual ~IOutSequence();

            public:
                /**
                 * Return last error code
                 * @return last error code
                 */
                inline status_t last_error() const  { return nErrorCode; };

                /**
                 * Write single character to output stream
                 * @param c character to write
                 * @return status of operation
                 */
                virtual status_t    write(lsp_wchar_t c);

                /**
                 * Write single character to output stream and trailing end-of-line
                 * @param c character to write
                 * @return status of operation
                 */
                virtual status_t    writeln(lsp_wchar_t c);

                /**
                 * Write multiple characters to output stream
                 * @param c array of characters to write
                 * @param count number of characters to write
                 * @return status of operation
                 */
                virtual status_t    write(const lsp_wchar_t *c, size_t count);

                /**
                 * Write multiple characters to output stream and end-of-line
                 * @param c array of characters to write
                 * @param count number of characters to write
                 * @return status of operation
                 */
                virtual status_t    writeln(const lsp_wchar_t *c, size_t count);
    
                /**
                 * Write null-terminated ASCII string to output
                 * @param s ASCII string to write
                 * @param count number of characters to write
                 * @return status of operation
                 */
                virtual status_t    write_ascii(const char *s);

                /**
                 * Write ASCII character sequence to output
                 * @param s ASCII string to write
                 * @param count number of characters to write
                 * @return status of operation
                 */
                virtual status_t    write_ascii(const char *s, size_t count);

                /**
                 * Write zero-terminated ASCII string to output and end-of-line
                 * @param s ASCII string to write
                 * @param count number of characters to write
                 * @return status of operation
                 */
                virtual status_t    writeln_ascii(const char *s);

                /**
                 * Write string to output stream
                 * @param s string to write
                 * @return status of operation
                 */
                virtual status_t    write(const LSPString *s);

                /**
                 * Write string to output stream and end-of-line
                 * @param s string to write
                 * @return status of operation
                 */
                virtual status_t    writeln(const LSPString *s);

                /**
                 * Write substring to output stream
                 * @param s string to write
                 * @param first index of first character to write substring from
                 * @return status of operation
                 */
                virtual status_t    write(const LSPString *s, ssize_t first);

                /**
                 * Write substring to output stream and end-of-line
                 * @param s string to write
                 * @param first index of first character to write substring from
                 * @return status of operation
                 */
                virtual status_t    writeln(const LSPString *s, ssize_t first);

                /**
                 * Write substring to output stream
                 * @param s string to write
                 * @param first index of first character to write substring from
                 * @param last index of last character to write substring
                 * @return status of operation
                 */
                virtual status_t    write(const LSPString *s, ssize_t first, ssize_t last);

                /**
                 * Write substring to output stream and end-of-line
                 * @param s string to write
                 * @param first index of first character to write substring from
                 * @param last index of last character to write substring
                 * @return status of operation
                 */
                virtual status_t    writeln(const LSPString *s, ssize_t first, ssize_t last);

                /**
                 * Flush all underlying buffers
                 * @return status of operation
                 */
                virtual status_t    flush();

                /**
                 * Close character output stream
                 * @return status of operation
                 */
                virtual status_t    close();
        };
    }

} /* namespace lsp */

#endif /* CORE_IO_IOUTSEQUENCE_H_ */
