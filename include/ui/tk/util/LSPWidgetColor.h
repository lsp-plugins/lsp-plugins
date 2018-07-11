/*
 * LSPWidgetColor.h
 *
 *  Created on: 5 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPWIDGETCOLOR_H_
#define UI_TK_UTIL_LSPWIDGETCOLOR_H_

namespace lsp
{
    namespace tk
    {
        class LSPWidget;

        class LSPWidgetColor: public LSPColor
        {
            protected:
                LSPWidget *pWidget;

            protected:
                virtual void    color_changed();

            public:
                explicit LSPWidgetColor();
                LSPWidgetColor(LSPWidget *widget);

                virtual ~LSPWidgetColor();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPWIDGETCOLOR_H_ */
