/*
 * Gtk2Cell.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2CELL_H_
#define _UI_GTK2_GTK2CELL_H_

namespace lsp
{
    class Gtk2Cell: public Gtk2WidgetProxy
    {
        private:
            size_t  nRowSpan;
            size_t  nColSpan;

        public:
            Gtk2Cell(plugin_ui *ui);
            virtual ~Gtk2Cell();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            inline size_t getRowspan() { return nRowSpan; }
            inline size_t getColspan() { return nColSpan; }
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2CELL_H_ */
