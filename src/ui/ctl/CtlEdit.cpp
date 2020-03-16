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
        const ctl_class_t CtlEdit::metadata = { "CtlEdit", &CtlWidget::metadata };

        CtlEdit::CtlEdit(CtlRegistry *src, LSPEdit *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pDialog         = NULL;

            char str[40];
            LSPMenu *menu = new LSPMenu(widget->display());
            vWidgets.add(menu);
            menu->init();

            for (size_t i=0; i<50; ++i)
            {
                LSPMenuItem *item = new LSPMenuItem(widget->display());
                vWidgets.add(item);
                item->init();
                sprintf(str, "Menu item %d", int(i));
                item->text()->set_raw(str);
                menu->add(item);
                item->slots()->bind(LSPSLOT_SUBMIT, slot_on_submit, this);

                if ((i%5) == 4)
                {
                    item = new LSPMenuItem(widget->display());
                    vWidgets.add(item);
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
            for (size_t i=0, n=vWidgets.size(); i<n; ++i)
            {
                LSPWidget *w = vWidgets.at(i);
                if (w != NULL)
                {
                    w->destroy();
                    delete w;
                }
            }
            vWidgets.clear();
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
                pDialog->title()->set_raw("Open file...");
                pDialog->action_title()->set("actions.open");
                pDialog->bind_action(slot_on_action, this);
                pDialog->bind_cancel(slot_on_cancel, this);
                pDialog->set_use_confirm(true);
                pDialog->confirm()->set("messages.file.confirm_load");

                LSPFileFilter *f = pDialog->filter();
                {
                    LSPFileFilterItem ffi;

                    ffi.pattern()->set("*.txt");
                    ffi.title()->set("files.text.txt");
                    ffi.set_extension(".txt");
                    f->add(&ffi);

                    ffi.pattern()->set("*.wav|*.mp3");
                    ffi.title()->set("files.audio.all");
                    ffi.set_extension(".wav");
                    f->add(&ffi);

                    ffi.pattern()->set("*");
                    ffi.title()->set("files.all");
                    ffi.set_extension("");
                    f->add(&ffi);
                }
                f->set_default(2);
            }

            pDialog->show(pWidget);

            return STATUS_OK;
        }
    }

} /* namespace lsp */
