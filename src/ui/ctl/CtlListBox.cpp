/*
 * CtlListBox.cpp
 *
 *  Created on: 13 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlListBox::metadata = { "CtlListBox", &CtlWidget::metadata };

        CtlListBox::CtlListBox(CtlRegistry *src, LSPListBox *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pDialog         = NULL;

//            char str[32];

            LSPItemList *items = widget->items();
            LSPItem *item;

            #define MP(x) items->add(&item); item->text()->set_raw(#x); item->set_value(x);

            MP(MP_NONE);
            MP(MP_ARROW);
            MP(MP_HAND);
            MP(MP_CROSS);
            MP(MP_IBEAM);
            MP(MP_DRAW);
            MP(MP_PLUS);
            MP(MP_SIZE_NESW);
            MP(MP_SIZE_NS);
            MP(MP_SIZE_WE);
            MP(MP_SIZE_NWSE);
            MP(MP_UP_ARROW);
            MP(MP_HOURGLASS);
            MP(MP_DRAG);
            MP(MP_NO_DROP);
            MP(MP_DANGER);
            MP(MP_HSPLIT);
            MP(MP_VPSLIT);
            MP(MP_MULTIDRAG);
            MP(MP_APP_START);
            MP(MP_HELP);

            #undef MP

            /*for (size_t i=0; i<20; ++i)
            {
                sprintf(str, "item %d", int(i));
                items->add(str, i);
            }*/
//            widget->selection()->set_value(1);
            widget->selection()->set_value(MP_DEFAULT);

            widget->slots()->bind(LSPSLOT_SUBMIT, slot_submit, this);
        }
        
        CtlListBox::~CtlListBox()
        {
            if (pDialog != NULL)
            {
                pDialog->destroy();
                delete pDialog;
                pDialog = NULL;
            }
        }

        status_t CtlListBox::slot_submit(LSPWidget *sender, void *ptr, void *data)
        {
            CtlListBox *_this    = static_cast<CtlListBox *>(ptr);
            return (_this != NULL) ? _this->on_submit() : STATUS_OK;
        }

        status_t CtlListBox::slot_on_ok(LSPWidget *sender, void *ptr, void *data)
        {
            CtlListBox *_this    = static_cast<CtlListBox *>(ptr);
            return (_this != NULL) ? _this->on_ok() : STATUS_OK;
        }

        status_t CtlListBox::on_submit()
        {
/*            if (pDialog == NULL)
            {
                pDialog = new LSPMessageBox(pWidget->display());
                pDialog->init();

                pDialog->set_title("Notification");
                pDialog->set_heading("ListBox value has been changed");
                pDialog->add_button("Okay", slot_on_ok, this);
                pDialog->add_button("Not okay");
                pDialog->add_button("Cancel");
            }

            char text[256];
            LSPListBox *lb = static_cast<LSPListBox *>(pWidget);
            ssize_t index = lb->selection()->value();
            LSPItem *itm = (index >= 0) ? lb->items()->get(index) : NULL;

            sprintf(text, "You've selected item '%s', it's value is %f, selected index = %d",
                (itm != NULL) ? itm->text() : NULL, (itm != NULL) ? itm->value() : 0.0f,
                int(index)
            );

            pDialog->set_message(text);
            pDialog->show(pWidget);*/
            LSPListBox *lb = widget_cast<LSPListBox>(pWidget);
            if (lb == NULL)
                return STATUS_OK;

//            LSPWindow *wnd = widget_cast<LSPWindow>(pWidget->toplevel());
//            if (wnd == NULL)
//                return STATUS_OK;
            size_t index = lb->selection()->value();
            LSPItem *item = lb->items()->get(index);

            if (item != NULL)
                lb->set_cursor(mouse_pointer_t(item->value()));

            return STATUS_OK;
        }

        status_t CtlListBox::on_ok()
        {
            lsp_trace("OK button pressed");
            return STATUS_OK;
        }
    }

} /* namespace lsp */
