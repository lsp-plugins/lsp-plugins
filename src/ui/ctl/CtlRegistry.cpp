/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 23 июн. 2017 г.
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

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlRegistry::CtlRegistry()
        {
        }
        
        CtlRegistry::~CtlRegistry()
        {
            destroy();
        }

        void CtlRegistry::destroy()
        {
            for (size_t i=0, n=vControls.size(); i<n; ++i)
            {
                CtlWidget *w = vControls.at(i);
                if (w != NULL)
                {
                    w->destroy();
                    delete w;
                }
            }
            vControls.flush();
        }

        CtlPort *CtlRegistry::port(const char *name)
        {
            return NULL;
        }

        status_t CtlRegistry::add_widget(CtlWidget *widget)
        {
            return (vControls.add(widget)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t CtlRegistry::remove_widget(CtlWidget *widget)
        {
            return (vControls.remove(widget, true)) ? STATUS_OK : STATUS_NOT_FOUND;
        }

        LSPWidget *CtlRegistry::resolve(const char *uid)
        {
            LSPWidget *res = NULL;

            for (size_t i=0, n=vControls.size(); i<n; ++i)
            {
                CtlWidget *w = vControls.at(i);
                if (w == NULL)
                    continue;
                if ((res = w->resolve(uid)) != NULL)
                    break;
            }

            return res;
        }

        KVTStorage *CtlRegistry::kvt_lock()
        {
            return NULL;
        }

        KVTStorage *CtlRegistry::kvt_trylock()
        {
            return NULL;
        }

        void CtlRegistry::kvt_release()
        {
        }

        void CtlRegistry::kvt_write(KVTStorage *storage, const char *id, const kvt_param_t *value)
        {
        }

        status_t CtlRegistry::add_kvt_listener(CtlKvtListener *listener)
        {
            return STATUS_NOT_IMPLEMENTED;
        }
    
    } /* namespace tk */
} /* namespace lsp */
