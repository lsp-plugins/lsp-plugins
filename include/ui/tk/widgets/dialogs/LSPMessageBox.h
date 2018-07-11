/*
 * LSPMessageBox.h
 *
 *  Created on: 29 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_DIALOGS_LSPMESSAGEBOX_H_
#define UI_TK_WIDGETS_DIALOGS_LSPMESSAGEBOX_H_

namespace lsp
{
    namespace tk
    {
        class LSPMessageBox: public LSPWindow
        {
            public:
                static const w_class_t metadata;

            protected:
                LSPLabel        sHeading;
                LSPLabel        sMessage;
                LSPAlign        sHeadAlign;
                LSPAlign        sMsgAlign;
                LSPBox          sVBox;
                LSPBox          sHBox;
                size_t          nMinBtnWidth;
                size_t          nMinBtnHeight;
                cvector<LSPButton> vButtons;

            public:
                explicit LSPMessageBox(LSPDisplay *dpy);
                virtual ~LSPMessageBox();

                virtual status_t init();
                virtual void destroy();

            protected:
                static status_t slot_on_button_submit(LSPWidget *sender, void *ptr, void *data);
                void do_destroy();

            public:
                inline LSPLabel        *heading()           { return &sHeading; }
                inline LSPLabel        *message()           { return &sMessage; }
                inline LSPButton       *button(size_t idx)  { return vButtons.get(idx); }
                inline size_t           buttons() const     { return vButtons.size(); }
                inline size_t           min_button_width() const    { return nMinBtnWidth; }
                inline size_t           min_button_height() const   { return nMinBtnHeight; }

            public:
                status_t    set_heading(const char *text);
                status_t    set_heading(const LSPString *text);

                status_t    set_message(const char *text);
                status_t    set_message(const LSPString *text);

                status_t    add_button(const char *text, ui_event_handler_t handler = NULL, void *arg = NULL);
                status_t    add_button(const LSPString *text, ui_event_handler_t handler = NULL, void *arg = NULL);

                void        clear_buttons();

                void        set_min_button_width(size_t value);
                void        set_min_button_height(size_t value);

        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_DIALOGS_LSPMESSAGEBOX_H_ */
