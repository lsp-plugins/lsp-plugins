/*
 * Window.h
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_WINDOW_H_
#define UI_X11_WINDOW_H_

#ifndef UI_X11_WS_H_INCL_
    #error "This header should not be included directly"
#endif /* UI_X11_WS_H_INCL_ */

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            class X11Window: public INativeWindow, public IEventHandler
            {
                protected:
                    enum flags_t
                    {
                        F_GRABBING      = 1 << 0,
                        F_LOCKING       = 1 << 1,
                        F_SYNC_WM       = 1 << 2
                    };

                public:
                    X11Display         *pX11Display;
                    ::Window            hWindow;
                    ::Window            hParent;
                    X11CairoSurface    *pSurface;
                    border_style_t      enBorderStyle;
                    ws_event_t          vMouseUp[2];
                    motif_hints_t       sMotif;
                    size_t              nActions;
                    size_t              nScreen;
                    size_t              nFlags;
                    mouse_pointer_t     enPointer;
                    bool                bWrapper;
//                    IClipboard         *pClipboard[_CBUF_TOTAL];

                    realize_t           sSize;
                    size_request_t      sConstraints;

                protected:
                    void    drop_surface();
                    void    do_create();
                    static bool check_double_click(const ws_event_t *pe, const ws_event_t *ce);

                protected:

                    void        calc_constraints(realize_t *dst, const realize_t *req);

                    status_t    do_update_constraints();

                public:
                    explicit X11Window(X11Display *core, size_t screen, ::Window wnd, IEventHandler *handler, bool wrapper);
                    virtual ~X11Window();

                    /** Window initialization routine
                     *
                     * @return status of operation
                     */
                    virtual status_t init();

                    /** Window finalization routine
                     *
                     */
                    virtual void destroy();

                public:
                    /** Get event handler
                     *
                     * @return event handler
                     */
                    inline IEventHandler *get_handler() { return pHandler; }

                    /** Get surface for drawing
                     *
                     * @return surface for drawing
                     */
                    virtual ISurface *get_surface();

                    /** Get left coordinate of window
                     *
                     * @return value
                     */
                    virtual ssize_t left();

                    /** Get top coordinate of window
                     *
                     * @return value
                     */
                    virtual ssize_t top();

                    /** Get width of the window
                     *
                     * @return value
                     */
                    virtual ssize_t width();

                    /** Get height of the window
                     *
                     * @return value
                     */
                    virtual ssize_t height();

                    /** Get window visibility
                     *
                     * @return true if window is visible
                     */
                    virtual bool is_visible();

                    /** Get native handle
                     *
                     */
                    virtual void *handle();

                    /** Get window's screen
                     *
                     * @return window's screen
                     */
                    virtual size_t screen();

                    virtual status_t set_caption(const char *ascii, const char *utf8);

                    inline ::Window x11handle() const { return hWindow; }

                    inline ::Window x11parent() const { return hParent; }

                public:
                    /** Handle X11 event
                     *
                     * @param ev event to handle
                     * @return status of operation
                     */
                    virtual status_t handle_event(const ws_event_t *ev);

                    /** Set event handler
                     *
                     * @param handler event handler
                     */
                    inline void set_handler(IEventHandler *handler)
                    {
                        pHandler = handler;
                    }

                    /** Move window
                     *
                     * @param left left coordinate
                     * @param top top coordinate
                     * @return status of operation
                     */
                    virtual status_t move(ssize_t left, ssize_t top);

                    /** Resize window
                     *
                     * @param width window width
                     * @param height window height
                     * @return status of operation
                     */
                    virtual status_t resize(ssize_t width, ssize_t height);

                    /** Set window geometry
                     *
                     * @param realize window realization structure
                     * @return status of operation
                     */
                    virtual status_t set_geometry(const realize_t *realize);

                    /** Set window's border style
                     *
                     * @param style window's border style
                     * @return status of operation
                     */
                    virtual status_t set_border_style(border_style_t style);

                    /** Get window's border style
                     *
                     * @param style window's border style
                     * @return status of operation
                     */
                    virtual status_t get_border_style(border_style_t *style);

                    /** Get window geometry
                     *
                     * @return window geometry
                     */
                    virtual status_t get_geometry(realize_t *realize);

                    virtual status_t get_absolute_geometry(realize_t *realize);

                    /** Hide window
                     *
                     * @return status of operation
                     */
                    virtual status_t hide();

                    /** Show window
                     *
                     * @return status of operation
                     */
                    virtual status_t show();

                    virtual status_t show(INativeWindow *over);

                    /**
                     * Grab events from the screen
                     * @param group grab group
                     * @return status of operation
                     */
                    virtual status_t grab_events(grab_t group);

                    /**
                     * Ungrab currently selected group of events
                     * @return status of operation
                     */
                    virtual status_t ungrab_events();

                    /** Set left coordinate of the window
                     *
                     * @param left left coordinate of the window
                     * @return status of operation
                     */
                    virtual status_t set_left(ssize_t left);

                    /** Set top coordinate of the window
                     *
                     * @param top top coordinate of the window
                     * @return status of operation
                     */
                    virtual status_t set_top(ssize_t top);

                    /** Set width of the window
                     *
                     * @param width width of the window
                     * @return status of operation
                     */
                    virtual ssize_t set_width(ssize_t width);

                    /** Set height of the window
                     *
                     * @param height height of the window
                     * @return status of operation
                     */
                    virtual ssize_t set_height(ssize_t height);

                    /** Set size constraints
                     *
                     * @param c size constraints
                     * @return status of operations
                     */
                    virtual status_t set_size_constraints(const size_request_t *c);

                    /** Get size constraints
                     *
                     * @param c size constraints
                     * @return status of operation
                     */
                    virtual status_t get_size_constraints(size_request_t *c);

                    /** Check constraints
                     *
                     * @return status of operation
                     */
                    virtual status_t check_constraints();

                    /** Set focus
                     *
                     * @param focus set/unset focus flag
                     * @return status of operation
                     */
                    virtual status_t set_focus(bool focus);

                    /** Toggle focus state
                     *
                     * @return status of operation
                     */
                    virtual status_t toggle_focus();

                    /** Get caption of the window
                     *
                     * @param text buffer to store data
                     * @param len length of bufer
                     * @return status of operation
                     */
                    virtual status_t get_caption(char *text, size_t len);

                    /** Set window icon
                     *
                     * @param bgra picture data in BGRA format
                     * @param width picture width
                     * @param height picture height
                     * @return status of operation
                     */
                    virtual status_t set_icon(const void *bgra, size_t width, size_t height);

                    /** Get bitmask of allowed window actions
                     *
                     * @param actions pointer to store action mask
                     * @return status of operation
                     */
                    virtual status_t get_window_actions(size_t *actions);

                    /** Set bitmask of allowed window actions
                     *
                     * @param actions action mask
                     * @return status of operation
                     */
                    virtual status_t set_window_actions(size_t actions);

                    /** Set mouse pointer
                     *
                     * @param ponter mouse pointer
                     * @return status of operation
                     */
                    virtual status_t set_mouse_pointer(mouse_pointer_t ponter);

                    /** Get mouse pointer
                     *
                     * @return mouse pointer
                     */
                    virtual mouse_pointer_t get_mouse_pointer();

                    /**
                     * Set window class
                     * @param instance window instance, ASCII-string
                     * @param wclass window class, ASCII-string
                     * @return status of operation
                     */
                    virtual status_t set_class(const char *instance, const char *wclass);

                    /**
                     * Set window role
                     * @param wrole window role, ASCII-string
                     * @return status of operation
                     */
                    virtual status_t set_role(const char *wrole);
            };
        }
    
    } /* namespace x11ui */
} /* namespace lsp */

#endif /* UI_X11_WINDOW_H_ */
