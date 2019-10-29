/*
 * LSPScrollableBox.h
 *
 *  Created on: 11 окт. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPSCROLLBOX_H_
#define UI_TK_LSPSCROLLBOX_H_

namespace lsp
{
    namespace tk
    {
        class LSPScrollBox: public LSPWidgetContainer
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct cell_t
                {
                    size_request_t      r;      // Required space
                    realize_t           a;      // Allocated space
                    realize_t           s;      // Really used space
                    padding_t           p;      // Padding
                    LSPWidget          *pWidget;
                } cell_t;

                typedef struct allocation_t
                {
                    ssize_t             aw;     // Area width
                    ssize_t             ah;     // Area height
                    bool                hs;     // horizontal scroll enabled
                    bool                vs;     // Vertical scroll enabled
                    size_request_t      r;      // Actual size request
                } allocation_t;

                enum box_flags_t
                {
                    BF_PROPORTIONAL     = 1 << 0,
                    BF_HSCROLL          = 1 << 1,
                    BF_VSCROLL          = 1 << 2
                };

            protected:
                cstorage<cell_t>    vItems;
                size_t              nSpacing;
                bool                bProportional;
                bool                bHSBypass;
                bool                bVSBypass;
                orientation_t       enOrientation;
                scrolling_t         enHScroll;
                scrolling_t         enVScroll;
                ssize_t             nAreaX;
                ssize_t             nAreaY;

                LSPSizeConstraints  sConstraints;
                LSPScrollBar        sHBar;
                LSPScrollBar        sVBar;

            protected:
                static status_t     slot_on_scroll(LSPWidget *sender, void *ptr, void *data);

            protected:
                static inline bool  hidden_widget(const cell_t *w);
                size_t              visible_items();
                virtual LSPWidget  *find_widget(ssize_t x, ssize_t y);
                void                do_destroy();
                virtual void        estimate_allocation(allocation_t *alloc, const realize_t *realize);
                void                realize_children();

            public:
                explicit LSPScrollBox(LSPDisplay *dpy, bool horizontal = true);
                virtual ~LSPScrollBox();

                virtual status_t init();

                /** Destroy widget
                 *
                 */
                virtual void destroy();

            //---------------------------------------------------------------------------------
            // Properties
            public:

                /** Get cell spacing
                 *
                 * @return cell spacing
                 */
                inline size_t       spacing() const { return nSpacing; }

                /** Get proportional flag
                 *
                 * @return proportional flag
                 */
                inline bool         proportional() const { return bProportional; }

                /** Check that orientation of the box is horizontal
                 *
                 * @return true if orientation of the box is horizontal
                 */
                inline bool         horizontal() const { return enOrientation == O_HORIZONTAL; }

                /** Check that orientation of the box is vertical
                 *
                 * @return true if orientation of the box is vertical
                 */
                inline bool         vertical() const { return enOrientation == O_VERTICAL; }

                /**
                 * Get size constraints
                 * @return size constraints
                 */
                inline LSPSizeConstraints  *constraints()   { return &sConstraints; }

                /** Get box orientation
                 *
                 * @return box orientation
                 */
                inline orientation_t    orientation() const { return enOrientation; }

                inline scrolling_t      vscroll() const         { return enVScroll; }
                inline float            vscroll_bypass() const  { return bVSBypass; }
                inline float            vscroll_pos() const     { return sVBar.value(); }
                inline float            vscroll_min() const     { return sVBar.min_value(); }
                inline float            vscroll_max() const     { return sVBar.max_value(); }
                inline float            vscroll_on() const      { return sVBar.visible(); }

                inline scrolling_t      hscroll() const         { return enHScroll; }
                inline float            hscroll_bypass() const  { return bHSBypass; }
                inline float            hscroll_pos() const     { return sHBar.value(); }
                inline float            hscroll_min() const     { return sHBar.min_value(); }
                inline float            hscroll_max() const     { return sHBar.max_value(); }
                inline bool             hscroll_on() const      { return sHBar.visible(); }

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Set box spacing
                 *
                 * @param value box spacing
                 */
                void        set_spacing(size_t value);

                /** Set proportional flag
                 *
                 * @param value proportional flag
                 */
                void        set_proportional(bool value);

                /** Set orientation of the box
                 *
                 * @param value orientation
                 */
                void        set_orientation(orientation_t value);

                /**
                 * Set vertical scrolling mode
                 * @param mode scrolling mode
                 */
                void        set_vscroll(scrolling_t mode);

                /**
                 * Set vertical scrolling
                 * @param value the vertical scrolling value in pixels
                 */
                void        set_vscroll_pos(float value) { sVBar.set_value(value); }

                /**
                 * Set bypass of vertical scrolling events
                 * @param value vertical scrolling events bypass
                 */
                void        set_vscroll_bypass(bool bypass = true) { bVSBypass = bypass; }

                /**
                 * Set horizontal scrolling mode
                 * @param mode scrolling mode
                 */
                void        set_hscroll(scrolling_t mode);

                /**
                 * Set horizontal scrolling
                 * @param value horizontal scrolling value in pixels
                 */
                void        set_hscroll_pos(float value) { sHBar.set_value(value); }

                /**
                 * Set bypass of horizontal scrolling events
                 * @param value horizontal scrolling events bypass
                 */
                void        set_hscroll_bypass(bool bypass = true) { bHSBypass = bypass; }

                /** Set horizontal orientation
                 *
                 * @param value horizontal orientation flag
                 */
                inline void         set_horizontal(bool value = true)   { set_orientation((value) ? O_HORIZONTAL : O_VERTICAL); }

                /** Set vertical orientation
                 *
                 * @param value vertical orientation flag
                 */
                inline void         set_vertical(bool value = true)     { set_orientation((value) ? O_VERTICAL : O_HORIZONTAL); }

                /** render widget
                 *
                 * @param s surface
                 * @param force force flag
                 */
                virtual void render(ISurface *s, bool force);

                /** Add widget
                 *
                 * @param widget widget to add
                 * @return status of operation
                 */
                virtual status_t add(LSPWidget *widget);

                /** Remove widget
                 *
                 * @param child widget to remove
                 * @return status of operation
                 */
                virtual status_t remove(LSPWidget *child);

                /** Remove all widgets
                 *
                 * @return status of operation
                 */
                virtual status_t    remove_all();

                /** Realize the widget
                 *
                 * @param r realization structure
                 */
                virtual void realize(const realize_t *r);

                /** Size request of the widget
                 *
                 * @param r size request
                 */
                virtual void size_request(size_request_t *r);

                virtual status_t    handle_event(const ws_event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPSCROLLABLEBOX_H_ */
