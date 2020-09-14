/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 16 авг. 2019 г.
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

#include <ui/ws/ws.h>

namespace lsp
{
    namespace ws
    {
        IDataSink::IDataSink()
        {
            nReferences     = 0;
        }
        
        IDataSink::~IDataSink()
        {
        }

        ssize_t IDataSink::open(const char *const *mime_type)
        {
            return -STATUS_NOT_IMPLEMENTED;
        }

        status_t IDataSink::write(const void *buf, size_t count)
        {
            return -STATUS_NOT_IMPLEMENTED;
        }

        status_t IDataSink::close(status_t code)
        {
            return STATUS_OK;
        }

        size_t IDataSink::acquire()
        {
            return ++nReferences;
        }

        size_t IDataSink::release()
        {
            ssize_t refs    = --nReferences;
            if (refs <= 0)
                delete this;

            return refs;
        }
    
    } /* namespace ws */
} /* namespace lsp */
