/*
 * CtlEdit.h
 *
 *  Created on: 6 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLEDIT_H_
#define UI_CTL_CTLEDIT_H_

namespace lsp
{
    namespace ctl
    {
        class CtlEdit: public CtlWidget
        {
            protected:
                LSPFileDialog      *pDialog;

            protected:
                static status_t slot_on_submit(void *ptr, void *data);
                static status_t slot_on_action(void *ptr, void *data);
                static status_t slot_on_cancel(void *ptr, void *data);

            public:
                CtlEdit(CtlRegistry *src, LSPEdit *widget);
                virtual ~CtlEdit();

            public:
                virtual status_t on_menu_submit();
        };
    }

} /* namespace lsp */

#endif /* UI_CTL_CTLEDIT_H_ */
