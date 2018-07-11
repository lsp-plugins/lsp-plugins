/*
 * Gtk2Graph.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2GRAPH_H_
#define _UI_GTK2_GTK2GRAPH_H_

#ifdef LSP_TRACE
    #include <time.h>
#endif /* LSP_TRACE */

namespace lsp
{
    class Gtk2Canvas;
    class IGraphCanvas;

    class Gtk2Graph: public Gtk2CustomWidget, public IGraph
    {
        protected:
            ColorHolder         sColor;
            ColorHolder         sBgColor;

            size_t              nSetWidth;
            size_t              nSetHeight;
            size_t              nBorder;
            size_t              nRadius;
            size_t              nPadding;
            float               fVPos;
            float               fHPos;
            ssize_t             nGrabbingID;
            Gtk2Canvas         *pCanvas;
            cairo_surface_t    *pGlass;

            #ifdef LSP_TRACE
            struct timespec sClock;
            size_t          nFrames;
            #endif /* LSP_TRACE */

        protected:
            bool            translate_coords(ssize_t &x, ssize_t &y);
            virtual void    draw(cairo_t *cr);

        public:
            Gtk2Graph(plugin_ui *ui);
            virtual ~Gtk2Graph();

            static IGraphObject    *getGraphObject(IWidget *widget);

            virtual IGraphCanvas   *canvas();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void notify(IUIPort *port);

            virtual void resize(size_t &w, size_t &h);

            virtual void add(IWidget *widget);

            virtual void markRedraw();

            virtual void button_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_release(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void motion(ssize_t x, ssize_t y, size_t state);

            virtual void scroll(ssize_t x, ssize_t y, size_t state, size_t direction);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2GRAPH_H_ */
