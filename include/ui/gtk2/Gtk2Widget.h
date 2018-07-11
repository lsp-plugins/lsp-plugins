/*
 * Gtk2Window.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef UI_GTK2_GTK2WIDGET_H_
#define UI_GTK2_GTK2WIDGET_H_

#include <core/atomic.h>

namespace lsp
{
    class Gtk2Widget : public IWidget
    {
        protected:
            enum wflags_t
            {
                F_EXPAND    = 1 << 0,
                F_FILL      = 1 << 0,
//                F_REDRAW    = 1 << 2
            };

            size_t              nAdded;
            size_t              nWFlags;
            atomic_t            lkWRedraw;
            GtkWidget          *pWidget;

        protected:
            void                update_gtk2_visibility();
            void                allowRedraw();

        public:
            Gtk2Widget(plugin_ui *ui, widget_t w_class);
            virtual ~Gtk2Widget();

            virtual GtkWidget *widget();

            virtual void add(IWidget *widget);

            virtual void set(widget_attribute_t att, const char *value);

            virtual bool expand() const;

            virtual bool fill() const;

            virtual void hide();

            virtual void show();

            virtual void draw();

            virtual void markRedraw();

        public:
            static Gtk2Widget *cast(IWidget *widget);
    };

} /* namespace lsp */

#endif /* UI_GTK2_GTK2WIDGET_H_ */
