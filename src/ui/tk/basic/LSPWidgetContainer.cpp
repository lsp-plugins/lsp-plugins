/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 16 июн. 2017 г.
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

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPWidgetContainer::metadata = { "LSPWidgetContainer", &LSPComplexWidget::metadata };

        LSPWidgetContainer::LSPWidgetContainer(LSPDisplay *dpy): LSPComplexWidget(dpy)
        {
            pClass          = &metadata;
        }

        LSPWidgetContainer::~LSPWidgetContainer()
        {
        }

        status_t LSPWidgetContainer::add(LSPWidget *child)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        status_t LSPWidgetContainer::remove(LSPWidget *child)
        {
            return STATUS_NOT_FOUND;
        }

        status_t LSPWidgetContainer::remove_all()
        {
            return STATUS_NOT_IMPLEMENTED;
        }
    } /* namespace tk */
} /* namespace lsp */
