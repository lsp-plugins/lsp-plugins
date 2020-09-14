/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 1 июн. 2018 г.
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

#ifndef UI_CTL_CTLFRACTION_H_
#define UI_CTL_CTLFRACTION_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlFraction: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort            *pPort;
                CtlPort            *pDenom;
                CtlColor            sTextColor;
                CtlColor            sColor;
                float               fSig;
                float               fMaxSig;
                ssize_t             nDenomMin;
                ssize_t             nDenomMax;
                ssize_t             nNum;
                ssize_t             nDenom;

            protected:
                static status_t    slot_change(LSPWidget *sender, void *ptr, void *data);

                void update_values();
                void submit_value();

                void sync_numerator(LSPFraction *frac);

            public:
                explicit CtlFraction(CtlRegistry *src, LSPFraction *frac);
                virtual ~CtlFraction();

            public:
                virtual void set(widget_attribute_t att, const char *value);

                virtual void init();

                virtual void begin();

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLFRACTION_H_ */
