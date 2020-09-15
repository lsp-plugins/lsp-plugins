/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 27 июл. 2017 г.
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

#ifndef UI_CTL_CTLAXIS_H_
#define UI_CTL_CTLAXIS_H_

namespace lsp
{
    namespace ctl
    {
        class CtlAxis: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                enum flags_t
                {
                    F_MIN_SET       = 1 << 0,
                    F_MAX_SET       = 1 << 1,
                    F_LOG_SET       = 1 << 2,
                    F_LOG           = 1 << 3
                };

            protected:
                size_t          nFlags;
                CtlExpression   sMin;
                CtlExpression   sMax;
                CtlExpression   sAngle;
                CtlExpression   sLength;
                CtlExpression   sDX;
                CtlExpression   sDY;
                CtlPort        *pPort;
                CtlColor        sColor;

            protected:
                static status_t     slot_graph_resize(LSPWidget *sender, void *ptr, void *data);

                void                update_axis();
                void                trigger_expr();
                float               eval_expr(CtlExpression *expr);

            public:
                explicit CtlAxis(CtlRegistry *src, LSPAxis *axis);
                virtual ~CtlAxis();

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

#endif /* UI_CTL_CTLAXIS_H_ */
