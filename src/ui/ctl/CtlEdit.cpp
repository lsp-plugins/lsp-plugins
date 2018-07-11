/*
 * CtlEdit.cpp
 *
 *  Created on: 6 сент. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        CtlEdit::CtlEdit(CtlRegistry *src, LSPEdit *widget): CtlWidget(src, widget)
        {
            pDialog = NULL;

            char str[40];
            LSPMenu *menu = new LSPMenu(widget->display());
            menu->init();

            for (size_t i=0; i<50; ++i)
            {
                LSPMenuItem *item = new LSPMenuItem(widget->display());
                item->init();
                sprintf(str, "Menu item %d", int(i));
                item->set_text(str);
                menu->add(item);
                item->slots()->bind(LSPSLOT_SUBMIT, slot_on_submit, this);

                if ((i%5) == 4)
                {
                    item = new LSPMenuItem(widget->display());
                    item->init();
                    item->set_separator(true);
                    menu->add(item);
                }
            }
            widget->set_popup(menu);
            widget->set_text("Hello World! This is a test of text editing capabilities.");
        }
        
        CtlEdit::~CtlEdit()
        {
        }

        status_t CtlEdit::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            CtlEdit *_this      = static_cast<CtlEdit *>(ptr);
            return (_this != NULL) ? _this->on_menu_submit() : STATUS_OK;
        }

        status_t CtlEdit::slot_on_action(LSPWidget *sender, void *ptr, void *data)
        {
            lsp_trace("Successful action");
            return STATUS_OK;
        }

        status_t CtlEdit::slot_on_cancel(LSPWidget *sender, void *ptr, void *data)
        {
            lsp_trace("Cancelled action");
            return STATUS_OK;
        }

        status_t CtlEdit::on_menu_submit()
        {
            if (pDialog == NULL)
            {
                pDialog     = new LSPFileDialog(pWidget->display());
                pDialog->init();
                pDialog->set_title("Open file...");
                pDialog->set_action_title("Open");
                pDialog->bind_action(slot_on_action, this);
                pDialog->bind_cancel(slot_on_cancel, this);
                pDialog->set_confirmation("Do you really want to load file?");

                LSPFileFilter *f = pDialog->filter();

                f->add("*.txt", "Text files", ".txt");
                f->add("*.wav|*.mp3", "Audio files", ".wav");
                f->add("*", "All files", "");
                f->set_default(2);
            }

            pDialog->show(pWidget);

            return STATUS_OK;
        }
    }

} /* namespace lsp */
