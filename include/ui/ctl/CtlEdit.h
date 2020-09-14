/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 6 сент. 2017 г.
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

#ifndef UI_CTL_CTLEDIT_H_
#define UI_CTL_CTLEDIT_H_

namespace lsp
{
    namespace ctl
    {
        class CtlEdit: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                LSPFileDialog      *pDialog;
                cvector<LSPWidget>  vWidgets;

            protected:
                static status_t slot_on_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_action(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_cancel(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit CtlEdit(CtlRegistry *src, LSPEdit *widget);
                virtual ~CtlEdit();

            public:
                virtual status_t on_menu_submit();
        };
    }

} /* namespace lsp */

#endif /* UI_CTL_CTLEDIT_H_ */
