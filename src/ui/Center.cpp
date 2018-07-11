/*
 * Axis.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/graphics.h>

#include <math.h>


namespace lsp
{
    Center::Center(plugin_ui *ui):
        IGraphObject(ui, W_CENTER)
    {
        fLeft       = 0.0f;
        fTop        = 0.0f;
        bVisible    = false;
        sColor.set(ui->theme(), C_GRAPH_AXIS);
    }

    Center::~Center()
    {
    }

    void Center::draw(IGraphCanvas *cv)
    {
        if (!bVisible)
            return;

        float x=0.0, y=0.0;
        cv->set_color(sColor);
        cv->center(this, &x, &y);
        cv->circle(x, y, 4);
    }

    void Center::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_VPOS:
                PARSE_FLOAT(value, fTop = __);
                break;
            case A_HPOS:
                PARSE_FLOAT(value, fLeft = __);
                break;
            case A_VISIBLE:
                PARSE_BOOL(value, bVisible = __);
                break;
            default:
                IWidget::set(att, value);
                break;
        }
    }

} /* namespace lsp */
