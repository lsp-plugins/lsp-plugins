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

        protected:
            enum mode_t
            {
                M_LEFT      = 0,
                M_RIGHT     = 1,
                M_TOP       = 2,

                M_TOTAL
            };

        private:
            ColorHolder     sBgColor;
            ColorHolder     sColor;
            ColorHolder     sTextColor;
            GtkWidget      *pMenu;

            char           *sText;
            size_t          nMode;
            size_t          nSize;
            size_t          nButtons;
            bool            bPressed;
            ssize_t         nLogoLeft;
            ssize_t         nLogoTop;
            ssize_t         nLogoRight;
            ssize_t         nLogoBottom;

        private:
            void draw_screw(cairo_t *cr, size_t x, size_t y, float angle);
            bool mouse_over_logo(ssize_t x, ssize_t y);

            static void export_settings(GtkWidget *menu, gpointer data);
            static void import_settings(GtkWidget *menu, gpointer data);
            static void toggle_hide(GtkWidget *menu, gpointer data);

        protected:
            virtual void    draw(cairo_t *cr);

        public:
            Gtk2MountStud(plugin_ui *ui);
            virtual ~Gtk2MountStud();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void resize(size_t &w, size_t &h);

            virtual void button_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_release(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void motion(ssize_t x, ssize_t y, size_t state);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2MOUNTSTUD_H_ */
