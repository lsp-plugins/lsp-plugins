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
    Axis::Axis(plugin_ui *ui):
        IGraphObject(ui)
    {
        nFlags          = F_BASIS | F_VISIBLE;
        fDX             = 1.0;
        fDY             = 0.0;
        fMin            = -1.0;
        fMax            = 1.0;
        sColor.set(ui->theme(), C_GRAPH_AXIS);
        pPort           = NULL;
        nWidth          = 1;
    }

    Axis::~Axis()
    {
    }

    void Axis::draw(IGraphCanvas *cv)
    {
        if (!(nFlags & F_VISIBLE))
            return;

        float x1, y1, x2, y2;

        if (!clip_line2d(fDX, fDY, cv->left(), cv->right(), cv->top(), cv->bottom(), x1, y1, x2, y2))
            return;

        cv->set_color(sColor);
        cv->set_line_width(nWidth);
        cv->line(x1, y1, x2, y2);
    }

    void Axis::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_ID:
                pPort       = pUI->port(value);
                if (pPort != NULL)
                    pPort->bind(this);
                break;
            case A_COLOR:
                sColor.set(pUI->theme(), value);
                break;
            case A_ANGLE:
                PARSE_DOUBLE(value,
                    fDX = cos(__ * M_PI);
                    fDY = sin(__ * M_PI);
                );
                break;
            case A_MIN:
                PARSE_FLOAT(value, fMin = __);
                nFlags      |= F_MIN;
                break;
            case A_MAX:
                PARSE_FLOAT(value, fMax = __);
                nFlags      |= F_MAX;
                break;
            case A_BASIS:
                PARSE_BOOL(value,
                    if (__)
                        nFlags |= F_BASIS;
                    else
                        nFlags &= ~F_BASIS;
                );
                break;
            case A_VISIBLE:
                PARSE_BOOL(value,
                    if (__)
                        nFlags |= F_VISIBLE;
                    else
                        nFlags &= ~F_VISIBLE;
                );
                break;
            case A_WIDTH:
                PARSE_INT(value, nWidth = __);
                break;
            default:
                IWidget::set(att, value);
                break;
        }
    }

    bool Axis::isBasis()
    {
        return nFlags & F_BASIS;
    }

    bool Axis::apply(IGraphCanvas *cv, float &x, float &y, float value)
    {
        float x1, y1, x2, y2;
        if (!clip_line2d(fDX, fDY, cv->left() + 1, cv->right() - 1, cv->top() - 1, cv->bottom() + 1, x1, y1, x2, y2))
            return false;

        float d1    = distance2d(0.0f, 0.0f, x1, y1), d2 = distance2d(0.0f, 0.0f, x2, y2);
        float d     = (d1 > d2) ? d1 : d2;

        // Normalize value according to minimum and maximum visible values of the axis
        float a_min = fabs(actualMin()), a_max = fabs(actualMax());
        float norm = (a_min > a_max) ? a_min : a_max;
        if (norm == 0.0f)
            return false;
        norm        = d / norm;

        // Now we can surely apply deltas
        x      += fDX * value * norm;
        y      += fDY * value * norm;
        return true;
    }

    void Axis::update()
    {
    }

    bool Axis::parallel(float x, float y, float &a, float &b, float &c)
    {
        return locate_line2d(fDX, fDY, x, y, a, b, c);
    }

    float Axis::actualMin()
    {
        if (pPort == NULL)
            return fMin;

        const port_t *mdata = pPort->metadata();
        if (mdata == NULL)
            return fMin;

        return (nFlags & F_MIN) ? pPort->getValue() * fMin : mdata->min;
    }

    float Axis::actualMax()
    {
        if (pPort == NULL)
            return fMax;

        const port_t *mdata = pPort->metadata();
        if (mdata == NULL)
            return fMax;

        return (nFlags & F_MAX) ? pPort->getValue() * fMax : mdata->max;
    }

} /* namespace lsp */
