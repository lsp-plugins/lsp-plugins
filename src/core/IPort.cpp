/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 22 окт. 2015 г.
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

#include <math.h>
#include <stddef.h>

#include <core/IPort.h>

namespace lsp
{
    
    IPort::IPort(const port_t *meta)
    {
        pMetadata       = meta;
    }
    
    IPort::~IPort()
    {
    }

    float IPort::getValue()
    {
        return (pMetadata != NULL) ? pMetadata->start : NAN;
    }

    void IPort::setValue(float value)
    {
    }

    void *IPort::getBuffer()
    {
        return NULL;
    }

    bool IPort::pre_process(size_t samples)
    {
        return false;
    }

    void IPort::post_process(size_t samples)
    {
    }

} /* namespace lsp */
