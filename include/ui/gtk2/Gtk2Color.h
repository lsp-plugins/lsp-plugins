/*
 * Gtk2Color.h
 *
 *  Created on: 26 окт. 2015 г.
 *      Author: sadko
 */

#ifndef UI_GTK2_GTK2COLOR_H_
#define UI_GTK2_GTK2COLOR_H_

namespace lsp
{
    
    class Gtk2Color: public Color
    {
        protected:
            static const size_t M_GDK           = (1 << Color::M_MASK_BITS);
            static const size_t M_MASK_BITS     = Color::M_MASK_BITS + 1;

            mutable GdkColor    sColor;
            static const double COLOR_BASE       = 1.0f / 0xffff;

            void calc_gdk_color() const;

        public:
            inline Gtk2Color(): Color() {};
            inline Gtk2Color(float r, float g, float b): Color(r, g, b) {};
            inline Gtk2Color(const Gtk2Color &src): Color(src) { sColor = src.sColor; };
            inline Gtk2Color(const GdkColor &src): Color(src.red * COLOR_BASE, src.green * COLOR_BASE, src.blue * COLOR_BASE, M_RGB) {};
            inline Gtk2Color(Theme &theme, const char *name): Color(theme, name) {};
            inline Gtk2Color(Theme &theme, color_t color): Color(theme, color) {};

            inline uint32_t getColor() const { return color()->pixel; };

            inline gchar   *format() const { return gdk_color_to_string (&sColor);  };
            inline GdkColor *color() const
            {
                sColor.red      =   R * 0xffff;
                sColor.green    =   G * 0xffff;
                sColor.blue     =   B * 0xffff;
                sColor.pixel    =   ((sColor.red & 0xff00) << 8) | (sColor.green & 0xff00) | (sColor.blue >> 8);
                return &sColor;
            };
    };

} /* namespace lsp */

#endif /* UI_GTK2_GTK2COLOR_H_ */
