/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 окт. 2019 г.
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

#ifndef UI_CTL_CTLSCROLLBOX_H_
#define UI_CTL_CTLSCROLLBOX_H_

namespace lsp
{
    namespace ctl
    {
        class CtlScrollBox: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                ssize_t     nOrientation;

            public:
                explicit CtlScrollBox(CtlRegistry *src, LSPScrollBox *widget, ssize_t orientation = -1);
                virtual ~CtlScrollBox();

            public:
                /** Set attribute
                 *
                 * @param att widget attribute
                 * @param value widget value
                 */
                virtual void set(widget_attribute_t att, const char *value);

                /** Add child widget
                 *
                 * @param child child widget to add
                 */
                virtual status_t add(CtlWidget *child);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLSCROLLBOX_H_ */
