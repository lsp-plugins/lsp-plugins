/*
 * CtlListBox.h
 *
 *  Created on: 13 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLLISTBOX_H_
#define UI_CTL_CTLLISTBOX_H_

namespace lsp
{
    namespace ctl
    {
        class CtlListBox: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                LSPMessageBox      *pDialog;

            protected:
                static status_t     slot_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_ok(LSPWidget *sender, void *ptr, void *data);

            protected:
                status_t            on_submit();
                status_t            on_ok();

            public:
                explicit CtlListBox(CtlRegistry *src, LSPListBox *widget);
                virtual ~CtlListBox();
        };
    }

} /* namespace lsp */

#endif /* UI_CTL_CTLLISTBOX_H_ */
