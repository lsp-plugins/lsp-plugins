/*
 * Gtk2VBox.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2GRID_H_
#define _UI_GTK2_GTK2GRID_H_

namespace lsp
{
    class Gtk2Grid: public Gtk2Container
    {
        private:
            size_t      nRows;
            size_t      nCols;
            size_t      nCurrRow;
            size_t      nCurrCol;
            size_t      nVSpacing;
            size_t      nHSpacing;
            bool        bTranspose;
            uint8_t    *vBitmap;

        private:
            void bitmap_set(size_t x, size_t y);
            bool bitmap_get(size_t x, size_t y);
            void bitmap_new();
            void bitmap_delete();

        public:
            Gtk2Grid(plugin_ui *ui);
            virtual ~Gtk2Grid();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void add(IWidget *widget);

            virtual void begin();

            virtual void end();
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2GRID_H_ */
