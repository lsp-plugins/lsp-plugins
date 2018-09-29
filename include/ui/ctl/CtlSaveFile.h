/*
 * CtlSaveFile.h
 *
 *  Created on: 20 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLSAVEFILE_H_
#define UI_CTL_CTLSAVEFILE_H_

namespace lsp
{
    namespace ctl
    {
        class CtlSaveFile: public CtlWidget
        {
            protected:
                CtlPort        *pFile;
                CtlPort        *pStatus;
                CtlPort        *pCommand;
                CtlPort        *pProgress;
                CtlPort        *pPath;
                CtlExpression   sFormat;

                char           *pPathID;

            protected:
                void        update_state();
                status_t    commit_state();

            protected:
                static status_t slot_on_activate(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_close(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_file_submit(LSPWidget *sender, void *ptr, void *data);
                void        update_path();

            public:
                explicit CtlSaveFile(CtlRegistry *reg, LSPSaveFile *save);
                virtual ~CtlSaveFile();

            public:
                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLSAVEFILE_H_ */
