/*
 * LSPComboGroup.h
 *
 *  Created on: 29 апр. 2018 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPCOMBOGROUP_H_
#define UI_TK_WIDGETS_LSPCOMBOGROUP_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPComboGroup: public LSPWidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    F_OPENED        = 1 << 0,
                    F_CIRCULAR      = 1 << 1,
                    F_MOUSE_OUT     = 1 << 2
                };

                typedef struct dimensions_t
                {
                    size_t      nGapLeft;
                    size_t      nGapTop;
                    size_t      nGapRight;
                    size_t      nGapBottom;
                    size_t      nMinWidth;
                    size_t      nMinHeight;
                } dimensions_t;

                class LSPComboList: public LSPListBox
                {
                    protected:
                        LSPComboGroup *pWidget;

                    protected:
                        virtual void            on_selection_change();

                        virtual void            on_item_change(ssize_t index, LSPItem *item);

                        virtual void            on_item_add(size_t index);

                        virtual void            on_item_remove(size_t index);

                        virtual void            on_item_swap(size_t idx1, size_t idx2);

                        virtual void            on_item_clear();

                    public:
                        LSPComboList(LSPDisplay *dpy, LSPComboGroup *widget);
                        virtual ~LSPComboList();
                };

                class LSPComboPopup: public LSPWindow
                {
                    protected:
                        LSPComboGroup *pWidget;

                    protected:
                        virtual status_t    handle_event(const ws_event_t *e);

                    public:
                        LSPComboPopup(LSPDisplay *dpy, LSPComboGroup *widget, ssize_t screen = -1);
                        virtual ~LSPComboPopup();
                };

            protected:
                size_t              nRadius;
                size_t              nBorder;
                size_t              nCBFlags;
                size_t              nMFlags;
                realize_t           sGroupHdr;
                LSPColor            sColor;
                cvector<LSPWidget>  vWidgets;
                LSPComboList        sListBox;
                LSPFont             sFont;
                LSPWindow          *pPopup;
                bool                bEmbed;

            protected:
                virtual LSPWidget  *find_widget(ssize_t x, ssize_t y);
                LSPWidget          *current_widget();
                void                query_dimensions(dimensions_t *d);
                void                do_destroy();
                bool                check_mouse_over(ssize_t x, ssize_t y);

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
                const LSPLocalString   *text() const;
                inline LSPColor    *color()                 { return &sColor; }
                inline size_t       radius() const          { return nRadius; }
                inline size_t       border() const          { return nBorder; }
                inline LSPFont     *font()                  { return &sFont; }
                inline bool         embed() const           { return bEmbed; }

                ssize_t             selected() const;
                inline LSPItemList *items()                 { return sListBox.items();  }
                inline bool         opened() const          { return nCBFlags & F_OPENED;     }
                inline bool         circular() const        { return nCBFlags & F_CIRCULAR;   }
                inline LSPColor    *list_color()            { return sListBox.color(); }
                inline LSPColor    *list_bg_color()         { return sListBox.bg_color(); }

            public:
                void                set_radius(size_t value);
                void                set_border(size_t value);

                status_t            set_selected(ssize_t value);
                status_t            set_opened(bool open = true);
                inline status_t     set_closed(bool closed = true) { return set_opened(!closed); };
                void                set_circular(bool circular = true);
                void                set_embed(bool embed);

                inline status_t open()  { return set_opened(true); }
                inline status_t toggle(){ return set_opened(!(nFlags & F_OPENED)); }
                inline status_t close() { return set_opened(false); }

            public:
                explicit LSPComboGroup(LSPDisplay *dpy);
                virtual ~LSPComboGroup();

                virtual status_t init();
                virtual void destroy();

            public:
                virtual void render(ISurface *s, bool force);

                virtual status_t add(LSPWidget *widget);

                virtual status_t remove(LSPWidget *widget);

                virtual void size_request(size_request_t *r);

                virtual void realize(const realize_t *r);


                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_scroll(const ws_event_t *e);

                virtual status_t on_change();

                virtual status_t on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPCOMBOGROUP_H_ */
