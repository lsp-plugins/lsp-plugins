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
        pUI             = ui;
        enClass         = w_class;
        pVisibility     = 0;
        nVisibilityKey  = 1;
        bVisible        = true;
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

    void IWidget::update_visibility()
    {
        if (pVisibility != NULL)
        {
            const port_t *meta  = pVisibility->metadata();
            ssize_t key     = 0;
            if ((meta != NULL) && (meta->unit == U_BOOL))
                key         = (pVisibility->getValue() >= 0.5f) ? 1 : 0;
            else
                key         = pVisibility->getValue();

            bVisible        = (key == nVisibilityKey);
        }

        if (bVisible)
            show();
        else
            hide();
    }

    void IWidget::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_VISIBILITY_ID:
                BIND_PORT(pUI, pVisibility, value);
                update_visibility();
                break;
            case A_VISIBILITY_KEY:
                PARSE_INT(value, nVisibilityKey = __);
                update_visibility();
                break;
            case A_VISIBLE:
                PARSE_BOOL(value, bVisible = __);
                update_visibility();
                break;
            default:
                break;
        }
    }

    void IWidget::add(IWidget *widget)
    {
    }

    void IWidget::notify(IUIPort *port)
    {
        if (port == pVisibility)
            update_visibility();
    }

    void IWidget::begin()
    {
    }

    void IWidget::end()
    {
        if (pVisibility != NULL)
            notify(pVisibility);
    }

    void IWidget::hide()
    {
    }

    void IWidget::show()
    {
    }

    void IWidget::draw()
    {
    }

} /* namespace lsp */
