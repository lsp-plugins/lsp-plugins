/*
 * CtlTempoTap.h
 *
 *  Created on: 6 июн. 2018 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLTEMPOTAP_H_
#define UI_CTL_CTLTEMPOTAP_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlTempoTap: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort        *pPort;
                CtlColor        sColor;
                CtlColor        sTextColor;
                ssize_t         nThresh;
                uint64_t        nLastTap;
                float           fTempo;

            protected:
                static status_t     slot_change(LSPWidget *sender, void *ptr, void *data);
                void                submit_value();

                static uint64_t     time();

            public:
                explicit CtlTempoTap(CtlRegistry *src, LSPButton *widget);
                virtual ~CtlTempoTap();

            public:
                virtual void init();

                virtual void set(const char *name, const char *value);

                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLTEMPOTAP_H_ */
