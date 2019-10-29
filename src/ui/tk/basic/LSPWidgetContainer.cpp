/*
 *
 * LSPWidgetContainer.cpp
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
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
