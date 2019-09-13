/*
 * CtlFrameBuffer.h
 *
 *  Created on: 13 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLFRAMEBUFFER_H_
#define UI_CTL_CTLFRAMEBUFFER_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlFrameBuffer: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            public:
                explicit CtlFrameBuffer(CtlRegistry *src, LSPFrameBuffer *fb);
                virtual ~CtlFrameBuffer();

            protected:
                CtlPort        *pPort;
                size_t          nRowID;
                CtlColor        sColor;
                CtlExpression   sMode;

            public:
                virtual void init();

                virtual void destroy();

                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_CTL_CTLFRAMEBUFFER_H_ */
