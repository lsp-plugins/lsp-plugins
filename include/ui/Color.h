/*
 * Color.h
 *
 *  Created on: 05 нояб. 2015 г.
 *      Author: sadko
 */

#ifndef UI_COLOR_H_
#define UI_COLOR_H_

#include <sys/types.h>
#include <core/Color.h>

namespace lsp
{
    class Theme;

    class ColorHolder: public Color
    {
        private:
            enum component_t
            {
                C_BASIC, C_R, C_G, C_B, C_H, C_S, C_L,
                C_TOTAL
            };

        private:
            IWidget    *pWidget;
//            Color       sColor;
            IUIPort    *vComponents[C_TOTAL];
            size_t      vAttributes[C_TOTAL];

        protected:
            void    init();

        public:
            ColorHolder();
            ColorHolder(const Color &src);
            ColorHolder(Theme &theme, const char *name);
            ColorHolder(Theme &theme, color_t color);

            void init(IWidget *widget, color_t dfl, size_t basic, size_t r, size_t g, size_t b, size_t h, size_t s, size_t l);

            bool notify(IUIPort *port);

            bool set(widget_attribute_t att, const char *value);
            bool set(Theme &theme, const char *name);
            bool set(Theme &theme, color_t color);

//            inline Color &color()               { return sColor; }
//
//            inline const Color &color() const   { return sColor; }
//
//            inline float red() const            { return sColor.red(); }
//            inline float green() const          { return sColor.green(); }
//            inline float blue() const           { return sColor.blue(); }
//            inline float alpha() const          { return sColor.alpha(); }
//
//            inline void red(float r)            { sColor.red(r); };
//            inline void green(float g)          { sColor.green(g); };
//            inline void blue(float b)           { sColor.blue(b); };
//            inline void alpha(float a)          { sColor.alpha(a); };
//
//            inline float hue() const            { return sColor.hue(); }
//            inline float saturation() const     { return sColor.saturation(); }
//            inline float lightness() const      { return sColor.lightness(); }
//
//            inline void hue(float h)            { sColor.hue(h);  };
//            inline void saturation(float s)     { sColor.saturation(s);  };
//            inline void lightness(float l)      { sColor.lightness(l); };
//
//            inline operator Color() const       { return sColor; };
//
//            inline uint32_t rgb24() const       { return sColor.rgb24(); };
    };

} /* namespace lsp */

#endif /* UI_COLOR_H_ */
