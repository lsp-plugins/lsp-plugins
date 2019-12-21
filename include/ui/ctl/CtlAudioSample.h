/*
 * CtlAudioSample.h
 *
 *  Created on: 9 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLAUDIOSAMPLE_H_
#define UI_CTL_CTLAUDIOSAMPLE_H_

namespace lsp
{
    namespace ctl
    {
        class CtlAudioSample: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlColor        sColor;
                CtlPadding      sPadding;

                CtlPort        *pMesh;
                CtlPort        *pStatus;
                CtlPort        *pLength;
                CtlPort        *pHeadCut;
                CtlPort        *pTailCut;
                CtlPort        *pFadeIn;
                CtlPort        *pFadeOut;
                CtlPort        *pCurrLen;
                CtlPort        *pMaxLen;

            protected:
                void            sync_status();
                void            sync_mesh();
                void            sync_fades();

            public:
                explicit CtlAudioSample(CtlRegistry *src, LSPAudioSample *as);
                virtual ~CtlAudioSample();

            public:
                virtual void init();

                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLAUDIOSAMPLE_H_ */
