/*
 * LSPColor.cpp
 *
 *  Created on: 5 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        LSPColor::Listener::Listener(LSPColor *color)
        {
            pColor      = color;
            aR          = -1;
            aG          = -1;
            aB          = -1;
            aRGB        = -1;
            aH          = -1;
            aS          = -1;
            aL          = -1;
            aHSL        = -1;
            aA          = -1;
            aRGBA       = -1;
            aHSLA       = -1;
        }

        LSPColor::Listener::~Listener()
        {
            unbind();
            pColor = NULL;
        }

        void LSPColor::Listener::unbind()
        {
            if ((pColor == NULL) || (pColor->pWidget == NULL))
                return;
            LSPStyle *style = pColor->pWidget->style();
            if (style == NULL)
                return;

            // R, G, B components
            #define LISTENER_UNBIND(var) if (var >= 0) { style->unbind(var, this); var = -1; }
            LISTENER_UNBIND(aR);
            LISTENER_UNBIND(aG);
            LISTENER_UNBIND(aB);

            LISTENER_UNBIND(aH);
            LISTENER_UNBIND(aS);
            LISTENER_UNBIND(aL);

            LISTENER_UNBIND(aRGB);
            LISTENER_UNBIND(aRGBA);
            LISTENER_UNBIND(aHSL);
            LISTENER_UNBIND(aHSLA);
            #undef LISTENER_UNBIND
        }
        
        status_t LSPColor::Listener::bind(const char *property)
        {
            if ((pColor == NULL) || (pColor->pWidget == NULL))
                return STATUS_NOT_BOUND;

            LSPDisplay *dpy = pColor->pWidget->display();   // Display provides atoms
            LSPStyle *style = pColor->pWidget->style();     // Style is a target for binding
            if ((style == NULL) || (dpy == NULL))
                return STATUS_NO_DATA;

            // TODO: perform binding

            return STATUS_OK;
        }

        void LSPColor::Listener::sync()
        {
            if ((pColor == NULL) || (pColor->pWidget == NULL))
                return;

            Color &c = pColor->sColor;
            LSPStyle *style = pColor->pWidget->style();
            if (style == NULL)
                return;

            char buf[16];
            style->begin();

            // R, G, B components
            if (aR >= 0)
                style->set_float(aR, c.red());
            if (aG >= 0)
                style->set_float(aG, c.green());
            if (aB >= 0)
                style->set_float(aB, c.blue());

            // H, S, L components
            if (aH >= 0)
                style->set_float(aH, c.hue());
            if (aS >= 0)
                style->set_float(aS, c.saturation());
            if (aL >= 0)
                style->set_float(aL, c.lightness());

            // Alpha component
            if (aA >= 0)
                style->set_float(aA, c.alpha());

            // Mixed components
            if (aRGB >= 0)
            {
                c.format_rgb(buf, sizeof(buf)/sizeof(char));
                style->set_string(aRGB, buf);
            }
            if (aRGBA >= 0)
            {
                c.format_rgba(buf, sizeof(buf)/sizeof(char));
                style->set_string(aRGBA, buf);
            }
            if (aHSL >= 0)
            {
                c.format_hsl(buf, sizeof(buf)/sizeof(char));
                style->set_string(aHSL, buf);
            }
            if (aHSLA >= 0)
            {
                c.format_hsla(buf, sizeof(buf)/sizeof(char));
                style->set_string(aHSLA, buf);
            }

            style->end();
        }

        void LSPColor::Listener::notify(ui_atom_t property)
        {
            if ((pColor == NULL) || (pColor->pWidget == NULL))
                return;
            LSPStyle *style = pColor->pWidget->style();
            if (style == NULL)
                return;
        }

        LSPColor::LSPColor(): sListener(this)
        {
            pWidget     = NULL;
        }

        LSPColor::LSPColor(LSPWidget *widget): sListener(this)
        {
            pWidget     = widget;
        }

        LSPColor::~LSPColor()
        {
        }

        void LSPColor::color_changed()
        {
        }

        void LSPColor::trigger_change()
        {
            color_changed();

            if (pWidget != NULL)
                pWidget->query_draw();

            sListener.sync();
        }

        void LSPColor::red(float r)
        {
            if (sColor.red() == r)
                return;
            sColor.red(r);
            trigger_change();
        };

        void LSPColor::green(float g)
        {
            if (sColor.green() == g)
                return;
            sColor.green(g);
            trigger_change();
        };

        void LSPColor::blue(float b)
        {
            if (sColor.blue() == b)
                return;
            sColor.blue(b);
            trigger_change();
        };

        void LSPColor::hue(float h)
        {
            if (sColor.hue() == h)
                return;
            sColor.hue(h);
            trigger_change();
        };

        void LSPColor::saturation(float s)
        {
            if (sColor.saturation() == s)
                return;
            sColor.saturation(s);
            trigger_change();
        };

        void LSPColor::lightness(float l)
        {
            if (sColor.lightness() == l)
                return;
            sColor.lightness(l);
            trigger_change();
        };

        void LSPColor::alpha(float a)
        {
            if (sColor.alpha() == a)
                return;
            sColor.alpha(a);
            trigger_change();
        };

        void LSPColor::set_rgb(float r, float g, float b)
        {
            float xr, xg, xb;
            sColor.get_rgb(xr, xg, xb);
            if ((xr == r) && (xg == g) && (xb == b))
                return;

            sColor.set_rgb(r, g, b);
            trigger_change();
        }

        void LSPColor::set_rgba(float r, float g, float b, float a)
        {
            float xr, xg, xb, xa;
            sColor.get_rgba(xr, xg, xb, xa);
            if ((xr == r) && (xg == g) && (xb == b) && (xa == a))
                return;

            sColor.set_rgba(r, g, b, a);
            trigger_change();
        }

        void LSPColor::set_hsl(float h, float s, float l)
        {
            float xh, xs, xl;
            sColor.get_hsl(xh, xs, xl);
            if ((xh == h) && (xs == s) && (xl == l))
                return;

            sColor.set_hsl(h, s, l);
            trigger_change();
        }

        void LSPColor::set_hsla(float h, float s, float l, float a)
        {
            float xh, xs, xl, xa;
            sColor.get_hsla(xh, xs, xl, xa);
            if ((xh == h) && (xs == s) && (xl == l) && (xa == a))
                return;

            sColor.set_hsla(h, s, l, a);
            trigger_change();
        }


    } /* namespace tk */
} /* namespace lsp */
