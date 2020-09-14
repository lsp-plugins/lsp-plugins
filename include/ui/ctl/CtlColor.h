/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 28 июн. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef UI_CTL_CTLCOLOR_H_
#define UI_CTL_CTLCOLOR_H_

namespace lsp
{
    namespace ctl
    {
        class CtlColor: public CtlPortListener
        {
            private:
                enum component_t
                {
                    C_BASIC,
                    C_R, C_G, C_B, C_H, C_S, C_L,
                    C_TOTAL
                };

                enum static_t
                {
                    C_ST_R, C_ST_G, C_ST_B, C_ST_H, C_ST_S, C_ST_L,
                    C_ST_TOTAL
                };

            private:
                CtlRegistry    *pRegistry;
                LSPWidget      *pWidget;
                CtlPort        *vComponents[C_TOTAL];
                size_t          vAttributes[C_TOTAL];
                size_t          vStatic[C_ST_TOTAL];
                char           *vValues[C_TOTAL];
                Color           sColor;
                LSPColor       *pDstColor;

            protected:
                void    commit_color();
                void    do_init(CtlRegistry *reg, LSPWidget *widget, Color *col, LSPColor *lcol, size_t basic, size_t r, size_t g, size_t b, size_t h, size_t s, size_t l);
                bool    do_bind(CtlRegistry *reg, LSPWidget *widget, Color *col, LSPColor *lcol);

            public:
                explicit CtlColor();
                virtual ~CtlColor();

            public:
                void init(CtlRegistry *reg, LSPWidget *widget, LSPColor *col, size_t basic, size_t r, size_t g, size_t b, size_t h, size_t s, size_t l)
                {
                    do_init(reg, widget, NULL, col, basic, r, g, b, h, s, l);
                }

                inline void init_rgb(CtlRegistry *reg, LSPWidget *widget, LSPColor *col, size_t basic, size_t r, size_t g, size_t b)
                {
                    do_init(reg, widget, NULL, col, basic, r, g, b, -1, -1, -1);
                }

                inline void init_hsl(CtlRegistry *reg, LSPWidget *widget, LSPColor *col, size_t basic, size_t h, size_t s, size_t l)
                {
                    do_init(reg, widget, NULL, col, basic, -1, -1, -1, h, s, l);
                }

                inline void init_basic(CtlRegistry *reg, LSPWidget *widget, LSPColor *col, size_t basic)
                {
                    do_init(reg, widget, NULL, col, basic, -1, -1, -1, -1, -1, -1);
                }

                void map_static(size_t r, size_t g, size_t b, size_t h, size_t s, size_t l);
                void map_static_rgb(size_t r, size_t g, size_t b);
                void map_static_hsl(size_t h, size_t s, size_t l);

                void set_alpha(float alpha);

                bool set(widget_attribute_t att, const char *value);
                bool set(LSPTheme *theme, const char *name);
                bool set(LSPTheme *theme, color_t color);

                bool bind(CtlRegistry *reg, LSPWidget *widget, LSPColor *col)
                {
                    return do_bind(reg, widget, NULL, col);
                }

            public:
                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCOLOR_H_ */
