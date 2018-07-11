/*
 * Gtk2Body.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2BODY_H_
#define _UI_GTK2_GTK2BODY_H_

namespace lsp
{
    class Gtk2Body: public Gtk2Container
    {
        public:
            Gtk2Body(plugin_ui *ui);
            virtual ~Gtk2Body();
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2BODY_H_ */
