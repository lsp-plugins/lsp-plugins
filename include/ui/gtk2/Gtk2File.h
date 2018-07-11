/*
 * Gtk2File.h
 *
 *  Created on: 24 мар. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_GTK2_GTK2FILE_H_
#define _UI_GTK2_GTK2FILE_H_

namespace lsp
{
    class Gtk2File: public Gtk2CustomWidget
    {
        protected:
            Color               sColor;
            Color               sBgColor;
            Color               sLineColor;
            Color               sLeftColor;
            Color               sRightColor;
            Color               sMiddleColor;
            Color               sFadeColor;
            Color               sTextColor;

            IUIPort            *pFile;
            IUIPort            *pHeadCut;
            IUIPort            *pTailCut;
            IUIPort            *pFadeIn;
            IUIPort            *pFadeOut;
            IUIPort            *pLength;
            IUIPort            *pStatus;
            IUIPort            *pMesh;
            cairo_surface_t    *pGlass;

            size_t              nBtnWidth;
            size_t              nBtnHeight;
            size_t              nBMask;
            size_t              nBorder;
            size_t              nRadius;
            bool                bPressed;

        private:
            bool        check_mouse_over(ssize_t x, ssize_t y);
            void        render_mesh(cairo_t *cr, mesh_t *mesh, size_t index, const Color &color, ssize_t x, ssize_t y, ssize_t w, ssize_t h);
            void        render_graph(cairo_surface_t *s, size_t w, size_t h);
            void        on_click();
            void        set_file(const char *fname);

        protected:
            virtual void draw(cairo_t *cr);

        public:
            Gtk2File(plugin_ui *ui);
            virtual ~Gtk2File();

        public:
            virtual void set(widget_attribute_t att, const char *value);

            virtual void resize(size_t &w, size_t &h);

            virtual void button_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_release(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void button_double_press(ssize_t x, ssize_t y, size_t state, size_t button);

            virtual void motion(ssize_t x, ssize_t y, size_t state);

            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* _UI_GTK2_GTK2FILE_H_ */
