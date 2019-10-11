/*
 * X11CairoSurface.h
 *
 *  Created on: 25 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_X11_X11CAIROSURFACE_H_
#define UI_X11_X11CAIROSURFACE_H_

#ifndef UI_X11_WS_H_INCL_
    #error "This header should not be included directly"
#endif /* UI_X11_WS_H_INCL_ */

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            class X11CairoSurface: public ISurface
            {
                protected:
                    cairo_surface_t    *pSurface;
                    cairo_t            *pCR;
                    bool                bBegin;

                protected:
                    void destroy_context();

                    inline void setSourceRGB(const Color &col);
                    inline void setSourceRGBA(const Color &col);
                    void drawRoundRect(float left, float top, float width, float height, float radius, size_t mask);

                public:
                    /** Create XLib surface
                     *
                     * @param dpy display
                     * @param drawabledrawable
                     * @param visual visual
                     * @param width surface width
                     * @param height surface height
                     */
                    X11CairoSurface(Display *dpy, Drawable drawable, Visual *visual, size_t width, size_t height);

                    /** Create image surface
                     *
                     * @param width surface width
                     * @param height surface height
                     */
                    X11CairoSurface(size_t width, size_t height);

                    /** Destructor
                     *
                     */
                    virtual ~X11CairoSurface();

                public:
                    /** resize cairo surface if possible
                     *
                     * @param width new width
                     * @param height new height
                     * @return true on success
                     */
                    bool resize(size_t width, size_t height);

                    virtual ISurface *create(size_t width, size_t height);

                    virtual ISurface *create_copy();

                    virtual IGradient *linear_gradient(float x0, float y0, float x1, float y1);

                    virtual IGradient *radial_gradient
                    (
                        float cx0, float cy0, float r0,
                        float cx1, float cy1, float r1
                    );

                    virtual void destroy();

                public:
                    // Drawing methods
                    virtual void draw(ISurface *s, float x, float y);

                    virtual void draw(ISurface *s, float x, float y, float sx, float sy);

                    virtual void draw_alpha(ISurface *s, float x, float y, float sx, float sy, float a);

                    virtual void draw_rotate_alpha(ISurface *s, float x, float y, float sx, float sy, float ra, float a);

                    virtual void draw_clipped(ISurface *s, float x, float y, float sx, float sy, float sw, float sh);

                    virtual void begin();

                    virtual void end();

                    virtual void clear_rgb(uint32_t color);

                    virtual void clear_rgba(uint32_t color);

                    virtual void fill_rect(float left, float top, float width, float height, const Color &color);

                    virtual void fill_rect(float left, float top, float width, float height, IGradient *g);

                    virtual void wire_rect(float left, float top, float width, float height, float line_width, const Color &color);

                    virtual void wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, const Color &color);

                    virtual void wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, IGradient *g);

                    virtual void fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, const Color &color);

                    virtual void fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, IGradient *g);

                    virtual void full_rect(float left, float top, float width, float height, float line_width, const Color &color);

                    virtual void fill_round_rect(float left, float top, float width, float height, float radius, const Color &color);

                    virtual void fill_round_rect(float left, float top, float width, float height, float radius, IGradient *g);

                    virtual void fill_sector(float cx, float cy, float radius, float angle1, float angle2, const Color &color);

                    virtual void fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, IGradient *g);

                    virtual void fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, const Color &color);

                    virtual void clear(const Color &color);

                    virtual bool get_font_parameters(const Font &f, font_parameters_t *fp);

                    virtual bool get_text_parameters(const Font &f, text_parameters_t *tp, const char *text);

                    virtual void out_text(const Font &f, float x, float y, const char *text, const Color &color);

                    virtual void out_text_relative(const Font &f, float x, float y, float dx, float dy, const char *text, const Color &color);

                    virtual void square_dot(float x, float y, float width, const Color &color);

                    virtual void square_dot(float x, float y, float width, float r, float g, float b, float a);

                    virtual void line(float x0, float y0, float x1, float y1, float width, const Color &color);

                    virtual void line(float x0, float y0, float x1, float y1, float width, IGradient *g);

                    virtual void parametric_line(float a, float b, float c, float width, const Color &color);

                    virtual void parametric_line(float a, float b, float c, float left, float right, float top, float bottom, float width, const Color &color);

                    virtual void parametric_bar(float a1, float b1, float c1, float a2, float b2, float c2,
                            float left, float right, float top, float bottom, IGradient *gr);

                    virtual void wire_arc(float x, float y, float r, float a1, float a2, float width, const Color &color);

                    virtual void fill_poly(const float *x, const float *y, size_t n, const Color & color);

                    virtual void fill_poly(const float *x, const float *y, size_t n, IGradient *gr);

                    virtual void wire_poly(const float *x, const float *y, size_t n, float width, const Color & color);

                    virtual void draw_poly(const float *x, const float *y, size_t n, float width, const Color &fill, const Color &wire);

                    virtual void fill_circle(float x, float y, float r, const Color & color);

                    virtual void fill_circle(float x, float y, float r, IGradient *g);

                    virtual void clip_begin(float x, float y, float w, float h);

                    void clip_end();

                    virtual void fill_frame(
                        float fx, float fy, float fw, float fh,
                        float ix, float iy, float iw, float ih,
                        const Color &color
                    );

                    virtual void fill_round_frame(
                            float fx, float fy, float fw, float fh,
                            float ix, float iy, float iw, float ih,
                            float radius, size_t flags,
                            const Color &color);

                    virtual bool get_antialiasing();

                    virtual bool set_antialiasing(bool set);

                    virtual surf_line_cap_t get_line_cap();

                    virtual surf_line_cap_t set_line_cap(surf_line_cap_t lc);

                    virtual void *start_direct();

                    virtual void end_direct();
            };
        }
    }

} /* namespace lsp */

#endif /* UI_X11_X11CAIROSURFACE_H_ */
