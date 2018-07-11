/*
 * Gtk2Box.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2BOX_H_
#define _UI_GTK2_GTK2BOX_H_

namespace lsp
{
    class Gtk2Box: public Gtk2Container
    {
        private:
            ssize_t     nSize;
            size_t      nBorder;
            size_t      nSpacing;
            bool        bProportional;

        public:
            Gtk2Box(plugin_ui *ui, bool horizontal);
            virtual ~Gtk2Box();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void end();

            virtual void add(IWidget *widget);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2BOX_H_ */
