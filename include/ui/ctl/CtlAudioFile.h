/*
 * CtlAudioFile.h
 *
 *  Created on: 28 окт. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLAUDIOFILE_H_
#define UI_CTL_CTLAUDIOFILE_H_

namespace lsp
{
    namespace ctl
    {
        class CtlAudioFile: public CtlWidget
        {
            protected:
                CtlColor        sColor;
                CtlColor        sBgColor;
                CtlPadding      sPadding;
                char           *pPathID;

                CtlPort        *pFile;
                CtlPort        *pMesh;
                CtlPort        *pStatus;
                CtlPort        *pLength;
                CtlPort        *pHeadCut;
                CtlPort        *pTailCut;
                CtlPort        *pFadeIn;
                CtlPort        *pFadeOut;
                CtlPort        *pPath;

            protected:
                void            sync_status();
                void            sync_file();
                void            sync_mesh();
                void            sync_fades();

                void            commit_file();
                void            update_path();

                static status_t     slot_on_activate(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_close(LSPWidget *sender, void *ptr, void *data);

            public:
                CtlAudioFile(CtlRegistry *src, LSPAudioFile *af);
                virtual ~CtlAudioFile();

            public:
                virtual void init();

                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);

        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLAUDIOFILE_H_ */
