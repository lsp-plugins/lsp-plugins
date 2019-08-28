/*
 * LSPWindow.h
 *
 *  Created on: 16 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPWINDOW_H_
#define UI_TK_LSPWINDOW_H_

namespace lsp
{
    namespace tk
    {
        enum window_poilicy_t
        {
            WP_NORMAL,
            WP_GREEDY
        };

        class LSPWindow: public LSPWidgetContainer
        {
            protected:
                friend class LSPWindowActions;
                friend class LSPDisplay;

            public:
                static const w_class_t    metadata;

            protected:
                INativeWindow      *pWindow;
                void               *pNativeHandle;
                LSPWidget          *pChild;
                border_style_t      enStyle;
                ssize_t             nScreen;
                size_request_t      sConstraints;
                LSPTimer            sRedraw;
                LSPWidget          *pFocus;
                LSPWidget          *pPointed;
                bool                bHasFocus;
                bool                bOverridePointer;
                bool                bSizeRequest;
                bool                bMapFlag;
                float               nVertPos;
                float               nHorPos;
                float               nVertScale;
                float               nHorScale;
                LSPString           sCaption;
                LSPWindowActions    sActions;
                window_poilicy_t    enPolicy;

            //---------------------------------------------------------------------------------
            // Slot handlers
            protected:
                static status_t     tmr_redraw_request(timestamp_t ts, void *args);
                static status_t     slot_window_close(LSPWidget *sender, void *ptr, void *data);

                virtual LSPWidget  *find_widget(ssize_t x, ssize_t y);
                status_t            do_render();
                void                do_destroy();
                status_t            sync_size();
                status_t            update_pointer();

            //---------------------------------------------------------------------------------
            // Construction and destruction
            public:
                explicit LSPWindow(LSPDisplay *dpy, void *handle = NULL, ssize_t screen = -1);
                virtual ~LSPWindow();

                /** Init window
                 *
                 * @return status of operation
                 */
                virtual status_t init();

                /** Destroy window
                 *
                 */
                virtual void destroy();

            //---------------------------------------------------------------------------------
            // Properties
            public:
                /** Get native window handle
                 *
                 */
                inline void *handle() { return (pWindow != NULL) ? pWindow->handle() : NULL; };

                /**
                 * Get native window
                 * @return native window
                 */
                inline INativeWindow *native() { return pWindow; };

                /**
                 * Return true if window is a sub-window of another window
                 * @return true if window is a sub-window of another window
                 */
                inline bool nested() { return pNativeHandle != NULL; }

                /** Get border style of the window
                 *
                 * @return border style of the window
                 */
                inline border_style_t border_style() const { return enStyle; }

                inline LSPWindowActions *actions() { return &sActions; }

                inline ssize_t screen() { return (pWindow != NULL) ? pWindow->screen() : -1; };

                status_t get_absolute_geometry(realize_t *realize);

                inline LSPWidget *focused_child() const  { return const_cast<LSPWindow *>(this)->pFocus; }

                inline LSPWidget *pointed_child() const  { return const_cast<LSPWindow *>(this)->pPointed; }

                inline bool override_pointer() const { return bOverridePointer; }

                inline bool size_request_pending() const { return bSizeRequest; }

                inline const char *title() const { return sCaption.get_native(); }

                inline status_t get_title(LSPString *dst) const { return dst->set(&sCaption) ? STATUS_OK : STATUS_NO_MEM; };

                inline window_poilicy_t policy() const          { return enPolicy; }

                inline float            vpos() const            { return nVertPos; }
                inline float            hpos() const            { return nHorPos; }
                inline float            vscale() const          { return nVertScale; }
                inline float            hscale() const          { return nHorScale; }

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                virtual void        query_resize();

                virtual status_t    set_title(const char *caption);

                virtual status_t    set_title(const LSPString *value);

                /** Render window's content to surface
                 *
                 * @param s surface to perform rendering
                 * @param force force flag
                 */
                virtual void render(ISurface *s, bool force);

                virtual status_t set_cursor(mouse_pointer_t mp);

                virtual status_t override_pointer(bool override = true);

                /** Hide window
                 *
                 */
                virtual bool hide();

                /** Show window
                 *
                 */
                virtual bool show();

                /** Show window over window of actor
                 *
                 * @param actor actor
                 * @return status of operation
                 */
                virtual bool show(LSPWidget *actor);

                /** Add child widget
                 *
                 * @param widget widget to add
                 */
                virtual status_t add(LSPWidget *widget);

                /** Remove child widget
                 *
                 * @param widget child widget
                 * @return status of operation
                 */
                virtual status_t remove(LSPWidget *widget);

                /** Set border style of the window
                 *
                 * @param style border style of the window
                 */
                status_t set_border_style(border_style_t style);

                /** Handle event from window system
                 *
                 * @param e event from window system
                 */
                virtual status_t handle_event(const ws_event_t *e);

                virtual status_t set_focus(bool focus = true);

                virtual status_t toggle_focus();

                virtual bool has_focus() const;

                status_t set_width(ssize_t width);

                status_t set_height(ssize_t height);

                status_t resize(ssize_t width, ssize_t height);

                status_t set_left(ssize_t left);

                status_t set_top(ssize_t top);

                status_t move(ssize_t left, ssize_t top);

                status_t set_geometry(ssize_t left, ssize_t top, ssize_t width, ssize_t height);

                status_t set_geometry(const realize_t *geometry);

                status_t get_geometry(realize_t *geometry);

                status_t set_min_width(ssize_t width);

                status_t set_min_height(ssize_t height);

                status_t set_min_size(ssize_t width, ssize_t height);

                status_t set_max_width(ssize_t width);

                status_t set_max_height(ssize_t height);

                status_t set_max_size(ssize_t width, ssize_t height);

                status_t set_size_constraints(const size_request_t *c);

                status_t get_size_constraints(size_request_t *c);

                status_t set_size_constraints(ssize_t min_width, ssize_t min_height, ssize_t max_width, ssize_t max_height);

                status_t focus_child(LSPWidget *focus);

                status_t unfocus_child(LSPWidget *focus);

                status_t toggle_child_focus(LSPWidget *focus);

                status_t point_child(LSPWidget *focus);

                status_t grab_events();

                void set_policy(window_poilicy_t policy);

                void                    set_vpos(float value);
                void                    set_hpos(float value);
                void                    set_vscale(float value);
                void                    set_hscale(float value);

            //---------------------------------------------------------------------------------
            // Event handling
            public:
                /** Close event
                 *
                 * @param e close event
                 * @return status of operation
                 */
                virtual status_t on_close(const ws_event_t *e);

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

                /** Realize window
                 *
                 * @param r realize parameters
                 */
                virtual void realize(const realize_t *r);

                /** Request size of the window
                 *
                 * @param r pointer to structure to store data
                 */
                virtual void size_request(size_request_t *r);

                /** Set window icon
                 *
                 * @param bgra color data
                 * @param width width of icon
                 * @param height height of icon
                 * @return status of operation
                 */
                virtual status_t set_icon(const void *bgra, size_t width, size_t height);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPWINDOW_H_ */
