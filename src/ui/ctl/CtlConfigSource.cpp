/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 22 июн. 2018 г.
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
        CtlConfigSource::CtlConfigSource()
        {
            nPortID = 0;
        }
        
        CtlConfigSource::~CtlConfigSource()
        {
        }

        status_t CtlConfigSource::set_comment(const char *comment)
        {
            if (!sComment.set_native(comment))
                return STATUS_NO_MEM;
            return STATUS_OK;
        }

        status_t CtlConfigSource::set_comment(const LSPString *comment)
        {
            if (!sComment.set(comment))
                return STATUS_NO_MEM;
            return STATUS_OK;
        }

        status_t CtlConfigSource::get_head_comment(LSPString *comment)
        {
            return (comment->set(&sComment)) ? STATUS_OK : STATUS_NO_MEM;
        }

        status_t CtlConfigSource::get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags)
        {
            port_ref_t *ref = vPorts.get(nPortID++);
            if (ref == NULL)
                return STATUS_EOF;

            LSP_STATUS_ASSERT(format_port_value(ref->pPort, name, value, comment, flags));
            if (!name->set(&ref->sName))
                return STATUS_NO_MEM;

            return STATUS_OK;
        }
    } /* namespace ctl */
} /* namespace lsp */
