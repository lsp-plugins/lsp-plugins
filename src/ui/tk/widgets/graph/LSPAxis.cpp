/*
 * LSPAxis.cpp
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#include <math.h>

#include <ui/tk/tk.h>

#include <ui/graphics.h>
#include <dsp/dsp.h>


namespace lsp
{
    namespace tk
    {
        const w_class_t LSPAxis::metadata = { "LSPAxis", &LSPGraphItem::metadata };

        LSPAxis::LSPAxis(LSPDisplay *dpy): LSPGraphItem(dpy),
            sColor(this)
        {
            nFlags          = F_BASIS;
            fAngle          = 0.0f;
            fDX             = 1.0f;
            fDY             = 0.0f;
            fMin            = -1.0f;
            fMax            = 1.0f;
            nWidth          = 1;
            nCenter         = 0;
            nLength         = -1;
            pClass          = &metadata;

            set_smooth(false);
        }

        LSPAxis::~LSPAxis()
        {
        }

        status_t LSPAxis::init()
        {
            status_t result = LSPGraphItem::init();
            if (result != STATUS_OK)
                return result;

            if (pDisplay != NULL)
            {
                LSPTheme *theme = pDisplay->theme();

                if (theme != NULL)
                    theme->get_color(C_GRAPH_AXIS, &sColor);
            }
            return STATUS_OK;
        }

        bool LSPAxis::apply(float *x, float *y, const float *dv, size_t count)
        {
            // Get graph
            LSPGraph *cv = graph();
            if (cv == NULL)
                return false;

            float cx = 0.0f, cy = 0.0f;
            cv->center(nCenter, &cx, &cy);

            float d     = nLength;
            if (d < 0.0f)
            {
                float la, lb, lc;

                if (!locate_line2d(fDX, fDY, cx, cy, la, lb, lc))
                    return false;

                float x1, y1, x2, y2;
                if (!clip_line2d(la, lb, lc, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), x1, y1, x2, y2))
                    return false;

                float d1    = distance2d(cx, cy, x1, y1);
                float d2    = distance2d(cx, cy, x2, y2);
                d           = (d1 > d2) ? d1 : d2;
            }
            if (d > 1.0f)
                d          -= 0.5f; // Fix rounding errors

            // Normalize value according to minimum and maximum visible values of the axis
            float a_min = fabsf(fMin), a_max = fabsf(fMax);

            // Now we can surely apply deltas
            if (nFlags & F_LOGARITHMIC)
            {
                if (a_min <= 0.0f)
                    a_min   = AMPLIFICATION_THRESH;
                if (a_max <= 0.0f)
                    a_max   = AMPLIFICATION_THRESH;
                float norm = (a_min > a_max) ? logf(a_min / a_max) : logf(a_max / a_min);
                if (norm == 0.0f)
                    return false;

                norm            = d / norm;
                a_min           = 1.0f / a_min;

                dsp::axis_apply_log2(x, y, dv, a_min, norm * fDX, norm * fDY, count);
            }
            else
            {
                float norm = (a_min > a_max) ? a_min : a_max;
                if (norm == 0.0f)
                    return false;
                norm    = d / norm;

                // Apply delta-vector
                dsp::fmadd_k3(x, dv, norm * fDX, count);
                dsp::fmadd_k3(y, dv, norm * fDY, count);
            }

            // Saturate values
            dsp::saturate(x, count);
            dsp::saturate(y, count);

            return true;
        }

        float LSPAxis::project(float x, float y)
        {
            // Get graph
            LSPGraph *cv = graph();
            if (cv == NULL)
                return false;

            // Get the center of coordinates
            float cx = 0.0f, cy = 0.0f;
            cv->center(nCenter, &cx, &cy);

            // Calculate the difference relative to the center and the projection vector length
            float dx = x - cx, dy = y - cy;
            float pv = dx*fDX + dy*fDY;

            float d     = nLength;
            if (d < 0.0f)
            {
                // Now prepare the image of the line
                float la, lb, lc;
                if (!locate_line2d(fDX, fDY, cx, cy, la, lb, lc))
                    return false;

                float x1, y1, x2, y2;
                if (!clip_line2d(la, lb, lc, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), x1, y1, x2, y2))
                    return false;

                float d1    = distance2d(cx, cy, x1, y1);
                float d2    = distance2d(cx, cy, x2, y2);
                d           = (d1 > d2) ? d1 : d2;
            }
            if (d > 1.0f)
                d          -= 0.5f; // Fix rounding errors

            // Normalize value according to minimum and maximum visible values of the axis
            float a_min = fabsf(fMin), a_max = fabsf(fMax);

            // Now we can surely apply deltas
            if (nFlags & F_LOGARITHMIC)
            {
                if (a_min <= 0.0f)
                    a_min   = AMPLIFICATION_THRESH;
                if (a_max <= 0.0f)
                    a_max   = AMPLIFICATION_THRESH;
                float norm = (a_min > a_max) ? logf(a_min / a_max) : logf(a_max / a_min);
                if (norm == 0.0f)
                    return fMin;

                return expf(pv * norm / d) * ((a_min > a_max) ? a_max : a_min);
            }
            else
            {
                float norm = (a_min > a_max) ? a_min : a_max;
                if (norm == 0.0f)
                    return fMin;

                return (pv*norm/d) + ((a_min > a_max) ? a_max : a_min);
            }

            return fMin;
        }

        bool LSPAxis::parallel(float x, float y, float &a, float &b, float &c)
        {
            return locate_line2d(fDX, fDY, x, y, a, b, c);
        }

        void LSPAxis::ortogonal_shift(float x, float y, float shift, float &nx, float &ny)
        {
            // When rotating 90 degrees left, we get: dy' = dx, dx' = -dy
            nx               = x + shift * fDY;
            ny               = y - shift * fDX;
        }

        bool LSPAxis::angle(float x, float y, float angle, float &a, float &b, float &c)
        {
            float c_sin     = sinf(angle);
            float c_cos     = cosf(angle);
            float dx        = fDX*c_cos - fDY*c_sin;
            float dy        = fDX*c_sin + fDY*c_cos;

            return locate_line2d(dx, -dy, x, y, a, b, c);
        }

        void LSPAxis::rotate_shift(float x, float y, float angle, float shift, float &nx, float &ny)
        {
            float c_sin     = sinf(angle);
            float c_cos     = cosf(angle);
            float dx        = fDX*c_cos - fDY*c_sin;
            float dy        = fDX*c_sin + fDY*c_cos;

            nx              = x + shift * dy;
            ny              = y - shift * dx;
        }

        void LSPAxis::set_flag(size_t flag, bool value)
        {
            size_t flags = nFlags;
            if (value)
                flags  |= flag;
            else
                flags  &= ~flag;
            if (nFlags == flags)
                return;

            nFlags  = flags;
            query_draw();

            return;
        }

        void LSPAxis::set_min_value(float value)
        {
            if (fMin == value)
                return;
            fMin    = value;
            query_draw();
        }

        void LSPAxis::set_max_value(float value)
        {
            if (fMax == value)
                return;
            fMax    = value;
            query_draw();
        }

        void LSPAxis::set_line_width(size_t value)
        {
            if (nWidth == value)
                return;
            nWidth  = value;
            query_draw();
        }

        void LSPAxis::set_center_id(size_t value)
        {
            if (nCenter == value)
                return;
            nCenter = value;
            query_draw();
        }

        void LSPAxis::set_angle(float value)
        {
            if (fAngle == value)
                return;
            fAngle      = value;

            float dx    = 0.0001f * truncf(cosf(value) * 10000.0f);
            float dy    = -0.0001f * truncf(sinf(value) * 10000.0f);
            if ((fDX == dx) && (fDY == dy))
                return;

            fDX         = dx;
            fDY         = dy;
            query_draw();
        }

        void LSPAxis::set_direction(float dx, float dy)
        {
            fDX         = dx;
            fDY         = dy;
            fAngle      = get_angle_2d(0.0f, 0.0f, dx, dy);

            query_draw();
        }

        void LSPAxis::set_length(ssize_t value)
        {
            if (nLength == value)
                return;
            nLength     = value;
            query_draw();
        }

        void LSPAxis::render(ISurface *s, bool force)
        {
            // Get graph
            LSPGraph *cv = graph();
            if (cv == NULL)
                return;

            // Prepare palette
            Color color(sColor);
            color.scale_lightness(brightness());

            // Draw
            float cx = 0.0f, cy = 0.0f;
            cv->center(nCenter, &cx, &cy);

            float la, lb, lc;
            if (!locate_line2d(fDX, fDY, cx, cy, la, lb, lc))
                return;

            bool aa = s->set_antialiasing(bSmooth);
            s->parametric_line(la, lb, lc, cv->area_left(), cv->area_right(), cv->area_top(), cv->area_bottom(), nWidth, color);
            s->set_antialiasing(aa);
        }
    } /* namespace tk */
} /* namespace lsp */
