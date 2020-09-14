/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 26 июн. 2018 г.
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

#ifndef UI_CTL_CTLCONFIGHANDLER_H_
#define UI_CTL_CTLCONFIGHANDLER_H_

#include <ui/ctl/CtlConfigHandler.h>
#include <core/files/config/IConfigHandler.h>

namespace lsp
{
    namespace ctl
    {
        class CtlConfigHandler: public config::IConfigHandler, public CtlPortHandler
        {
            public:
                explicit CtlConfigHandler();
                virtual ~CtlConfigHandler();

            public:
                virtual status_t handle_parameter(const LSPString *name, const LSPString *value, size_t flags);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCONFIGHANDLER_H_ */
