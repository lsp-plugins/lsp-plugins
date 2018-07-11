/*
 * Gtk2ComboBox.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2COMBOBOX_H_
#define _UI_GTK2_GTK2COMBOBOX_H_

namespace lsp
{
    class Gtk2ComboBox: public Gtk2Widget
    {
        protected:
            ColorHolder         sColor;
            ColorHolder         sTextColor;
            ColorHolder         sBgColor;
            GtkCellRenderer    *pRenderer;
            GtkListStore       *pStore;
            IUIPort            *pPort;
            float               fMin;
            float               fMax;
            float               fStep;
            guint               hChangeHandler;
            size_t              nWidth;
            size_t              nHeight;

        protected:
            void        apply_metadata_params(const port_t *p);
            void        changed();

        protected:
            static void value_changed(GtkComboBox *widget, gpointer user_data);

        public:
            Gtk2ComboBox(plugin_ui *ui);
            virtual ~Gtk2ComboBox();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void render();

            virtual void end();

            virtual void resize(ssize_t &w, ssize_t &h);

            virtual void destroy();

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2COMBOBOX_H_ */
