/*
 * CtlProgressBar.h
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLPROGRESSBAR_H_
#define UI_CTL_CTLPROGRESSBAR_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlProgressBar: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                enum pflags_t
                {
                    XF_VALUE             = 1 << 0,
                    XF_MIN               = 1 << 1,
                    XF_MAX               = 1 << 2
                };

                CtlPort        *pPort;
                CtlExpression   sMin;
                CtlExpression   sMax;
                CtlExpression   sValue;
                CtlColor        sColor;
                CtlColor        sScaleColor;
                LSPString       sFormat;
                size_t          nXFlags;

            protected:
                void    sync_state(CtlPort *port, bool force);

            public:
                explicit CtlProgressBar(CtlRegistry *src, LSPProgressBar *widget);
                virtual ~CtlProgressBar();

                virtual void init();

            public:
                virtual void set(widget_attribute_t att, const char *value);

                virtual void notify(CtlPort *port);

                virtual void end();

                virtual void sync_metadata(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLPROGRESSBAR_H_ */
