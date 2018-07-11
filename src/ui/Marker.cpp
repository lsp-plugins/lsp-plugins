/*
 * Marker.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/graphics.h>

#include <math.h>


namespace lsp
{
    Marker::Marker(plugin_ui *ui):
        IGraphObject(ui, W_MARKER)
    {
        nBasisID    = 0;
        nParallelID = 1;
        fValue      = 0;
        sColor.set_rgb(1.0, 1.0, 1.0);
        pPort       = NULL;
        nWidth      = 1;
    }

    Marker::~Marker()
    {
    }

    void Marker::draw(IGraphCanvas *cv)
    {
        if (pGraph == NULL)
            return;

        float value     = (pPort != NULL) ? pPort->getValue() : fValue;

        Axis *basis     = pGraph->axis(nBasisID);
        if (basis == NULL)
            return;
        Axis *parallel  = pGraph->axis(nParallelID);
        if (parallel == NULL)
            return;

        float x = 0.0f, y = 0.0f;
        float a, b, c;

        // Translate point and get the owner line
        basis->apply(cv, x, y, value);
        if (!parallel->parallel(x, y, a, b, c))
            return;

        // Clip line
        float x1, y1, x2, y2;
        if (!clip_line2d(a, b, c, cv->left(), cv->right(), cv->top(), cv->bottom(), x1, y1, x2, y2))
            return;

        // Draw line
        cv->set_color(sColor);
        cv->set_line_width(nWidth);
        cv->line(x1, y1, x2, y2);
    }

    void Marker::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                pPort       = pUI->port(value);
                if (pPort != NULL)
                    pPort->bind(this);
                break;
            case A_VALUE:
                PARSE_FLOAT(value, fValue = __);
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_BASIS:
                PARSE_INT(value, nBasisID = __);
                break;
            case A_PARALLEL:
                PARSE_INT(value, nParallelID = __);
                break;
            case A_WIDTH:
                PARSE_INT(value, nWidth = __);
                break;
            default:
                IWidget::set(att, value);
                break;
        }
    }

} /* namespace lsp */
