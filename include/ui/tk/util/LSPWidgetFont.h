/*
 * LSPWidgetFont.h
 *
 *  Created on: 7 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPWIDGETFONT_H_
#define UI_TK_UTIL_LSPWIDGETFONT_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidget;
        
        class LSPWidgetFont: public LSPFont
        {
            protected:
                LSPWidget *pWidget;

            protected:
                virtual void on_change();

            public:
                explicit LSPWidgetFont(LSPDisplay *dpy);
                explicit LSPWidgetFont(LSPWidget *widget);
                explicit LSPWidgetFont(LSPDisplay *dpy, LSPWidget *widget);
                virtual ~LSPWidgetFont();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPWIDGETFONT_H_ */
