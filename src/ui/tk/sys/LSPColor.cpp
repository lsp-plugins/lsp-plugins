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
            pStyle      = NULL;
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
            pColor      = NULL;
            pStyle      = NULL;
        }

        void LSPColor::Listener::unbind()
        {
            LSPStyle *style = pStyle;
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

            LISTENER_UNBIND(aA);

            LISTENER_UNBIND(aRGB);
            LISTENER_UNBIND(aRGBA);
            LISTENER_UNBIND(aHSL);
            LISTENER_UNBIND(aHSLA);
            #undef LISTENER_UNBIND

            pStyle  = NULL;
        }
        
        void LSPColor::Listener::reset()
        {
            LSPStyle *style = pStyle;
            if (style == NULL)
                return;

            style->begin();

            #define LISTENER_RESET(var) if (var >= 0) style->set_default(var);
            LISTENER_RESET(aR);
            LISTENER_RESET(aG);
            LISTENER_RESET(aB);

            LISTENER_RESET(aH);
            LISTENER_RESET(aS);
            LISTENER_RESET(aL);

            LISTENER_RESET(aA);

            LISTENER_RESET(aRGB);
            LISTENER_RESET(aRGBA);
            LISTENER_RESET(aHSL);
            LISTENER_RESET(aHSLA);
            #undef LISTENER_UNBIND

            style->end();
        }

        status_t LSPColor::Listener::bind(LSPDisplay *dpy, LSPStyle *style, const char *property)
        {
            if (pStyle == style)
                return STATUS_OK;

            // Unbind from previously used style
            unbind();

            LSPString tmp;
            if (!tmp.set_utf8(property))
                return STATUS_NO_MEM;
            size_t len = tmp.length();

            ui_atom_t atom = -1;
            status_t res = STATUS_OK;

            style->begin();

            #define LISTENER_BIND(var, ptype, postfix) \
                if (res == STATUS_OK) \
                { \
                    res = (tmp.append_utf8(postfix)) ? STATUS_OK : STATUS_NO_MEM; \
                    if ((res == STATUS_OK) && ((atom = dpy->atom_id(tmp.get_utf8())) < 0)) \
                        res = -atom; \
                    if (res == STATUS_OK) \
                        res = style->bind(atom, ptype, this); \
                    var = atom; \
                    tmp.set_length(len); \
                }

            // Perform binding
            LISTENER_BIND(aR, PT_FLOAT, ".red");
            LISTENER_BIND(aG, PT_FLOAT, ".green");
            LISTENER_BIND(aB, PT_FLOAT, ".blue");

            LISTENER_BIND(aH, PT_FLOAT, ".hue");
            LISTENER_BIND(aS, PT_FLOAT, ".sat");
            LISTENER_BIND(aL, PT_FLOAT, ".light");

            LISTENER_BIND(aA, PT_FLOAT, ".alpha");

            LISTENER_BIND(aRGB, PT_STRING, ".rgb");
            LISTENER_BIND(aRGBA, PT_STRING, ".rgba");
            LISTENER_BIND(aHSL, PT_STRING, ".hsl");
            LISTENER_BIND(aHSLA, PT_STRING, ".hsla");

            #undef LISTENER_BIND

            if (res != STATUS_OK)
                unbind();
            else
                pStyle = style;

            style->end();

            return res;
        }

        void LSPColor::Listener::sync()
        {
            LSPStyle *style = pStyle;
            if (style == NULL)
                return;
            if (pColor == NULL)
                return;
            Color &c = pColor->sColor;

            char buf[32];
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
            LSPStyle *style = pStyle;
            if (style == NULL)
                return;
            Color *c = (pColor != NULL) ? &pColor->sColor : NULL;
            if (c == NULL)
                return;

            float v;
            style->begin();

            if ((property == aR) && (style->get_float(aR, &v) == STATUS_OK))
                c->red(v);
            if ((property == aG) && (style->get_float(aG, &v) == STATUS_OK))
                c->green(v);
            if ((property == aB) && (style->get_float(aB, &v) == STATUS_OK))
                c->blue(v);

            if ((property == aH) && (style->get_float(aH, &v) == STATUS_OK))
                c->hue(v);
            if ((property == aS) && (style->get_float(aS, &v) == STATUS_OK))
                c->saturation(v);
            if ((property == aL) && (style->get_float(aL, &v) == STATUS_OK))
                c->lightness(v);

            if ((property == aA) && (style->get_float(aA, &v) == STATUS_OK))
                c->alpha(v);

            // TODO: add textual properties configuration

            style->end();

            if (pColor != NULL)
            {
                pColor->color_changed();
                if (pColor->pWidget != NULL)
                    pColor->pWidget->query_draw();
            }
        }

        LSPColor::LSPColor(): sListener(this)
        {
            pWidget     = NULL;
        }

        LSPColor::LSPColor(LSPWidget *widget): sListener(this)
        {
            pWidget     = widget;
        }

        status_t LSPColor::bind(const char *property)
        {
            if (property == NULL)
                return STATUS_BAD_ARGUMENTS;
            if (pWidget == NULL)
                return STATUS_BAD_STATE;
            return sListener.bind(pWidget->display(), pWidget->style(), property);
        }

        status_t LSPColor::bind(LSPStyle *style, const char *property)
        {
            if ((property == NULL) || (style == NULL))
                return STATUS_BAD_ARGUMENTS;
            if (pWidget == NULL)
                return STATUS_BAD_STATE;

            return sListener.bind(pWidget->display(), style, property);
        }

        status_t LSPColor::bind(LSPDisplay *dpy, LSPStyle *style, const char *property)
        {
            if ((property == NULL) || (style == NULL) || (dpy == NULL))
                return STATUS_BAD_ARGUMENTS;

            return sListener.bind(dpy, style, property);
        }

        LSPColor::~LSPColor()
        {
        }

        void LSPColor::set_default()
        {
            sListener.reset();
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
