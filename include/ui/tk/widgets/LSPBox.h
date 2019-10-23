/*
 * LSPBox.h
 *
 *  Created on: 20 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPBOX_H_
#define UI_TK_LSPBOX_H_

namespace lsp
{
    namespace tk
    {
        class LSPBox: public LSPWidgetContainer
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

            protected:
                cstorage<cell_t>    vItems;
                size_t              nSpacing;
                bool                bProportional;
                orientation_t       enOrientation;

            protected:
                static inline bool  hidden_widget(const cell_t *w);
                size_t              visible_items();
                virtual LSPWidget  *find_widget(ssize_t x, ssize_t y);
                void                do_destroy();

            public:
                explicit LSPBox(LSPDisplay *dpy, bool horizontal = true);
                virtual ~LSPBox();

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

                /** Get box orientation
                 *
                 * @return box orientation
                 */
                inline orientation_t    orientation() const { return enOrientation; }

            //---------------------------------------------------------------------------------
            // Manipulation
            public:
                /** Set box border
                 *
                 * @param value border value
                 */
                void        set_border(size_t value);

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
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPBOX_H_ */
