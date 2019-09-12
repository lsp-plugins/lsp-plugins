/*
 * CtlScrollBar.cpp
 *
 *  Created on: 4 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlScrollBar::metadata = { "CtlScrollBar", &CtlWidget::metadata };

        CtlScrollBar::CtlScrollBar(CtlRegistry *src, LSPScrollBar *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
        }

        CtlScrollBar::~CtlScrollBar()
        {
        }

    } /* namespace ctl */
} /* namespace lsp */
