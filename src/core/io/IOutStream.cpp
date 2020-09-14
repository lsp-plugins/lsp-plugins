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

#include <core/io/IOutStream.h>
#include <core/status.h>

namespace lsp
{
    namespace io
    {
        
        IOutStream::IOutStream()
        {
            nErrorCode      = STATUS_OK;
        }
        
        IOutStream::~IOutStream()
        {
            nErrorCode      = STATUS_OK;
        }

        wssize_t IOutStream::position()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        ssize_t IOutStream::write(const void *buf, size_t count)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        wssize_t IOutStream::seek(wsize_t position)
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        status_t IOutStream::flush()
        {
            return - set_error(STATUS_NOT_IMPLEMENTED);
        }

        status_t IOutStream::close()
        {
            return set_error(STATUS_OK);
        }
    
    } /* namespace ws */
} /* namespace lsp */
