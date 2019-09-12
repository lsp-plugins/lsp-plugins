/*
 * CtlColor.cpp
 *
 *  Created on: 28 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        CtlColor::CtlColor()
        {
            pRegistry       = NULL;
            pWidget         = NULL;
            pDstColor       = NULL;

            for (size_t i=0; i<C_TOTAL; ++i)
            {
                vComponents[i]  = NULL;
                vAttributes[i]  = -1;
                vValues[i]      = NULL;
            }
        }

        CtlColor::~CtlColor()
        {
            for (size_t i=0; i<C_TOTAL; ++i)
            {
                if (vValues[i] != NULL)
                    free(vValues[i]);
                vValues[i]      = NULL;
            }
        }

        void CtlColor::do_init(CtlRegistry *reg, LSPWidget *widget, Color *col, LSPColor *lcol, size_t basic, size_t r, size_t g, size_t b, size_t h, size_t s, size_t l)
        {
            pRegistry               = reg;
            pWidget                 = widget;
            pDstColor               = lcol;

            vAttributes[C_BASIC]    = basic;
            vAttributes[C_R]        = r;
            vAttributes[C_G]        = g;
            vAttributes[C_B]        = b;
            vAttributes[C_H]        = h;
            vAttributes[C_S]        = s;
            vAttributes[C_L]        = l;

            vStatic[C_ST_R]         = -1;
            vStatic[C_ST_G]         = -1;
            vStatic[C_ST_B]         = -1;
            vStatic[C_ST_H]         = -1;
            vStatic[C_ST_S]         = -1;
            vStatic[C_ST_L]         = -1;

            for (size_t i=0; i<C_TOTAL; ++i)
                vValues[i]              = NULL;

            if (pDstColor != NULL)
                sColor.copy(pDstColor->color());
        }

        void CtlColor::map_static(size_t r, size_t g, size_t b, size_t h, size_t s, size_t l)
        {
            vStatic[C_ST_R]         = r;
            vStatic[C_ST_G]         = g;
            vStatic[C_ST_B]         = b;
            vStatic[C_ST_H]         = h;
            vStatic[C_ST_S]         = s;
            vStatic[C_ST_L]         = l;
        }

        void CtlColor::map_static_rgb(size_t r, size_t g, size_t b)
        {
            vStatic[C_ST_R]         = r;
            vStatic[C_ST_G]         = g;
            vStatic[C_ST_B]         = b;
        }

        void CtlColor::map_static_hsl(size_t h, size_t s, size_t l)
        {
            vStatic[C_ST_H]         = h;
            vStatic[C_ST_S]         = s;
            vStatic[C_ST_L]         = l;
        }

        void CtlColor::notify(CtlPort *port)
        {
            for (size_t i=C_R; i<=C_L; ++i)
            {
                if (port != vComponents[i])
                    continue;

                // Set-up color value
                float value = port->get_value();
                float c[C_TOTAL];

                switch (i)
                {
                    case C_R:
                    case C_G:
                    case C_B:
                        sColor.get_rgb(c[C_R], c[C_G], c[C_B]);
                        if (c[i] == value)
                            continue;

                        c[i] = value;
                        sColor.set_rgb(c[C_R], c[C_G], c[C_B]);
                        sColor.get_hsl(c[C_H], c[C_S], c[C_L]);
                        commit_color();
                        break;

                    case C_H:
                    case C_S:
                    case C_L:
                        sColor.get_hsl(c[C_H], c[C_S], c[C_L]);
                        if (c[i] == value)
                            continue;

                        c[i] = value;
                        sColor.set_hsl(c[C_H], c[C_S], c[C_L]);
                        sColor.get_rgb(c[C_R], c[C_G], c[C_B]);
                        commit_color();
                        break;
                    default:
                        continue;
                }

                // Update all
                for (size_t j=C_R; j<=C_L; ++j)
                {
                    if (vComponents[j] == NULL)
                        continue;
                    vComponents[j]->set_value(c[j]);
                }

                // Notify all
                for (size_t j=C_R; j<=C_L; ++j)
                {
                    if (vComponents[j] == NULL)
                        continue;
                    vComponents[j]->notify_all();
                }
            }
        }

        void CtlColor::set_alpha(float alpha)
        {
            if (sColor.alpha() == alpha)
                return;

            sColor.alpha(alpha);
            commit_color();
        }

        bool CtlColor::set(widget_attribute_t att, const char *value)
        {
            if (pWidget == NULL)
            {
                bool set = false;
                for (size_t i=0; i<C_TOTAL; ++i)
                {
                    if (vAttributes[i] != size_t(att))
                        continue;
                    if (vValues[i] != NULL)
                        free(vValues[i]);
                    vValues[i]  = strdup(value);
                    set         = true;
                }

                return set;
            }

            LSPDisplay *dpy =  pWidget->display();
            if (dpy == NULL)
                return false;
            LSPTheme *t = dpy->theme();
            if (t == NULL)
                return false;

            bool set = false;
            if (size_t(att) == vAttributes[C_BASIC])
            {
                t->get_color(value, &sColor);
                commit_color();
                set = true;
            }

            for (size_t i=C_R; i<=C_L; ++i)
            {
                if (size_t(att) != vAttributes[i])
                    continue;

                CtlPort *p = pRegistry->port(value);
                if (p == NULL)
                    continue;

                p->bind(this);
                vComponents[i] = p;
                set = true;
            }

            for (size_t i=C_ST_R; i<=C_ST_L; ++i)
            {
                if (size_t(att) != vStatic[i])
                    continue;

                float res;
                if (parse_float(value, &res))
                {
                    switch (i)
                    {
                        case C_ST_R: sColor.red(res); break;
                        case C_ST_G: sColor.green(res); break;
                        case C_ST_B: sColor.blue(res); break;
                        case C_ST_H: sColor.hue(res); break;
                        case C_ST_S: sColor.saturation(res); break;
                        case C_ST_L: sColor.lightness(res); break;
                        default:
                            continue;
                    }

                    commit_color();
                }
            }

            return set;
        }

        bool CtlColor::do_bind(CtlRegistry *reg, LSPWidget *widget, Color *col, LSPColor *lcol)
        {
            pRegistry           = reg;
            pWidget             = widget;
            pDstColor           = lcol;

            LSPDisplay *dpy =  pWidget->display();
            if (dpy == NULL)
                return false;
            LSPTheme *t = dpy->theme();
            if (t == NULL)
                return false;

            bool bound = false;
            for (size_t i=0; i<C_TOTAL; ++i)
            {
                if (vValues[i] == NULL)
                    continue;

                if (i == C_BASIC)
                {
                    t->get_color(vValues[i], &sColor);
                    commit_color();
                    bound = true;
                }
                else
                {
                    CtlPort *p = pRegistry->port(vValues[i]);
                    if (p == NULL)
                        continue;

                    p->bind(this);
                    vComponents[i] = p;
                    bound = true;
                }

                free(vValues[i]);
                vValues[i]      = NULL;
            }
            return bound;
        }

        bool CtlColor::set(LSPTheme *theme, const char *name)
        {
            bool result = theme->get_color(name, &sColor);
            commit_color();
            return result;
        }

        bool CtlColor::set(LSPTheme *theme, color_t color)
        {
            bool result = theme->get_color(color, &sColor);
            commit_color();
            return result;
        }

        void CtlColor::commit_color()
        {
            if (pDstColor != NULL)
                pDstColor->copy(sColor);
            if (pWidget != NULL)
                pWidget->query_draw();
        }
    
    } /* namespace ctl */
} /* namespace lsp */
