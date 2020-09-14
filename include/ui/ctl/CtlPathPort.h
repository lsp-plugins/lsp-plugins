/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 нояб. 2017 г.
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

#ifndef UI_CTL_CTLPATHPORT_H_
#define UI_CTL_CTLPATHPORT_H_

namespace lsp
{
    namespace ctl
    {
        class CtlPathPort: public CtlPort
        {
            protected:
                char            sPath[PATH_MAX];
                plugin_ui      *pUI;

            public:
                explicit CtlPathPort(const port_t *meta, plugin_ui *ui);
                virtual ~CtlPathPort();

            public:
                virtual void write(const void* buffer, size_t size);

                virtual void *get_buffer();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLPATHPORT_H_ */
