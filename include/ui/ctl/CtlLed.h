/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 10 июл. 2017 г.
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

#ifndef UI_CTL_CTLLED_H_
#define UI_CTL_CTLLED_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlLed: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlColor            sColor;
                CtlPort            *pPort;
                float               fValue;
                float               fKey;
                CtlExpression       sActivity;
                bool                bActivitySet;
                bool                bInvert;

            protected:
                void        update_value();

            public:
                explicit CtlLed(CtlRegistry *src, LSPLed *widget);
                virtual ~CtlLed();

            public:
                /** Begin initialization of controller
                 *
                 */
                virtual void init();

                virtual void destroy();

                /** Set attribute to widget
                 *
                 * @param att attribute identifier
                 * @param value attribute value
                 */
                virtual void set(widget_attribute_t att, const char *value);

                /** Notify controller about one of port bindings has changed
                 *
                 * @param port port triggered change
                 */
                virtual void notify(CtlPort *port);

                /** Complete initialization
                 *
                 */
                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLLED_H_ */
