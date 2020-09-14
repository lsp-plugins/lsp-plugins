/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 6 июн. 2019 г.
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

#ifndef UI_CTL_CTLKVTLISTENER_H_
#define UI_CTL_CTLKVTLISTENER_H_

#include <core/KVTStorage.h>

namespace lsp
{
    namespace ctl
    {
        class CtlKvtListener
        {
            public:
                explicit CtlKvtListener();
                virtual ~CtlKvtListener();

            public:
                /**
                 * Get listener name
                 * @return listener name or NULL
                 */
                virtual const char *name();

                /**
                 * Handle change of the KVT paramter
                 * @param kvt KVT storage
                 * @param id KVT parameter full path identifier
                 * @param value actual KVT parameter value
                 * @return true if listener processed the message, false if ignored
                 */
                virtual bool changed(KVTStorage *kvt, const char *id, const kvt_param_t *value);

                /**
                 * Check that parameter name matches
                 * @param id parameter identifier
                 * @return true if parameter name matches
                 */
                virtual bool match(const char *id);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLKVTLISTENER_H_ */
