/*
 * Gtk2Graph.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2GRAPH_H_
#define _UI_GTK2_GTK2GRAPH_H_

namespace lsp
{
    class Gtk2Graph: public Gtk2CustomWidget, public IGraph
    {
        protected:
            Gtk2Color       sColor;
            Gtk2Color       sBgColor;

            size_t          nSetWidth;
            size_t          nSetHeight;
            size_t          nBorder;
            size_t          nRadius;
            size_t          nPadding;
            float           fVPos;
            float           fHPos;

        public:
            Gtk2Graph(plugin_ui *ui);
            virtual ~Gtk2Graph();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void render();

            virtual void notify(IUIPort *port);

            virtual void resize(size_t &w, size_t &h);

            virtual void add(IWidget *widget);

            virtual void markRedraw();
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2GRAPH_H_ */
