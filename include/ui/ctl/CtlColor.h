/*
 * CtlColor.h
 *
 *  Created on: 28 июн. 2017 г.
 *      Author: sadko
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
                    C_BASIC, C_R, C_G, C_B, C_H, C_S, C_L,
                    C_TOTAL
                };

            private:
                CtlRegistry    *pRegistry;
                LSPWidget      *pWidget;
                CtlPort        *vComponents[C_TOTAL];
                size_t          vAttributes[C_TOTAL];
                char           *vValues[C_TOTAL];
                Color           sColor;
                Color          *pDstColor;

            protected:
                void    commit_color();

            public:
                CtlColor();
                virtual ~CtlColor();

            public:
                void init(CtlRegistry *reg, LSPWidget *widget, Color *col, size_t basic, size_t r, size_t g, size_t b, size_t h, size_t s, size_t l);

                inline void init_rgb(CtlRegistry *reg, LSPWidget *widget, Color *col, size_t basic, size_t r, size_t g, size_t b)
                {
                    init(reg, widget, col, basic, r, g, b, -1, -1, -1);
                }

                inline void init_hsl(CtlRegistry *reg, LSPWidget *widget, Color *col, size_t basic, size_t h, size_t s, size_t l)
                {
                    init(reg, widget, col, basic, -1, -1, -1, h, s, l);
                }

                inline void init_basic(CtlRegistry *reg, LSPWidget *widget, Color *col, size_t basic)
                {
                    init(reg, widget, col, basic, -1, -1, -1, -1, -1, -1);
                }

                void set_alpha(float alpha);

                bool set(widget_attribute_t att, const char *value);
                bool set(LSPTheme *theme, const char *name);
                bool set(LSPTheme *theme, color_t color);

                bool bind(CtlRegistry *reg, LSPWidget *widget, Color *col);

            public:
                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCOLOR_H_ */
