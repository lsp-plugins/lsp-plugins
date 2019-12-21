/*
 * CtlAudioFile.h
 *
 *  Created on: 28 окт. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLAUDIOFILE_H_
#define UI_CTL_CTLAUDIOFILE_H_

#include <core/io/IInStream.h>
#include <ui/ctl/CtlPortHandler.h>

namespace lsp
{
    namespace ctl
    {
        class CtlAudioFile: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                enum const_t
                {
                    N_MENU_ITEMS = 4
                };

            protected:
                class DataSink: public LSPTextDataSink
                {
                    private:
                        CtlAudioFile *pFile;

                    public:
                        explicit DataSink(CtlAudioFile *file);
                        virtual ~DataSink();

                    public:
                        virtual status_t    on_complete(status_t code, const LSPString *data);

                        void unbind();
                };

            protected:
                CtlColor        sColor;
                CtlPadding      sPadding;
                CtlExpression   sFormat;
                LSPMenu         sMenu;
                LSPMenuItem    *vMenuItems[N_MENU_ITEMS];
                char           *pPathID;
                LSPString       sBind;

                CtlPort        *pFile;
                CtlPort        *pMesh;
                CtlPort        *pStatus;
                CtlPort        *pLength;
                CtlPort        *pHeadCut;
                CtlPort        *pTailCut;
                CtlPort        *pFadeIn;
                CtlPort        *pFadeOut;
                CtlPort        *pPath;
                DataSink       *pDataSink;

            protected:
                void            sync_status();
                void            sync_file();
                void            sync_mesh();
                void            sync_fades();

                void            commit_file();
                void            update_path();
                status_t        bind_ports(CtlPortHandler *h);

                static status_t     slot_on_activate(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_close(LSPWidget *sender, void *ptr, void *data);

                static status_t     slot_popup_cut_action(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_popup_copy_action(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_popup_paste_action(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_popup_clear_action(LSPWidget *sender, void *ptr, void *data);

                static status_t     clipboard_handler(void *arg, status_t s, io::IInStream *is);

            public:
                explicit CtlAudioFile(CtlRegistry *src, LSPAudioFile *af);
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
