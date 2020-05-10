/*
 * LSPWidget.h
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPWIDGET_H_
#define UI_TK_LSPWIDGET_H_

namespace lsp
{
    namespace tk
    {
        class LSPComplexWidget;

        /** Basic widget class for any widget in the toolkit
         *
         */
        class LSPWidget: public ws::IEventHandler
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    REDRAW_SURFACE  = 1 << 0,       // Need to redraw surface
                    REDRAW_CHILD    = 1 << 1,       // Need to redraw child only
                    F_VISIBLE       = 1 << 2,       // Widget is visible
                    F_REALIZED      = 1 << 3,       // Widget is realized
                    F_EXPAND        = 1 << 4,       // Area for the widget should be expanded
                    F_HFILL         = 1 << 5,       // Widget should desirable fill all the provided area horizontally
                    F_VFILL         = 1 << 6,       // Widget should desirable fill all the provided area vertically
                };

            protected:
                char               *pUID;           // Unique widget identifier

                LSPDisplay         *pDisplay;
                ISurface           *pSurface;

                LSPComplexWidget   *pParent;
                const w_class_t    *pClass;

                realize_t           sSize;          // Geometry
                size_t              nFlags;         // Flags
                mouse_pointer_t     enCursor;

                LSPSlotSet          sSlots;         // Slots
                LSPPadding          sPadding;

                LSPColor            sBgColor;       // Widget color
                LSPFloat            sBrightness;    // Brightness
                LSPStyle            sStyle;         // Style

            //---------------------------------------------------------------------------------
            // Slot handlers
            protected:
                static status_t slot_mouse_move(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_down(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_up(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_dbl_click(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_tri_click(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_scroll(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_in(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_mouse_out(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_key_down(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_key_up(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_hide(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_show(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_destroy(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_resize(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_resize_parent(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_focus_in(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_focus_out(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_drag_request(LSPWidget *sender, void *ptr, void *data);

            //---------------------------------------------------------------------------------
            // Interface for nested classes
            protected:
                void            do_destroy();

                void            unlink_widget(LSPWidget *widget);

                void            init_color(color_t value, LSPColor *color);

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit LSPWidget(LSPDisplay *dpy);
                virtual ~LSPWidget();

                /** Initialize wiget
                 *
                 */
                virtual status_t init();

                /** Destroy widget
                 *
                 */
                virtual void destroy();

            //---------------------------------------------------------------------------------
            // Metadata, casting and type information
            public:
                /** Get widget class
                 *
                 * @return actual widget class metadata
                 */
                inline const w_class_t *get_class() const { return pClass; }

                /** Check wheter the widget is instance of some class
                 *
                 * @param wclass widget class
                 * @return true if widget is instance of some class
                 */
                bool instance_of(const w_class_t *wclass) const;

                inline bool instance_of(const w_class_t &wclass) const { return instance_of(&wclass); }

                /** Another way to check if widget is instance of some class
                 *
                 * @return true if widget is instance of some class
                 */
                template <class LSPTarget>
                    inline bool instance_of() const { return instance_of(&LSPTarget::metadata); };

                /** Cast widget to another type
                 *
                 * @return pointer to widget or NULL if cast failed
                 */
                template <class LSPTarget>
                    inline LSPTarget *cast() { return instance_of(&LSPTarget::metadata) ? static_cast<LSPTarget *>(this) : NULL; }

                /** Cast widget to another type
                 *
                 * @return pointer to widget or NULL if cast failed
                 */
                template <class LSPTarget>
                    inline const LSPTarget *cast() const { return instance_of(&LSPTarget::metadata) ? static_cast<const LSPTarget *>(this) : NULL; }

                /** Get pointer to self as pointer to LSPWidget class
                 *
                 * @return pointer to self
                 */
                inline LSPWidget *self()              { return this;  }

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /** Get display
                 *
                 * @return display
                 */
                inline LSPDisplay *display()        { return pDisplay; };

                /** Get horizontal coordinate of the left top corner
                 *
                 * @return coordinate of the left corner
                 */
                inline ssize_t left() const         { return sSize.nLeft;       };

                /** Get horizontal coordinate of the right bottom corner
                 *
                 * @return value
                 */
                inline ssize_t right() const        { return sSize.nLeft + sSize.nWidth;    };

                /** Get vertical coordinate of the left top corner
                 *
                 * @return value
                 */
                inline ssize_t top() const          { return sSize.nTop;        };

                /** Get vertical coordinate of the right bottom corner
                 *
                 * @return value
                 */
                inline ssize_t bottom() const       { return sSize.nTop + sSize.nHeight;     };

                /** Get width of the widget
                 *
                 * @return width of the widget
                 */
                inline ssize_t width() const        { return sSize.nWidth;      };

                /** Get height of the widget
                 *
                 * @return height of the widget
                 */
                inline ssize_t height() const       { return sSize.nHeight;     };

                /** Get widget dimensions
                 *
                 * @param r real widget dimensions
                 */
                inline void get_dimensions(realize_t *r) { *r = sSize; }

                /** Check if there is redraw request pending
                 *
                 * @return true if there is redraw request pending
                 */
                inline bool redraw_pending() const  { return nFlags & (REDRAW_SURFACE | REDRAW_CHILD); };

                /** Check that widget is visible
                 *
                 * @return true if widget is visible
                 */
                inline bool visible() const         { return nFlags & F_VISIBLE; };

                /** Check that widget is hidden
                 *
                 * @return true if widget is visible
                 */
                inline bool hidden() const          { return !(nFlags & F_VISIBLE); };
                inline bool invisible() const       { return !(nFlags & F_VISIBLE); };

                /** Get expanding flag: true if container should desirable expand area provided for the widget
                 *
                 * @return expanding flag
                 */
                inline bool expand() const          { return nFlags & F_EXPAND; }

                /** Get fill flag: true if container should desirable fill all provided area with widget
                 *
                 * @return fill flag
                 */
                inline bool fill() const            { return (nFlags & (F_HFILL | F_VFILL)) == (F_HFILL | F_VFILL); }

                /** Get horizontal fill flag: true if container should horizontally fill all provided area with widget
                 *
                 * @return horizontal fill flag
                 */
                inline bool hfill() const           { return nFlags & F_HFILL; }

                /** Get vertical fill flag: true if container should vertically fill all provided area with widget
                 *
                 * @return vertical fill flag
                 */
                inline bool vfill() const           { return nFlags & F_VFILL; }

                /** Check that widget has focus
                 *
                 * @return widget focus
                 */
                virtual bool has_focus() const;

                /** Check that widget has focus
                 *
                 * @return widget focus
                 */
                inline bool focused() const         { return has_focus(); };

                /** Check that widget is visible
                 *
                 * @return true if widget is visible
                 */
                inline bool is_visible() const      { return nFlags & F_VISIBLE; };

                /** Check that widget is realized
                 *
                 * @return true if widget is realized
                 */
                inline bool is_realized() const     { return nFlags & F_REALIZED; };

                /** Get horizontal coordinate of the left top corner relative to the parent widget
                 *
                 * @return the value
                 */
                ssize_t relative_left() const;

                /** Get horizontal coordinate of the right bottom corner relative to the parent widget
                 *
                 * @return the value
                 */
                ssize_t relative_right() const;

                /** Get vertical coordinate of the left top corner relative to the parent widget
                 *
                 * @return the value
                 */
                ssize_t relative_top() const;

                /** Get vertical coordinate of the right bottom corner relative to the parent widget
                 *
                 * @return the value
                 */
                ssize_t relative_bottom() const;

                /**
                 * Get unique widget identifier for DOM search
                 * @return unique widget identifier for DOM search
                 */
                inline const char *unique_id() const {  return pUID;    }

                /** Check that specified window coordinate lies within widget's bounds
                 * Always returns false for invisible widgets
                 *
                 * @param x x coordinate
                 * @param y y coordinate
                 * @return true on success
                 */
                virtual bool inside(ssize_t x, ssize_t y);

                /** Get parent widget
                 *
                 * @return parent widget
                 */
                inline LSPComplexWidget *parent()           { return pParent; }

                /** Get slots
                 *
                 * @return slots
                 */
                inline LSPSlotSet *slots()                  { return &sSlots; }

                /** Get slot
                 *
                 * @param id slot identifier
                 * @return pointer to slot or NULL
                 */
                inline LSPSlot *slot(ui_slot_t id)          { return sSlots.slot(id); }

                /** Get mouse pointer
                 *
                 * @return mouse pointer
                 */
                inline mouse_pointer_t cursor() const       { return enCursor; }

                /** Get active curstor
                 *
                 * @return active cursor
                 */
                virtual mouse_pointer_t active_cursor() const;

                /** Get widget padding
                 *
                 * @return widget padding
                 */
                inline LSPPadding  *padding()               { return &sPadding; };

                /**
                 * Get background color of the widget
                 * @return background color of the widget
                 */
                inline LSPColor    *bg_color()              { return &sBgColor;     }

                /**
                 * Return widget's style
                 * @return widget's style
                 */
                inline LSPStyle    *style()                 { return &sStyle; }

                /**
                 * Get brightness
                 * @return brightness
                 */
                inline float        brightness() const      { return sBrightness.get(); }

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /**
                 * Set unique widget identifier for DOM search
                 * @param uid unique widget identifier for DOM search
                 * @return status of operation
                 */
                status_t            set_unique_id(const char *uid);

                /** Query widget for redraw
                 *
                 * @param flags redraw flags
                 */
                virtual void        query_draw(size_t flags = REDRAW_SURFACE);

                /**
                 * Put the widget to the destroy queue of the main loop
                 * @return status of operation
                 */
                virtual status_t    queue_destroy();

                /** Query widget for resize
                 *
                 */
                virtual void        query_resize();

                /** Set expanding flag
                 *
                 * @param value expanding flag value
                 */
                virtual void        set_expand(bool value = true);

                /** Set fill flag
                 *
                 * @param value filling flag value
                 */
                virtual void        set_fill(bool value = true);

                /** Set horizontal fill flag
                 *
                 * @param value horizontal filling flag value
                 */
                virtual void        set_hfill(bool value = true);

                /** Set vertical fill flag
                 *
                 * @param value vertical filling flag value
                 */
                virtual void        set_vfill(bool value = true);

                /**
                 * Set brightness of the widget
                 * @param brightness brightness
                 * @return previous value
                 */
                inline float        set_brightness(float brightness) { return sBrightness.set(brightness); }

                /** Set mouse pointer
                 *
                 * @param mp mouse pointer
                 * @return mouse pointer
                 */
                virtual status_t    set_cursor(mouse_pointer_t mp);

                /** Get widget surface
                 *
                 * @param s base surface
                 * @return widget surface or NULL
                 */
                ISurface           *get_surface(ISurface *s);

                /** Get widget surface
                 *
                 * @param s base surface
                 * @param width requested width
                 * @param height requested height
                 * @return widget surface or NULL
                 */
                ISurface           *get_surface(ISurface *s, ssize_t width, ssize_t height);

                /** Render widget to the external surface
                 *
                 * @param surface surface to perform rendering
                 * @param force force child rendering
                 */
                virtual void        render(ISurface *s, bool force);

                /** Draw widget on the internal surface
                 *
                 * @param surface surface to perform drawing
                 */
                virtual void        draw(ISurface *s);

                /** Realize widget
                 *
                 * @param r widget realization parameters
                 */
                virtual void        realize(const realize_t *r);

                /** Request for size
                 *
                 * @param r minimum and maximum dimensions of the widget
                 */
                virtual void        size_request(size_request_t *r);

                /** Hide widget
                 *
                 */
                virtual bool        hide();

                /** Show widget
                 *
                 */
                virtual bool        show();

                /** Set widget visibility
                 *
                 * @param visible widget visibility
                 */
                virtual void        set_visible(bool visible=true);

                /** Set focus on widget
                 *
                 * @param focus focusing parameter
                 * @return status of operation
                 */
                virtual status_t    set_focus(bool focus = true);

                /** Kill focus on widget
                 *
                 * @return status of operation
                 */
                inline status_t     kill_focus() { return set_focus(false); };

                /** Kill focus on widget
                 *
                 * @return status of operation
                 */
                inline status_t     take_focus() { return set_focus(true); };

                /** Mark this widget to be currently pointed by mouse
                 *
                 * @return status of operation
                 */
                status_t            mark_pointed();

                /** Invert focus
                 *
                 * @return status of operation
                 */
                virtual status_t    toggle_focus();

                /** Set widget invisibility
                 *
                 * @param invisible widget invisibility
                 */
                inline void         set_invisible(bool invisible=true) { set_visible(!invisible); }

                /** Handle UI event from the display
                 *
                 * @param e UI event
                 * @return status of operation
                 */
                virtual status_t    handle_event(const ws_event_t *e);

                /** Set parent widget of this widget
                 *
                 * @param parent parent widget
                 */
                void                set_parent(LSPComplexWidget *parent);

                /** Commit widet redraw
                 *
                 */
                virtual void        commit_redraw();

                /** Get most top-level widget
                 *
                 * @return most top-level widget
                 */
                LSPWidget          *toplevel();

            //---------------------------------------------------------------------------------
            // Event handling
            public:
                /** Widget has taken focus
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_focus_in(const ws_event_t *e);

                /** Widget has lost focus
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_focus_out(const ws_event_t *e);

                /** Handle key press event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_key_down(const ws_event_t *e);

                /** Handle key release event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_key_up(const ws_event_t *e);

                /** Handle mouse button press event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_down(const ws_event_t *e);

                /** Handle mouse button release event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_up(const ws_event_t *e);

                /** Handle mouse motion event
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_move(const ws_event_t *e);

                /** Handle mouse cursor moved into the zone of widget
                 *
                 * @param e mouse event that triggered MouseIn
                 * @return status of operation
                 */
                virtual status_t on_mouse_in(const ws_event_t *e);

                /** Handle mouse cursor moved outside the zone of widget
                 *
                 * @param e mouse event that triggered MouseIn
                 * @return status of operation
                 */
                virtual status_t on_mouse_out(const ws_event_t *e);

                /** Handle mouse scroll event
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_scroll(const ws_event_t *e);

                /** Handle mouse double click event
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_dbl_click(const ws_event_t *e);

                /** Handle mouse triple click
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_mouse_tri_click(const ws_event_t *e);

                /** Geometry has changed: size or position
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_resize(const realize_t *r);

                /** Geometry of parent widget has changed: size or position
                 *
                 * @param e event
                 * @return status of operation
                 */
                virtual status_t on_resize_parent(const realize_t *r);

                /** The widget becomes hidden
                 *
                 * @return status of operation
                 */
                virtual status_t on_hide();

                /** The widget becomes visible
                 *
                 * @return status of operation
                 */
                virtual status_t on_show();

                /** The widget becomes destroyed
                 *
                 * @return status of operation
                 */
                virtual status_t on_destroy();

                /**
                 * Process the drag request event
                 * @param e drag request event
                 * @param ctype NULL-terminated list of provided content types
                 * @return status of operation
                 */
                virtual status_t on_drag_request(const ws_event_t *e, const char * const *ctype);
        };

        template <class LSPTarget>
            inline LSPTarget *widget_cast(LSPWidget *src)
            {
                return ((src != NULL) && (src->instance_of(&LSPTarget::metadata))) ? static_cast<LSPTarget *>(src) : NULL;
            }

        template <class LSPTarget>
            inline const LSPTarget *widget_cast(const LSPWidget *src)
            {
                return ((src != NULL) && (src->instance_of(&LSPTarget::metadata))) ? static_cast<const LSPTarget *>(src) : NULL;
            }

        template <class LSPTarget>
            inline LSPTarget *widget_ptrcast(void *src)
            {
                LSPWidget *w = (src != NULL) ? static_cast<LSPWidget *>(src) : NULL;
                return ((w != NULL) && (w->instance_of(&LSPTarget::metadata))) ? static_cast<LSPTarget *>(w) : NULL;
            }

        template <class LSPTarget>
            inline const LSPTarget *widget_ptrcast(const void *src)
            {
                const LSPWidget *w = (src != NULL) ? static_cast<const LSPWidget *>(src) : NULL;
                return ((w != NULL) && (w->instance_of(&LSPTarget::metadata))) ? static_cast<const LSPTarget *>(w) : NULL;
            }
    }

} /* namespace lsp */

#endif /* UI_TK_LSPWIDGET_H_ */
