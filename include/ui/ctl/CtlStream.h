/*
 * Copyright (C) 2021 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2021 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 7 февр. 2021 г.
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

#ifndef UI_CTL_CTLSTREAM_H_
#define UI_CTL_CTLSTREAM_H_

#include <core/port_data.h>

namespace lsp
{
    namespace ctl
    {
        class CtlStream: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort        *pPort;
                CtlColor        sColor;
                CtlExpression   sStrobes;
                CtlExpression   sMaxDots;
                float           fTransparency;
                ssize_t         nMaxDots;

                mesh_t         *pMesh;

            protected:
                void            trigger_expr();
                void            commit_data();

            public:
                explicit CtlStream(CtlRegistry *src, LSPMesh *mesh);
                virtual ~CtlStream();

            public:
                /** Begin initialization of controller
                 *
                 */
                virtual void init();

                /** Set attribute
                 *
                 * @param att widget attribute
                 * @param value widget value
                 */
                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };

    } /* namespace ctl */
} /* namespace lsp */



#endif /* UI_CTL_CTLSTREAM_H_ */
