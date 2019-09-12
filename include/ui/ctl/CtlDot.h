/*
 * CtlDot.h
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLDOT_H_
#define UI_CTL_CTLDOT_H_

namespace lsp
{
    namespace ctl
    {
        class CtlDot: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort        *pTop;
                CtlPort        *pLeft;
                CtlPort        *pScroll;
                size_t          nFlags;
                float           fTop;
                float           fLeft;
                bool            bEditable;
                CtlColor        sColor;

            protected:
                static status_t     slot_change(LSPWidget *sender, void *ptr, void *data);
                void                submit_values();
                void                submit_value(CtlPort *port, float value);
                void                commit_value(float value);

            public:
                explicit CtlDot(CtlRegistry *src, LSPDot *dot);
                virtual ~CtlDot();

            public:
                virtual void init();

                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLDOT_H_ */
