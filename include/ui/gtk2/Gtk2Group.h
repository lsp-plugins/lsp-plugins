/*
 * Gtk2Group.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2GROUP_H_
#define _UI_GTK2_GTK2GROUP_H_

namespace lsp
{
    class Gtk2Group: public Gtk2Widget
    {
        private:
            char       *sText;
            Color       sColor;
            Color       sTextColor;
            Color       sBgColor;
            size_t      sRadius;
            size_t      nBorder;

        public:
            Gtk2Group(plugin_ui *ui);
            virtual ~Gtk2Group();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void render();

            virtual void end();

            virtual void resize(ssize_t &w, ssize_t &h);

            virtual void destroy();
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2GROUP_H_ */
