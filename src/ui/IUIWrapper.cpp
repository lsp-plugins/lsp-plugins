/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 25 апр. 2016 г.
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

#include <ui/IUIWrapper.h>

namespace lsp
{
    
    IUIWrapper::IUIWrapper()
    {
    }
    
    IUIWrapper::~IUIWrapper()
    {
    }

    void IUIWrapper::ui_activated()
    {
    }

    void IUIWrapper::ui_deactivated()
    {
    }

    KVTStorage *IUIWrapper::kvt_lock()
    {
        return NULL;
    }

    KVTStorage *IUIWrapper::kvt_trylock()
    {
        return NULL;
    }

    bool IUIWrapper::kvt_release()
    {
        return false;
    }

    void IUIWrapper::dump_state_request()
    {
    }

} /* namespace lsp */
