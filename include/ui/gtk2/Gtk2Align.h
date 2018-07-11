/*
 * Gtk2Align.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2ALIGN_H_
#define _UI_GTK2_GTK2ALIGN_H_

namespace lsp
{
    class Gtk2Align: public Gtk2Container
    {
        private:
            size_t      nBorder;
            size_t      nPadLeft;
            size_t      nPadTop;
            size_t      nPadRight;
            size_t      nPadBottom;
            float       nVertPos;
            float       nHorPos;
            float       nVertScale;
            float       nHorScale;

        public:
            Gtk2Align(plugin_ui *ui);
            virtual ~Gtk2Align();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void end();
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2ALIGN_H_ */
