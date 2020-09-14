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

#ifndef UI_CTL_CTLCONFIGSOURCE_H_
#define UI_CTL_CTLCONFIGSOURCE_H_

#include <ui/ctl/CtlConfigHandler.h>
#include <core/files/config/IConfigSource.h>

namespace lsp
{
    namespace ctl
    {
        class CtlConfigSource: public config::IConfigSource, public CtlPortHandler
        {
            protected:
                LSPString           sComment;

            public:
                explicit CtlConfigSource();
                virtual ~CtlConfigSource();

            public:
                status_t set_comment(const char *comment);
                status_t set_comment(const LSPString *comment);

            public:
                virtual status_t get_head_comment(LSPString *comment);

                virtual status_t get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCONFIGSOURCE_H_ */
