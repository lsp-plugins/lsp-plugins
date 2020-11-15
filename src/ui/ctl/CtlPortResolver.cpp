/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 28 сент. 2019 г.
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
        CtlPortResolver::CtlPortResolver()
        {
            pRegistry   = NULL;
        }
        
        CtlPortResolver::~CtlPortResolver()
        {
            pRegistry   = NULL;
        }
        
        void CtlPortResolver::init(CtlRegistry *registry)
        {
            pRegistry   = registry;
        }

        status_t CtlPortResolver::resolve(calc::value_t *value, const char *name, size_t num_indexes, const ssize_t *indexes)
        {
            LSPString path;
            if (!path.set_utf8(name))
                return STATUS_NO_MEM;
            for (size_t i=0; i<num_indexes; ++i)
                if (!path.fmt_append_utf8("_%d", int(indexes[i])))
                    return STATUS_NO_MEM;

            CtlPort *p      = (pRegistry != NULL) ? pRegistry->port(path.get_utf8()) : NULL;
            if (p == NULL)
                return STATUS_NOT_FOUND;

            value->type     = calc::VT_FLOAT;
            value->v_float  = p->get_value();

            return on_resolved(&path, p);
        }

        status_t CtlPortResolver::resolve(calc::value_t *value, const LSPString *name, size_t num_indexes, const ssize_t *indexes)
        {
            LSPString path;
            if (num_indexes > 0)
            {
                if (!path.set(name))
                    return STATUS_NO_MEM;
                for (size_t i=0; i<num_indexes; ++i)
                    if (!path.fmt_append_utf8("_%d", int(indexes[i])))
                        return STATUS_NO_MEM;
                name = &path;
            }

            CtlPort *p      = (pRegistry != NULL) ? pRegistry->port(name->get_utf8()) : NULL;
            if (p == NULL)
                return STATUS_NOT_FOUND;

            value->type     = calc::VT_FLOAT;
            value->v_float  = p->get_value();

            return on_resolved(name, p);
        }

        status_t CtlPortResolver::on_resolved(const LSPString *name, CtlPort *p)
        {
            return on_resolved(name->get_utf8(), p);
        }

        status_t CtlPortResolver::on_resolved(const char *name, CtlPort *p)
        {
            return STATUS_OK;
        }
    
    } /* namespace java */
} /* namespace lsp */
