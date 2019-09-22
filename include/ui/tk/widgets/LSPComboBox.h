/*
 * LSPComboBox.h
 *
 *  Created on: 31 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPCOMBOBOX_H_
#define UI_TK_LSPCOMBOBOX_H_

namespace lsp
{
    namespace tk
    {
        class LSPComboBox: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                class LSPComboList: public LSPListBox
                {
                    protected:
                        LSPComboBox *pWidget;

                    protected:
                        virtual void            on_selection_change();

                        virtual void            on_item_change(ssize_t index, LSPItem *item);

                        virtual void            on_item_add(size_t index);

                        virtual void            on_item_remove(size_t index);

                        virtual void            on_item_swap(size_t idx1, size_t idx2);

                        virtual void            on_item_clear();

                    public:
                        explicit LSPComboList(LSPDisplay *dpy, LSPComboBox *widget);
                        virtual ~LSPComboList();
                };

                class LSPComboPopup: public LSPWindow
                {
                    protected:
                        LSPComboBox *pWidget;

                    protected:
                        virtual status_t    handle_event(const ws_event_t *e);

                    public:
                        explicit LSPComboPopup(LSPDisplay *dpy, LSPComboBox *widget, ssize_t screen = -1);
                        virtual ~LSPComboPopup();

//                    public:
//                        virtual void size_request(size_request_t *r);
                };

                enum flags_t
                {
                    F_OPENED        = 1 << 0,
                    F_CIRCULAR      = 1 << 1
                };

            protected:
                size_t              nCBFlags;
                ssize_t             nMinWidth;
                ssize_t             nMinHeight;
                size_t              nMFlags;

                LSPComboList        sListBox;
                LSPWindow          *pPopup;
                LSPFont             sFont;

            protected:
                void        do_destroy();
                status_t    on_list_change();
                status_t    on_list_submit();

                static status_t slot_on_change(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_list_change(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_list_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_list_focus_out(LSPWidget *sender, void *ptr, void *data);

                static status_t slot_on_list_show(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_list_mouse_down(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_list_key_down(LSPWidget *sender, void *ptr, void *data);

                ssize_t     estimate_max_size(ISurface *s);

            protected:
                virtual void            on_selection_change();
                virtual void            on_item_change(size_t index, LSPItem *item);
                virtual void            on_item_add(size_t index);
                virtual void            on_item_remove(size_t index);
                virtual void            on_item_swap(size_t idx1, size_t idx2);
                virtual void            on_item_clear();
                virtual status_t        on_list_focus_out();
                virtual status_t        on_list_show();


                virtual status_t        on_grab_mouse_down(const ws_event_t *e);
                virtual status_t        on_grab_key_down(const ws_event_t *e);

            public:
                explicit LSPComboBox(LSPDisplay *dpy);
                virtual ~LSPComboBox();

                virtual status_t init();

                virtual void destroy();

            public:
                ssize_t             selected() const;
                inline LSPItemList *items()             { return sListBox.items();  }
                inline ssize_t      min_width() const   { return nMinWidth;  }
                inline ssize_t      min_height() const  { return nMinHeight; }
                inline bool         opened() const      { return nCBFlags & F_OPENED;     }
                inline bool         circular() const    { return nCBFlags & F_CIRCULAR;   }

                inline LSPColor    *color()             { return sListBox.color(); }
                inline LSPColor    *bg_color()          { return sListBox.bg_color(); }
                inline LSPFont     *font()              { return &sFont; }

            public:
                status_t set_selected(ssize_t value);
                void set_min_width(ssize_t value);
                void set_min_height(ssize_t value);

                status_t set_opened(bool open = true);
                inline status_t set_closed(bool closed = true) { return set_opened(!closed); };

                void set_circular(bool circular = true);

                inline status_t open()  { return set_opened(true); }
                inline status_t toggle(){ return set_opened(!(nCBFlags & F_OPENED)); }
                inline status_t close() { return set_opened(false); }

            public:
                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_scroll(const ws_event_t *e);

                virtual status_t on_change();

                virtual status_t on_submit();

                virtual void size_request(size_request_t *r);

                virtual void draw(ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPCOMBOBOX_H_ */
