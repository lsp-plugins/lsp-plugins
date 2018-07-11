/*
 * IWidget.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>

namespace lsp
{
    IWidget::IWidget(plugin_ui *ui, widget_t w_class)
    {
        pUI     = ui;
        enClass = w_class;
    }

    IWidget::~IWidget()
    {
    }

    void IWidget::set(const char *name, const char *value)
    {
        widget_attribute_t att = widget_attribute(name);
        if (att != A_UNKNOWN)
            set(att, value);
    }

    void IWidget::set(widget_attribute_t att, const char *value)
    {
    }

    void IWidget::add(IWidget *widget)
    {
    }

    void IWidget::notify(IUIPort *port)
    {
    }

    void IWidget::begin()
    {
    }

    void IWidget::end()
    {
    }

} /* namespace lsp */
