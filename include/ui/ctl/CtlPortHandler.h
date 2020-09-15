/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 27 июн. 2018 г.
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

#ifndef UI_CTL_CTLPORTHANDLER_H_
#define UI_CTL_CTLPORTHANDLER_H_

namespace lsp
{
    namespace ctl
    {
        class CtlPortHandler
        {
            protected:
                typedef struct port_ref_t {
                    LSPString       sName;
                    CtlPort        *pPort;
                } port_ref_t;

            protected:
                cvector<port_ref_t> vPorts;
                size_t              nPortID;

            public:
                explicit CtlPortHandler();
                virtual ~CtlPortHandler();

            public:
                status_t add_port(const char *name, CtlPort *port);
                status_t add_port(const LSPString *name, CtlPort *port);
                status_t add_port(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLPORTHANDLER_H_ */
