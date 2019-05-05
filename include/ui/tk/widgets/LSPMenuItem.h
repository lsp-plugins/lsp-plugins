/*
 * LSPMenuItem.h
 *
 *  Created on: 18 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPMENUITEM_H_
#define UI_TK_WIDGETS_LSPMENUITEM_H_

namespace lsp
{
    namespace tk
    {
        class LSPMenu;

        class LSPMenuItem: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                friend class LSPMenu;

            protected:
                LSPString       sText;
                LSPMenu        *pSubmenu;
                bool            bSeparator;

            protected:
                static status_t        slot_on_submit(LSPWidget *sender, void *ptr, void *data);

            public:
                LSPMenuItem(LSPDisplay *dpy);
                virtual ~LSPMenuItem();

                virtual status_t init();

            public:
                const char         *text() const            { return sText.get_native(); }
                LSPMenu            *submenu()               { return pSubmenu; }
                inline bool         is_separator() const    { return bSeparator; }
                inline bool         has_submenu() const     { return pSubmenu != NULL; }

            public:
                status_t            set_text(const char *text);
                status_t            set_text(const LSPString *text);
                status_t            set_submenu(LSPMenu *submenu);
                status_t            set_separator(bool value);

            public:
                virtual status_t    on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPMENUITEM_H_ */
