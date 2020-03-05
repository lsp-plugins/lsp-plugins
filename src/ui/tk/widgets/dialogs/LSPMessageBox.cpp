/*
 * LSPMessageBox.cpp
 *
 *  Created on: 29 сент. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPMessageBox::metadata = { "LSPMessageBox", &LSPWindow::metadata };

        LSPMessageBox::LSPMessageBox(LSPDisplay *dpy):
            LSPWindow(dpy),
            sHeading(dpy),
            sMessage(dpy),
            sHeadAlign(dpy),
            sMsgAlign(dpy),
            sVBox(dpy),
            sHBox(dpy)
        {
            nMinBtnWidth    = 96;
            nMinBtnHeight   = 24;
            pClass          = &metadata;
        }

        LSPMessageBox::~LSPMessageBox()
        {
            do_destroy();
        }

        status_t LSPMessageBox::init()
        {
            // Initialize labels
            status_t result = LSPWindow::init();
            if (result == STATUS_OK)
                result = sHeading.init();
            if (result == STATUS_OK)
            {
                sHeading.font()->set_size(16);
                sHeading.font()->set_bold(true);
            }
            if (result == STATUS_OK)
                result = sMessage.init();

            // Initialize boxes
            if (result == STATUS_OK)
                result = sVBox.init();
            if (result == STATUS_OK)
            {
                sVBox.set_vertical();
                sVBox.set_spacing(8);
            }
            if (result == STATUS_OK)
                result = sHBox.init();
            if (result == STATUS_OK)
            {
                sHBox.set_horizontal();
                sHBox.set_spacing(8);
            }

            // Initialize alignment
            if (result == STATUS_OK)
                result = sHeadAlign.init();
            if (result == STATUS_OK)
                sHeadAlign.set_pos(0, 0.5f);
            if (result == STATUS_OK)
                result = sMsgAlign.init();
            if (result == STATUS_OK)
                sMsgAlign.set_pos(0.5f, 0.5f);

            // Initialize structure
            if (result == STATUS_OK)
                result = sHeadAlign.add(&sHeading);
            if (result == STATUS_OK)
                result = sMsgAlign.add(&sMessage);
            if (result == STATUS_OK)
                result = sVBox.add(&sHeadAlign);
            if (result == STATUS_OK)
                result = sVBox.add(&sMsgAlign);
            if (result == STATUS_OK)
                result = sVBox.add(&sHBox);

            // Add child
            if (result == STATUS_OK)
                result = this->add(&sVBox);

            padding()->set_all(16);
            set_border_style(BS_DIALOG);
            actions()->set_actions(WA_DIALOG);

            return result;
        }

        void LSPMessageBox::do_destroy()
        {
            // Clear all buttons
            clear_buttons();

            // Destroy widgets
            sHBox.destroy();
            sVBox.destroy();
            sHeadAlign.destroy();
            sMsgAlign.destroy();
            sMessage.destroy();
            sHeading.destroy();
        }

        void LSPMessageBox::destroy()
        {
            do_destroy();
            LSPWindow::destroy();
        }

        void LSPMessageBox::set_use_heading(bool use)
        {
            sHeadAlign.set_visible(use);
        }

        void LSPMessageBox::set_use_message(bool use)
        {
            sMsgAlign.set_visible(use);
        }

        status_t LSPMessageBox::slot_on_button_submit(LSPWidget *sender, void *ptr, void *data)
        {
            // Hide message box
            LSPMessageBox *mbox = widget_ptrcast<LSPMessageBox>(ptr);
            if (mbox != NULL)
                mbox->hide();

            return STATUS_OK;
        }

        status_t LSPMessageBox::add_button(const char *text, ui_event_handler_t handler, void *arg)
        {
            LSPLocalString tmp;
            status_t res = tmp.set(text);
            return (res == STATUS_OK) ? add_button(&tmp, handler, arg) : res;
        }

        status_t LSPMessageBox::add_button(const LSPString *text, ui_event_handler_t handler, void *arg)
        {
            LSPLocalString tmp;
            status_t res = tmp.set(text);
            return (res == STATUS_OK) ? add_button(&tmp, handler, arg) : res;
        }

        status_t LSPMessageBox::add_button(const LSPLocalString *text, ui_event_handler_t handler, void *arg)
        {
            LSPButton *btn = new LSPButton(pDisplay);
            if (btn == NULL)
                return STATUS_NO_MEM;

            status_t result = (vButtons.add(btn)) ? STATUS_OK : STATUS_NO_MEM;

            if (result == STATUS_OK)
                result = btn->init();
            if (result == STATUS_OK)
            {
                btn->set_normal();
                btn->set_min_size(nMinBtnWidth, nMinBtnHeight);
                result = btn->slots()->bind(LSPSLOT_SUBMIT, slot_on_button_submit, self());
                result = (result >= 0) ? STATUS_OK : -result;
            }

            if (result == STATUS_OK)
                result = btn->title()->set(text);

            if ((result == STATUS_OK) && (handler != NULL))
            {
                result = btn->slots()->bind(LSPSLOT_SUBMIT, handler, arg);
                result = (result >= 0) ? STATUS_OK : -result;
            }

            if (result == STATUS_OK)
                result = sHBox.add(btn);

            sHBox.set_visible(vButtons.size() > 0);

            if (result != STATUS_OK)
            {
                vButtons.remove(btn);
                btn->destroy();
                delete btn;
            }

            return result;
        }

        void LSPMessageBox::clear_buttons()
        {
            // Unbind all buttons
            size_t n = vButtons.size();
            for (size_t i=0; i<n; ++i)
            {
                LSPButton *btn = vButtons.at(i);
                if (btn == NULL)
                    continue;

                sHBox.remove(btn);
                btn->destroy();
                delete btn;
            }

            // Clear all buttons in  the array
            vButtons.clear();
        }

        void LSPMessageBox::set_min_button_width(size_t value)
        {
            if (nMinBtnWidth == value)
                return;

            size_t n = vButtons.size();
            for (size_t i=0; i<n; ++i)
            {
                LSPButton *btn = vButtons.at(i);
                if (btn == NULL)
                    continue;

                btn->set_min_width(value);
            }
            nMinBtnWidth = value;
        }

        void LSPMessageBox::set_min_button_height(size_t value)
        {
            if (nMinBtnHeight == value)
                return;

            size_t n = vButtons.size();
            for (size_t i=0; i<n; ++i)
            {
                LSPButton *btn = vButtons.at(i);
                if (btn == NULL)
                    continue;

                btn->set_min_height(value);
            }
            nMinBtnHeight = value;
        }

    } /* namespace tk */
} /* namespace lsp */
