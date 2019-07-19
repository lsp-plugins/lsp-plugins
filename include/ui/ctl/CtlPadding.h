/*
 * CtlPadding.h
 *
 *  Created on: 12 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLPADDING_H_
#define UI_CTL_CTLPADDING_H_

namespace lsp
{
    namespace ctl
    {
        class CtlPadding
        {
            protected:
                LSPPadding     *pPadding;
                ssize_t         vAttributes[5];

            public:
                explicit CtlPadding();
                virtual ~CtlPadding();

            public:
                void init(LSPPadding *padding, ssize_t l, ssize_t r, ssize_t t, ssize_t b, ssize_t c);

                inline void init(LSPPadding *padding)
                {
                    init(padding, A_PAD_LEFT, A_PAD_RIGHT, A_PAD_TOP, A_PAD_BOTTOM, A_PADDING);
                }

                bool set(widget_attribute_t att, const char *value);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLPADDING_H_ */
