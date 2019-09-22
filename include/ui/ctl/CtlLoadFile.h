/*
 * CtlLoadFile.h
 *
 *  Created on: 07 мая 2018 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLLOADFILE_H_
#define UI_CTL_CTLLOADFILE_H_

namespace lsp
{
    namespace ctl
    {
        class CtlLoadFile: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

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
                explicit CtlLoadFile(CtlRegistry *reg, LSPLoadFile *load);
                virtual ~CtlLoadFile();

            public:
                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLLOADFILE_H_ */
