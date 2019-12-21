/*
 * LSPMenu.h
 *
 *  Created on: 18 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPMENU_H_
#define UI_TK_WIDGETS_LSPMENU_H_

namespace lsp
{
    namespace tk
    {
        class LSPMenuItem;

        class LSPMenu: public LSPWidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                class MenuWindow: public LSPWindow
                {
                    protected:
                        LSPMenu *pMenu;

                    protected:
                        virtual LSPWidget    *find_widget(ssize_t x, ssize_t y);

                        LSPMenu             *get_handler(ws_event_t *e);

                    public:
                        MenuWindow(LSPDisplay *dpy, LSPMenu *menu, size_t screen);
                        virtual ~MenuWindow();

                    public:
                        virtual void render(ISurface *s, bool force);

                        virtual void size_request(size_request_t *r);

                        virtual status_t on_mouse_down(const ws_event_t *e);

                        virtual status_t on_mouse_up(const ws_event_t *e);

                        virtual status_t on_mouse_scroll(const ws_event_t *e);

                        virtual status_t on_mouse_move(const ws_event_t *e);
                };

                enum selection_t
                {
                    SEL_NONE            = -3,
                    SEL_TOP_SCROLL      = -2,
                    SEL_BOTTOM_SCROLL   = -1
                };

            protected:
                cvector<LSPMenuItem>    vItems;
                LSPFont                 sFont;
                MenuWindow             *pWindow;
                LSPMenu                *pParentMenu;
                LSPMenu                *pActiveMenu;
                LSPTimer                sScroll;
                ssize_t                 nPopupLeft;
                ssize_t                 nPopupTop;
                ssize_t                 nSelected;
                ssize_t                 nScroll;
                ssize_t                 nScrollMax;
                size_t                  nMBState;
                LSPColor                sSelColor;
                LSPColor                sBorderColor;
                size_t                  nBorder;
                size_t                  nSpacing;

            protected:
                ssize_t                 find_item(ssize_t x, ssize_t y, ssize_t *ry);
                static status_t         timer_handler(timestamp_t time, void *arg);
                void                    update_scroll();
                void                    selection_changed(ssize_t sel, ssize_t ry);
                LSPMenu                *check_inside_submenu(ws_event_t *e);

                void                    do_destroy();

            public:
                explicit LSPMenu(LSPDisplay *dpy);
                virtual ~LSPMenu();

                virtual status_t init();
                virtual void destroy();

            public:
                inline ssize_t  popup_left() const  { return nPopupLeft;    }
                inline ssize_t  popup_top() const   { return nPopupTop;     }
                inline size_t   border() const      { return nBorder; };
                inline size_t   spacing() const     { return nSpacing; };
                inline ssize_t  scroll() const      { return nScroll; };
                LSPColor       *sel_color()         { return &sSelColor; }
                LSPColor       *border_color()      { return &sBorderColor; }

            public:
                inline void         set_popup_left(ssize_t value) { nPopupLeft = value; }
                inline void         set_popup_top(ssize_t value) { nPopupTop = value; }
                void                set_border(size_t value);
                void                set_spacing(size_t value);
                void                set_scroll(ssize_t scroll);

                virtual status_t    add(LSPWidget *child);

                virtual status_t    remove(LSPWidget *child);

                virtual void        draw(ISurface *s);

                virtual void        query_resize();

                virtual void        size_request(size_request_t *r);

                virtual void        realize(const realize_t *r);

                virtual bool        hide();

                virtual bool        show();

                virtual bool        show(size_t screen);

                virtual bool        show(size_t screen, ssize_t left, ssize_t top);

                virtual bool        show(LSPWidget *w, size_t screen, ssize_t left, ssize_t top);

                virtual bool        show(LSPWidget *w);

                virtual bool        show(LSPWidget *w, ssize_t left, ssize_t top);

                virtual bool        show(LSPWidget *w, const ws_event_t *ev);

                virtual status_t    on_mouse_down(const ws_event_t *e);

                virtual status_t    on_mouse_up(const ws_event_t *e);

                virtual status_t    on_mouse_move(const ws_event_t *e);

                virtual status_t    on_mouse_scroll(const ws_event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPMENU_H_ */
