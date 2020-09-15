/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 июн. 2018 г.
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


#include <core/files/config/IConfigSource.h>

namespace lsp
{
    namespace config
    {
        
        IConfigSource::IConfigSource()
        {
            init_buf(&sBuf);
        }
        
        IConfigSource::~IConfigSource()
        {
            destroy_buf(&sBuf);
        }

        status_t IConfigSource::get_head_comment(LSPString *comment)
        {
            return STATUS_NO_DATA;
        }

        status_t IConfigSource::get_head_comment(const char **comment)
        {
            LSPString c;
            LSP_STATUS_ASSERT(get_head_comment(&c));

            clear_buf(&sBuf);
            if (!append_buf(&sBuf, c.get_utf8()))
                return STATUS_NO_MEM;
            *comment    = sBuf.pString;

            return STATUS_OK;
        }
    
        status_t IConfigSource::get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags)
        {
            *flags = 0;
            return STATUS_EOF;
        }

        status_t IConfigSource::get_parameter(LSPString *name, LSPString *value, int *flags)
        {
            LSPString comment;
            return get_parameter(name, value, &comment, flags);
        }

        status_t IConfigSource::get_parameter(const char **name, const char **value, int *flags)
        {
            LSPString n, v, c;
            status_t res = get_parameter(&n, &v, &c, flags);
            if (res != STATUS_OK)
                return res;

            const char *ns = n.get_utf8(), *vs = v.get_utf8();
            if ((ns == NULL) || (vs == NULL))
                return STATUS_NO_MEM;

            size_t nl   = strlen(ns) + 1, vl = strlen(vs) + 1;
            clear_buf(&sBuf);

            if (!append_buf(&sBuf, ns, nl))
                return STATUS_NO_MEM;
            if (!append_buf(&sBuf, vs, vl))
                return STATUS_NO_MEM;

            *name       = sBuf.pString;
            *value      = &sBuf.pString[nl];

            return STATUS_OK;
        }

        status_t IConfigSource::get_parameter(const char **name, const char **value, const char **comment, int *flags)
        {
            LSPString n, v, c;
            status_t res = get_parameter(&n, &v, &c, flags);
            if (res != STATUS_OK)
                return res;

            const char *ns = n.get_utf8(), *vs = v.get_utf8(), *cs = c.get_utf8();
            if ((ns == NULL) || (vs == NULL) || (cs == NULL))
                return STATUS_NO_MEM;

            size_t nl   = strlen(ns) + 1, vl = strlen(vs) + 1, cl = strlen(cs) + 1;
            clear_buf(&sBuf);

            if (!append_buf(&sBuf, ns, nl))
                return STATUS_NO_MEM;
            if (!append_buf(&sBuf, vs, vl))
                return STATUS_NO_MEM;
            if (!append_buf(&sBuf, cs, cl))
                return STATUS_NO_MEM;

            *name       = sBuf.pString;
            *value      = &sBuf.pString[nl];
            *comment    = &sBuf.pString[nl + vl];

            return STATUS_OK;
        }
    } /* namespace config */
} /* namespace lsp */
