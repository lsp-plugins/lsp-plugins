/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 19 июн. 2017 г.
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

#ifndef UI_TK_LSPTHEME_H_
#define UI_TK_LSPTHEME_H_

namespace lsp
{
    namespace tk
    {
        class LSPFont;
        class LSPDisplay;
        class LSPColor;
        class LSPStyle;

        class LSPTheme
        {
            private:
                typedef struct color_data_t
                {
                    char *name;
                    Color color;
                } color_data_t;

            private:
                cstorage<color_data_t>  sColors;
                LSPFont                *pFont;
                LSPStyle                sStyle;
                LSPColor                sBgColor;
                LSPColor                sHoleColor;
                LSPColor                sGlassColor;
                LSPFloat                sBrightness;

            private:
                bool find_color(const char *name, Color *dst);
                static ssize_t read_component(const char *text, size_t digits);
                static bool get_components(const char *text, float &c1, float &c2, float &c3);
                bool parse_rgb(const char *text, Color *dst);
                bool parse_hsl(const char *text, Color *dst);

            public:
                explicit LSPTheme();
                ~LSPTheme();

                void init(LSPDisplay *dpy);

                status_t after_load();

            public:
                inline LSPFont          *font() { return pFont; }

                /**
                 * Get root style
                 * @return root style
                 */
                inline LSPStyle         *root() { return &sStyle; }

                /**
                 * Get default background color for widgets
                 * @return default background color for widgets
                 */
                inline LSPColor         *bg_color() { return &sBgColor; }

                /**
                 * Get color of hole (space not containing anything)
                 * @return color of hole
                 */
                inline LSPColor         *hole_color() { return &sHoleColor; }

                /**
                 * Get default brightness for widgets
                 * @return default brightness for widgets
                 */
                inline LSPFloat         *brightness() { return &sBrightness; }

            public:
                /** Add color to theme
                 *
                 * @param name color name
                 * @param value color value
                 * @return true if color was added
                 */
                bool add_color(const char *name, const char *value);

                /** Add color to theme
                 *
                 * @param name color name
                 * @param color color value
                 * @return true if added
                 */
                bool add_color(const char *name, const Color *color);

                /** Get color by it's symbolic name
                 *
                 * @param name color name
                 * @param dst pointer to store object
                 * @return false if default color was used
                 */
                bool get_color(const char *name, Color *dst);

                /** Add color to theme
                 *
                 * @param name color name
                 * @param color color value
                 * @return true if added
                 */
                bool add_color(const char *name, const Color &color);

                /** Get color by it's symbolic name
                 *
                 * @param name color name
                 * @param dst pointer to store object
                 * @return false if default color was used
                 */
                bool get_color(const char *name, Color &dst);

                /** Get color by it's symbolic name
                 *
                 * @param color color ID
                 * @param dst pointer to store object
                 * @return false if default color was used
                 */
                bool get_color(color_t color, Color *dst);

                /** Get color by it's symbolic name
                 *
                 * @param color color ID
                 * @param dst pointer to store object
                 * @return false if default color was used
                 */
                bool get_color(color_t color, Color &dst);

                /** Get color by it's symbolic name
                 *
                 * @param color color ID
                 * @param dst pointer to store object
                 * @return false if default color was used
                 */
                bool get_color(color_t color, LSPColor &dst);

                /** Get color by it's symbolic name
                 *
                 * @param color color ID
                 * @param dst pointer to store object
                 * @return false if default color was used
                 */
                bool get_color(color_t color, LSPColor *dst);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPTHEME_H_ */
