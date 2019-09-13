/*
 * CtlFraction.h
 *
 *  Created on: 1 июн. 2018 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLFRACTION_H_
#define UI_CTL_CTLFRACTION_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlFraction: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort            *pPort;
                CtlPort            *pDenom;
                CtlColor            sTextColor;
                CtlColor            sColor;
                float               fSig;
                float               fMaxSig;
                ssize_t             nDenomMin;
                ssize_t             nDenomMax;
                ssize_t             nNum;
                ssize_t             nDenom;

            protected:
                static status_t    slot_change(LSPWidget *sender, void *ptr, void *data);

                void update_values();
                void submit_value();

                void sync_numerator(LSPFraction *frac);

            public:
                explicit CtlFraction(CtlRegistry *src, LSPFraction *frac);
                virtual ~CtlFraction();

            public:
                virtual void set(widget_attribute_t att, const char *value);

                virtual void init();

                virtual void begin();

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLFRACTION_H_ */
