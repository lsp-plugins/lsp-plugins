/*
 * Axis.cpp
 *
 *  Created on: 27 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/graphics.h>

#include <math.h>

#define DOT_LOG_BASE            1.0f
#define DOT_DB_BASE             0.025f

namespace lsp
{
    Dot::Dot(plugin_ui *ui):
        IGraphObject(ui, W_DOT)
    {
        pLeft       = NULL;
        pTop        = NULL;
        pScroll     = NULL;

        fLeft       = 0.0f;
        fTop        = 0.0f;
        fScroll     = 0.0f;
        nCenter     = 0;
        nFlags      = 0;
        nBasisID    = 0;
        nParallelID = 1;
        nRealX      = -1;
        nRealY      = -1;
        fLastLeft   = -1;
        fLastTop    = -1;
        nMouseX     = -1;
        nMouseY     = -1;
        nMouseBtn   = 0;

        sColor.init(this, C_GRAPH_MESH, A_COLOR, -1, -1, -1, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
    }

    Dot::~Dot()
    {
    }

    void Dot::draw(IGraphCanvas *cv)
    {
        if (!bVisible)
            return;

        float left  = fLeft;
        float top   = fTop;

        // Get axises
        Axis *basis     = pGraph->axis(nBasisID);
        if (basis == NULL)
            return;
        Axis *parallel  = pGraph->axis(nParallelID);
        if (parallel == NULL)
            return;

        // Locate the point at the center
        float x = 0.0f, y = 0.0f;
        cv->center(nCenter, &x, &y);

        // Translate point and get the owner line
        basis->apply(cv, &x, &y, &left, 1);
        parallel->apply(cv, &x, &y, &top, 1);

        // Store real coordinates
        nRealX  = x;
        nRealY  = y;

        // Draw the dot
        if (nFlags & F_EDITABLE)
        {
            Color c1(sColor.color()), c2(sColor.color());

            bool aa = cv->set_anti_aliasing(true);
            c2.alpha(0.9);
            if (nFlags & F_HIGHLIGHT)
                cv->radial_gradient(x, y, c1, c2, 16);
            else
                cv->radial_gradient(x, y, c1, c2, 12);
            cv->set_anti_aliasing(aa);

            cv->set_color_rgb(0, 0, 0);
            cv->circle(x, y, 4);
        }

        cv->set_color(sColor.color());
        cv->circle(x, y, 3);
    }

    void Dot::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_VPOS:
                PARSE_FLOAT(value, fTop = __);
                break;
            case A_HPOS:
                PARSE_FLOAT(value, fLeft = __);
                break;

            case A_BASIS:
                PARSE_INT(value, nBasisID = __);
                break;
            case A_PARALLEL:
                PARSE_INT(value, nParallelID = __);
                break;
            case A_CENTER:
                PARSE_INT(value, nCenter = __);
                break;

            case A_SCROLL_ID:
                BIND_PORT(pUI, pScroll, value);
                break;
            case A_VPOS_ID:
                BIND_PORT(pUI, pTop, value);
                break;
            case A_HPOS_ID:
                BIND_PORT(pUI, pLeft, value);
                break;

            case A_EDITABLE:
                PARSE_FLAG(value, nFlags, F_EDITABLE);
                break;

            default:
                if (sColor.set(att, value))
                    break;
                IGraphObject::set(att, value);
                break;
        }
    }

    void Dot::notify(IUIPort *port)
    {
        if (port == pLeft)
            fLeft   = pLeft->getValue();
        else if (port == pTop)
            fTop    = pTop->getValue();
        sColor.notify(port);
        IGraphObject::notify(port);
    }

    void Dot::end()
    {
        if (pScroll != NULL)
            fScroll     = pScroll->getValue();
        if (pLeft != NULL)
            fLeft       = pLeft->getValue();
        if (pTop != NULL)
            fTop        = pTop->getValue();
    }

    bool Dot::mouse_over(ssize_t x, ssize_t y, size_t &flags)
    {
        if (!(nFlags & F_EDITABLE))
            return false;

        bool mover  = false;
        if (bVisible)
        {
            float dx    = x-nRealX, dy = y-nRealY;
            float R     = 8; // TODO: make configurable
            mover       = (dx*dx + dy*dy) <= R*R;
        }

        if ((!mover) && (nFlags & F_HIGHLIGHT))
        {
            flags       |= EVF_HANDLED;
            nFlags      &= ~F_HIGHLIGHT;
        }
        else if ((mover) && (!(nFlags & F_HIGHLIGHT)))
        {
            flags       |= EVF_HANDLED;
            nFlags      |= F_HIGHLIGHT;
        }

        return mover;
    }

    size_t Dot::button_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        size_t flags = EVF_NONE;

        if (nMouseBtn == 0)
        {
            if (!mouse_over(x, y, flags))
                return flags;

            if ((button == MCB_LEFT) || (button == MCB_RIGHT))
            {
                nMouseX     = x;
                nMouseY     = y;
                fLastLeft   = fLeft;
                fLastTop    = fTop;
                nFlags     |= F_EDITING;
                if (button == MCB_RIGHT)
                    nFlags     |= F_FINE_TUNE;
            }
        }

        flags      |= EVF_HANDLED | EVF_GRAB;
        button      = 1 << button;
        nMouseBtn  |= button;

        size_t bflag    = (nFlags & F_FINE_TUNE) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);
        if (nMouseBtn == bflag)
            apply_motion(x, y);
        else
            apply_motion(nMouseX, nMouseY);

        return flags;
    }

    size_t Dot::button_release(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        size_t flags    = EVF_NONE;
        if ((!(nFlags & F_EDITING)) || (nMouseBtn == 0))
            return flags;

        button          = 1 << button;
        size_t bflag    = (nFlags & F_FINE_TUNE) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);

        flags          |= EVF_HANDLED;
        nMouseBtn      &= ~button;

        if (nMouseBtn != 0)
        {
            if (nMouseBtn == bflag)
                apply_motion(x, y);
            else
                apply_motion(nMouseX, nMouseY);

            flags      |= EVF_GRAB;
        }
        else
        {
            if (button == bflag)
                apply_motion(x, y);
            else
                apply_motion(nMouseX, nMouseY);

            nFlags     &= ~F_FINE_TUNE;
            // Check that mouse is still over
            mouse_over(x, y, flags);
        }

        return flags;
    }

    size_t Dot::motion(ssize_t x, ssize_t y, size_t state)
    {
        size_t flags = EVF_NONE;

        if (nMouseBtn == 0)
        {
            if (!mouse_over(x, y, flags))
                return flags;
        }
        else
        {
            size_t bflag    = (nFlags & F_FINE_TUNE) ? (1 << MCB_RIGHT) : (1 << MCB_LEFT);

            if (nMouseBtn == bflag)
                apply_motion(x, y);
            else
                apply_motion(nMouseX, nMouseY);
            flags  |= EVF_HANDLED;
        }

        flags   |= EVF_GRAB;

        return flags;
    }

    void Dot::apply_motion(ssize_t x, ssize_t y)
    {
        // Get canvas
        IGraphCanvas *cv = pGraph->canvas();
        if (cv == NULL)
            return;

        // Get axises
        Axis *basis     = pGraph->axis(nBasisID);
        if (basis == NULL)
            return;
        Axis *parallel  = pGraph->axis(nParallelID);
        if (parallel == NULL)
            return;

        // Update the difference relative to the sensitivity
        float rx = x, ry = y;
        if (nFlags & F_FINE_TUNE)
        {
            float dx = x - nMouseX, dy = y - nMouseY;
            rx      = nMouseX + 0.1 * dx;
            ry      = nMouseY + 0.1 * dy;
        }

        // Modify the value according to X coordinate
        if ((nMouseX == x) && (pLeft != NULL))
        {
            pLeft->setValue(fLastLeft);
            pLeft->notifyAll();
        }
        else if (basis != NULL)
        {
            float value = basis->project(cv, rx, ry);
            pLeft->setValue(value);
            pLeft->notifyAll();
        }

        // Modify the value according to Y  coordinate
        if ((nMouseY == y) && (pTop != NULL))
        {
            pTop->setValue(fLastTop);
            pTop->notifyAll();
        }
        else if (parallel != NULL)
        {
            float value = parallel->project(cv, rx, ry);
            pTop->setValue(value);
            pTop->notifyAll();
        }
    }

    size_t Dot::scroll(ssize_t x, ssize_t y, size_t state, size_t direction)
    {
        // Process generic mouse event
        size_t flags = EVF_NONE;
        if (!mouse_over(x, y, flags))
            return flags;
        else if (pScroll == NULL)
            return flags;

        // Get port metadata
        const port_t *p = pScroll->metadata();
        if (p == NULL)
            return flags;

        // Determine port parameters
        float min = 0.0f, max = 1.0f, step = 0.01f;
        get_port_parameters(p, &min, &max, NULL);
        float tolerance = (state & MCF_SHIFT) ? 0.1f : 1.0f;

        if (!(is_discrete_unit(p->unit) || (p->flags & F_INT)))
        {
            step = 1.0;
            if ((is_decibel_unit(p->unit)) || (p->flags & F_LOG))
                tolerance *= 0.25f;

            step *= tolerance;
        }

        // Update value
        float delta = 0.0;
        if (direction == MCD_UP)
            delta   = step;
        else if (direction == MCD_DOWN)
            delta   = -step;
        else
            return flags;

        float value     = pScroll->getValue();

        // Analyze metadata
        if (is_decibel_unit(p->unit)) // Decibels
        {
            // Translate value to decibels, update and translate back
            double mul  = (p->unit == U_GAIN_AMP) ? 20.0 : 10.0;
            if (max > 0.0)
            {
                value       = mul * log(value + DOT_DB_BASE) / M_LN10;
                value      += delta;
                value       = exp(value * M_LN10 / mul) - DOT_DB_BASE;
            }
            else
            {
                value       = mul * log(- value + DOT_DB_BASE) / M_LN10;
                value      += delta;
                value       = - exp(value * M_LN10 / mul) - DOT_DB_BASE;
            }
        }
        else if (is_discrete_unit(p->unit)) // Integer type
        {
            value       = truncf(value + delta);
        }
        else // Float and other values
        {
            if (p->flags & F_LOG)
            {
                if (max > 0.0)
                {
                    value       = log(value + DOT_LOG_BASE);
                    value      += delta;
                    value       = exp(value) - DOT_LOG_BASE;
                }
                else
                {
                    value       = log(- value + DOT_LOG_BASE);
                    value      += delta;
                    value       = - exp(value) - DOT_LOG_BASE;
                }
            }
            else
                value      += delta;
        }

        // Update port's value
        pScroll->setValue(float(value));
        pScroll->notifyAll();

        // Mark mouse event processed
        flags      |= EVF_STOP | EVF_HANDLED;
        return flags;
    }


} /* namespace lsp */
