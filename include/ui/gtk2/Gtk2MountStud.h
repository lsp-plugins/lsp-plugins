/*
 * Gtk2MountStud.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2MOUNTSTUD_H_
#define _UI_GTK2_GTK2MOUNTSTUD_H_

namespace lsp
{
    class Gtk2MountStud: public Gtk2CustomWidget
    {
        private:
            static const size_t CURVE           = 12;
            static const size_t STUD_H          = 16;
            static const size_t SCREW_SIZE      = 40;

        private:
            Color           sBgColor;
            Color           sColor;
            Color           sTextColor;

            char           *sText;
            bool            bLeft;
            size_t          nSize;

        private:
            void draw_screw(cairo_t *cr, size_t x, size_t y, float angle);

        public:
            Gtk2MountStud(plugin_ui *ui);
            virtual ~Gtk2MountStud();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void render();

            virtual void resize(size_t &w, size_t &h);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2MOUNTSTUD_H_ */
