/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 19 нояб. 2017 г.
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

#ifndef UI_CTL_CTLFADER_H_
#define UI_CTL_CTLFADER_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlFader: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                bool                bLog;
                float               fDefaultValue;
                CtlPort            *pPort;

            protected:
                static status_t     slot_change(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_dbl_click(LSPWidget *sender, void *ptr, void *data);

            protected:
                void                submit_value();
                void                set_default_value();
                void                commit_value(float value);

            public:
                explicit CtlFader(CtlRegistry *src, LSPFader *widget);
                virtual ~CtlFader();

            public:
                virtual void init();

                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLFADER_H_ */
