/*
 * LSPListBox.h
 *
 *  Created on: 2 авг. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPLISTBOX_H_
#define UI_TK_WIDGETS_LSPLISTBOX_H_

namespace lsp
{
    namespace tk
    {
        class LSPListBox: public LSPComplexWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                class LSPListBoxList: public LSPItemList
                {
                    protected:
                        LSPListBox     *pWidget;

                    protected:
                        virtual void            on_item_change(LSPListItem *item); // Triggered when the content of item has been changed

                        virtual void            on_item_add(size_t index);

                        virtual void            on_item_remove(size_t index);

                        virtual void            on_item_swap(size_t idx1, size_t idx2);

                        virtual void            on_item_clear();

                    public:
                        explicit LSPListBoxList(LSPListBox *widget);
                        virtual ~LSPListBoxList();
                };

                class LSPListBoxSelection: public LSPItemSelection
                {
                    protected:
                        LSPListBox     *pWidget;

                    protected:
                        virtual void on_remove(ssize_t value);

                        virtual void on_add(ssize_t value);

                        virtual bool validate(ssize_t value);

                        virtual void request_fill(ssize_t *first, ssize_t *last);

                        virtual void on_fill();

                        virtual void on_clear();

                    public:
                        explicit LSPListBoxSelection(LSPListBox *widget);
                        virtual ~LSPListBoxSelection();
                };

                enum flags_t
                {
                    F_MDOWN         = 1 << 0,
                    F_SUBMIT        = 1 << 1
                };

            protected:
                LSPListBoxList          sItems;
                LSPListBoxSelection     sSelection;
                LSPScrollBar            sHBar;
                LSPScrollBar            sVBar;
                LSPSizeConstraints      sConstraints;
                LSPColor                sColor;
                LSPFont                 sFont;
                realize_t               sArea;
                size_t                  nFlags;
                size_t                  nBMask;
                ISurface               *pArea;

            protected:
                static status_t slot_on_change(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_sbar_vscroll(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_sbar_hscroll(LSPWidget *sender, void *ptr, void *data);

                static status_t slot_on_vscroll(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_hscroll(LSPWidget *sender, void *ptr, void *data);

                virtual LSPWidget    *find_widget(ssize_t x, ssize_t y);

                void on_click(ssize_t x, ssize_t y);
                void do_destroy();

            protected:
                virtual void            on_item_change(size_t index, LSPItem *item);

                virtual void            on_item_add(size_t index);

                virtual void            on_item_remove(size_t index);

                virtual void            on_item_swap(size_t idx1, size_t idx2);

                virtual void            on_item_clear();

                virtual void            on_selection_change();

            public:
                explicit LSPListBox(LSPDisplay *dpy);
                virtual ~LSPListBox();

                virtual status_t init();
                virtual void destroy();

            public:
                inline LSPItemList         *items()         { return &sItems; }

                inline LSPItemSelection    *selection()     { return &sSelection; }

                inline LSPColor            *color()         { return &sColor; }

                inline LSPSizeConstraints  *constraints()   { return &sConstraints; }

                inline LSPFont             *font()          { return &sFont; }

                inline float                vscroll() const     { return sVBar.value(); }
                inline float                vscroll_min() const { return sVBar.min_value(); }
                inline float                vscroll_max() const { return sVBar.max_value(); }
                inline float                vscroll_on() const  { return sVBar.visible(); }
                inline float                hscroll() const     { return sHBar.value(); }
                inline float                hscroll_min() const { return sHBar.min_value(); }
                inline float                hscroll_max() const { return sHBar.max_value(); }
                inline bool                 hscroll_on() const  { return sHBar.visible(); }

            public:
                void set_min_width(ssize_t value);

                void set_min_height(ssize_t value);

                void set_vscroll(float value) { sVBar.set_value(value); }
                void set_hscroll(float value) { sHBar.set_value(value); }

            public:
                virtual void size_request(size_request_t *r);

                virtual void optimal_size_request(size_request_t *r);

                virtual void realize(const realize_t *r);

                virtual status_t on_change();

                virtual status_t on_submit();

                virtual status_t on_hscroll();

                virtual status_t on_vscroll();

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_mouse_scroll(const ws_event_t *e);

                virtual void render(ISurface *s, bool force);

                virtual void draw(ISurface *s);

                virtual bool hide();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPLISTBOX_H_ */
