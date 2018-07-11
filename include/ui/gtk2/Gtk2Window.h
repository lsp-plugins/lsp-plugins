/*
 * Gtk2Window.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef UI_GTK2_GTK2WINDOW_H_
#define UI_GTK2_GTK2WINDOW_H_

namespace lsp
{
    class Gtk2Window: public Gtk2Container
    {
        private:
            enum child_t
            {
                C_ROOT,
                C_BOX,
                C_LEFT,
                C_MIDDLE,
                C_RIGHT,
                C_TOTAL,

                C_FIRST = C_ROOT
            };

            Gtk2Color   sBgColor;
            IWidget    *pWidgets[C_TOTAL];
            bool        bBody;
            ssize_t     nWidth;
            ssize_t     nHeight;
            bool        bResizable;
            bool        bMapped;
            guint       hFunction;
            guint       hMapHandler;
            guint       hUnmapHandler;
            GtkWidget  *pToplevel;

        public:
            Gtk2Window(plugin_ui *ui);
            ~Gtk2Window();

        protected:
            static void gtk_window_set_parent(GtkWidget *widget, GtkObject *prev, gpointer p_this);
            static gboolean redraw_window(gpointer ptr);
            static void map_window(GtkWidget *widget, gpointer ptr);
            static void unmap_window(GtkWidget *widget, gpointer ptr);
            void set_parent(GtkWidget *parent);

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void begin();

            virtual void add(IWidget *widget);

            virtual void end();
    };

} /* namespace lsp */

#endif /* UI_GTK2_GTK2WINDOW_H_ */
