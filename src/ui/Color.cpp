/*
 * Color.cpp
 *
 *  Created on: 05 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>

namespace lsp
{
    bool ColorHolder::set(Theme &theme, const char *name)
    {
        return theme.getColor(name, this);
    }

    bool ColorHolder::set(Theme &theme, color_t color)
    {
        return theme.getColor(color, this);
    }

    void ColorHolder::init()
    {
        pWidget         = NULL;
        for (size_t i=0; i<C_TOTAL; ++i)
        {
            vComponents[i]  = NULL;
            vAttributes[i]  = -1;
        }
    }

    ColorHolder::ColorHolder(const Color &src): Color(src)
    {
        init();
    }

    ColorHolder::ColorHolder()
    {
        init();
    }

    ColorHolder::ColorHolder(Theme &theme, const char *name)
    {
        init();
        set(theme, name);
    }

    ColorHolder::ColorHolder(Theme &theme, color_t color)
    {
        init();
        set(theme, color);
    }

    void ColorHolder::init(IWidget *widget, color_t dfl, size_t basic, size_t r, size_t g, size_t b, size_t h, size_t s, size_t l)
    {
        pWidget                 = widget;
        vAttributes[C_BASIC]    = basic;
        vAttributes[C_R]        = r;
        vAttributes[C_G]        = g;
        vAttributes[C_B]        = b;
        vAttributes[C_H]        = h;
        vAttributes[C_S]        = s;
        vAttributes[C_L]        = l;

        if (pWidget->getUI() != NULL)
            pWidget->getUI()->theme().getColor(dfl, this);
    }

    bool ColorHolder::notify(IUIPort *port)
    {
        for (size_t i=C_R; i<=C_L; ++i)
        {
            if (port != vComponents[i])
                continue;

            // Set-up color value
            float value = port->getValue();
            float c[C_TOTAL];

            switch (i)
            {
                case C_R:
                case C_G:
                case C_B:
                    get_rgb(c[C_R], c[C_G], c[C_B]);
                    if (c[i] == value)
                        return true;

                    c[i] = value;
                    set_rgb(c[C_R], c[C_G], c[C_B]);
                    get_hsl(c[C_H], c[C_S], c[C_L]);
                    break;

                case C_H:
                case C_S:
                case C_L:
                    get_hsl(c[C_H], c[C_S], c[C_L]);
                    if (c[i] == value)
                        return true;

                    c[i] = value;
                    set_hsl(c[C_H], c[C_S], c[C_L]);
                    get_rgb(c[C_R], c[C_G], c[C_B]);
                    break;
                default:
                    return false;
            }

            // Update all
            for (size_t j=C_R; j<=C_L; ++j)
            {
                if (vComponents[j] == NULL)
                    continue;
                vComponents[j]->setValue(c[j]);
            }

            // Notify all
            for (size_t j=C_R; j<=C_L; ++j)
            {
                if (vComponents[j] == NULL)
                    continue;
                vComponents[j]->notifyAll();
            }
            return true;
        }

        return false;
    }

    bool ColorHolder::set(widget_attribute_t att, const char *value)
    {
        if (pWidget == NULL)
            return false;
        plugin_ui *ui = pWidget->getUI();
        if (ui == NULL)
            return false;

        if (size_t(att) == vAttributes[C_BASIC])
        {
            ui->theme().getColor(value, this);
            return true;
        }

        for (size_t i=C_R; i<=C_L; ++i)
        {
            if (size_t(att) != vAttributes[i])
                continue;

            IUIPort *p = ui->port(value);
            if (p == NULL)
                continue;

            p->bind(pWidget);
            vComponents[i] = p;
            return true;
        }

        return false;
    }

} /* namespace lsp */
