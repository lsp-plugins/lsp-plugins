/*
 * Window.h
 *
 *  Created on: 10 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_WINDOW_H_
#define UI_X11_WINDOW_H_

namespace lsp
{
    namespace x11ui
    {
        class X11Window: public X11Widget
        {
            protected:
                X11Core            *pCore;
                Window              hWindow;
                X11CairoSurface    *pSurface;

            protected:
                void drop_surface();
                void do_mapping(bool mapped);

            public:
                X11Window(plugin_ui *ui, X11Core *core);
                virtual ~X11Window();

            public:
                int init();
                void destroy();

                inline Window x11handle() const { return hWindow; };
                void setVisibility(bool visible);
                inline bool getVisibility() const { return bVisible; }
                void show();
                void hide();

                void render(ISurface *surface);

            public:
                inline size_t width() const { return nWidth; }
                void setWidth(size_t width);

                inline size_t height() const { return nHeight; }
                void setHeight(size_t height);

            public:
                virtual void handleEvent(const ui_event_t *ev);
        };
    
    } /* namespace x11ui */
} /* namespace lsp */

#endif /* UI_X11_WINDOW_H_ */
