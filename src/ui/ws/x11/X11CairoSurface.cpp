/*
 * X11CairoSurface.cpp
 *
 *  Created on: 25 окт. 2016 г.
 *      Author: sadko
 */

#include <ui/ws/x11/ws.h>

#ifdef USE_X11_DISPLAY
namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            X11CairoSurface::X11CairoSurface(Display *dpy, Drawable drawable, Visual *visual, size_t width, size_t height):
                ISurface(width, height, ST_XLIB)
            {
                pSurface        = cairo_xlib_surface_create(dpy, drawable, visual, width, height);
                if (pSurface == NULL)
                    return;
                pCR             = cairo_create(pSurface);
                if (pCR == NULL)
                    return;
                bBegin          = false;

                cairo_set_antialias(pCR, CAIRO_ANTIALIAS_NONE);
                cairo_set_line_join(pCR, CAIRO_LINE_JOIN_BEVEL);
            }

            X11CairoSurface::X11CairoSurface(size_t width, size_t height):
                ISurface(width, height, ST_IMAGE)
            {
                pSurface        = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
                if (pSurface == NULL)
                    return;
                pCR             = cairo_create(pSurface);
                if (pCR == NULL)
                    return;
                bBegin          = false;

                cairo_set_antialias(pCR, CAIRO_ANTIALIAS_NONE);
                cairo_set_line_join(pCR, CAIRO_LINE_JOIN_BEVEL);
                nStride         = cairo_image_surface_get_stride(pSurface);
            }

            ISurface *X11CairoSurface::create(size_t width, size_t height)
            {
                X11CairoSurface *s = new X11CairoSurface(width, height);
                if (s == NULL)
                    return NULL;
                if (s->pCR != NULL)
                    return s;

                pData = NULL;

                delete s;
                return NULL;
            }

            ISurface *X11CairoSurface::create_copy()
            {
                X11CairoSurface *s = new X11CairoSurface(nWidth, nHeight);
                if (s == NULL)
                    return NULL;
                if (s->pCR == NULL)
                {
                    delete s;
                    return NULL;
                }

                // Draw one surface on another
                cairo_set_source_surface(s->pCR, pSurface, 0.0f, 0.0f);
                cairo_paint(s->pCR);

                return s;
            }

            IGradient *X11CairoSurface::linear_gradient(float x0, float y0, float x1, float y1)
            {
                return new X11CairoLinearGradient(x0, y0, x1, y1);
            }

            IGradient *X11CairoSurface::radial_gradient(float cx0, float cy0, float r0, float cx1, float cy1, float r1)
            {
                return new X11CairoRadialGradient(cx0, cy0, r0, cx1, cy1, r1);
            }

            X11CairoSurface::~X11CairoSurface()
            {
                destroy_context();
            }

            void X11CairoSurface::destroy_context()
            {
                if (pCR != NULL)
                {
                    cairo_destroy(pCR);
                    pCR             = NULL;
                }
                if (pSurface != NULL)
                {
                    cairo_surface_destroy(pSurface);
                    pSurface        = NULL;
                }
            }

            void X11CairoSurface::destroy()
            {
                destroy_context();
            }

            bool X11CairoSurface::resize(size_t width, size_t height)
            {
                if (nType == ST_XLIB)
                {
                    cairo_xlib_surface_set_size(pSurface, width, height);
                    return true;
                }
                else if (nType == ST_IMAGE)
                {
                    // Create new surface and cairo
                    cairo_surface_t *s  = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, width, height);
                    if (s == NULL)
                        return false;
                    cairo_t *cr         = cairo_create(s);
                    if (cr == NULL)
                    {
                        cairo_surface_destroy(s);
                        return false;
                    }

                    // Draw previous content
                    cairo_set_source_surface(cr, pSurface, 0, 0);
                    cairo_fill(cr);

                    // Destroy previously used context
                    destroy_context();

                    // Update context
                    pSurface            = s;
                    pCR                 = cr;
                }

                return false;
            }

            void X11CairoSurface::draw(ISurface *s, float x, float y)
            {
                surface_type_t type = s->type();
                if ((type != ST_XLIB) && (type != ST_IMAGE))
                    return;
                if (pCR == NULL)
                    return;
                X11CairoSurface *cs = static_cast<X11CairoSurface *>(s);
                if (cs->pSurface == NULL)
                    return;

                // Draw one surface on another
                cairo_set_source_surface(pCR, cs->pSurface, x, y);
                cairo_paint(pCR);
            }

            void X11CairoSurface::draw(ISurface *s, float x, float y, float sx, float sy)
            {
                surface_type_t type = s->type();
                if ((type != ST_XLIB) && (type != ST_IMAGE))
                    return;
                if (pCR == NULL)
                    return;
                X11CairoSurface *cs = static_cast<X11CairoSurface *>(s);
                if (cs->pSurface == NULL)
                    return;

                // Draw one surface on another
                cairo_save(pCR);
                if (sx < 0.0f)
                    x       -= sx * s->width();
                if (sy < 0.0f)
                    y       -= sy * s->height();
                cairo_translate(pCR, x, y);
                cairo_scale(pCR, sx, sy);
                cairo_set_source_surface(pCR, cs->pSurface, 0.0f, 0.0f);
                cairo_paint(pCR);
                cairo_restore(pCR);
            }

            void X11CairoSurface::draw_alpha(ISurface *s, float x, float y, float sx, float sy, float a)
            {
                surface_type_t type = s->type();
                if ((type != ST_XLIB) && (type != ST_IMAGE))
                    return;
                if (pCR == NULL)
                    return;
                X11CairoSurface *cs = static_cast<X11CairoSurface *>(s);
                if (cs->pSurface == NULL)
                    return;

                // Draw one surface on another
                cairo_save(pCR);
                if (sx < 0.0f)
                    x       -= sx * s->width();
                if (sy < 0.0f)
                    y       -= sy * s->height();
                cairo_translate(pCR, x, y);
                cairo_scale(pCR, sx, sy);
                cairo_set_source_surface(pCR, cs->pSurface, 0.0f, 0.0f);
                cairo_paint_with_alpha(pCR, 1.0f - a);
                cairo_restore(pCR);
            }

            void X11CairoSurface::draw_rotate_alpha(ISurface *s, float x, float y, float sx, float sy, float ra, float a)
            {
                surface_type_t type = s->type();
                if ((type != ST_XLIB) && (type != ST_IMAGE))
                    return;
                if (pCR == NULL)
                    return;
                X11CairoSurface *cs = static_cast<X11CairoSurface *>(s);
                if (cs->pSurface == NULL)
                    return;

                // Draw one surface on another
                cairo_save(pCR);
                cairo_translate(pCR, x, y);
                cairo_scale(pCR, sx, sy);
                cairo_rotate(pCR, ra);
                cairo_set_source_surface(pCR, cs->pSurface, 0.0f, 0.0f);
                cairo_paint_with_alpha(pCR, 1.0f - a);
                cairo_restore(pCR);
            }

            void X11CairoSurface::draw_clipped(ISurface *s, float x, float y, float sx, float sy, float sw, float sh)
            {
                surface_type_t type = s->type();
                if ((type != ST_XLIB) && (type != ST_IMAGE))
                    return;
                if (pCR == NULL)
                    return;
                X11CairoSurface *cs = static_cast<X11CairoSurface *>(s);
                if (cs->pSurface == NULL)
                    return;

                // Draw one surface on another
                cairo_save(pCR);
                cairo_set_source_surface(pCR, cs->pSurface, x - sx, y - sy);
                cairo_rectangle(pCR, x, y, sw, sh);
                cairo_fill(pCR);
                cairo_restore(pCR);
            }

            void X11CairoSurface::begin()
            {
                if (nType != ST_XLIB)
                    return;
                if (bBegin)
                    end();
                bBegin = true;

                // TODO
            }

            void X11CairoSurface::end()
            {
                if ((nType != ST_XLIB) || (!bBegin) || (pCR == NULL))
                    return;

                // TODO

                bBegin = false;
                cairo_surface_flush(pSurface);
            }

            void X11CairoSurface::clear_rgb(uint32_t rgb)
            {
                clear_rgba(rgb & 0xffffff);
            }

            void X11CairoSurface::clear_rgba(uint32_t rgba)
            {
                if (pCR == NULL)
                    return;

                cairo_operator_t op = cairo_get_operator(pCR);
                cairo_set_operator (pCR, CAIRO_OPERATOR_SOURCE);
                cairo_set_source_rgb(pCR,
                    float((rgba >> 16) & 0xff)/255.0f,
                    float((rgba >> 8) & 0xff)/255.0f,
                    float(rgba & 0xff)/255.0f
                );
                cairo_paint(pCR);
                cairo_set_operator (pCR, op);
            }

            inline void X11CairoSurface::setSourceRGB(const Color &col)
            {
                cairo_set_source_rgb(pCR, col.red(), col.green(), col.blue());
            }

            inline void X11CairoSurface::setSourceRGBA(const Color &col)
            {
                cairo_set_source_rgba(pCR, col.red(), col.green(), col.blue(), 1.0f - col.alpha());
            }

            void X11CairoSurface::clear(const Color &color)
            {
                if (pCR == NULL)
                    return;
                setSourceRGBA(color);
                cairo_operator_t op = cairo_get_operator(pCR);
                cairo_set_operator (pCR, CAIRO_OPERATOR_SOURCE);
                cairo_paint(pCR);
                cairo_set_operator (pCR, op);
            }

            void X11CairoSurface::fill_rect(float left, float top, float width, float height, const Color &color)
            {
                if (pCR == NULL)
                    return;
                setSourceRGBA(color);
                cairo_rectangle(pCR, left, top, width, height);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_rect(float left, float top, float width, float height, IGradient *g)
            {
                if (pCR == NULL)
                    return;

                X11CairoGradient *cg = static_cast<X11CairoGradient *>(g);
                cg->apply(pCR);
                cairo_rectangle(pCR, left, top, width, height);
                cairo_fill(pCR);
            }

            void X11CairoSurface::wire_rect(float left, float top, float width, float height, float line_width, const Color &color)
            {
                if (pCR == NULL)
                    return;
                setSourceRGBA(color);
                double w = cairo_get_line_width(pCR);
                cairo_set_line_width(pCR, line_width);
                cairo_rectangle(pCR, left, top, width, height);
                cairo_stroke(pCR);
                cairo_set_line_width(pCR, w);
            }

            void X11CairoSurface::drawRoundRect(float left, float top, float width, float height, float radius, size_t mask)
            {
                if (mask & SURFMASK_LT_CORNER)
                {
                    cairo_move_to(pCR, left, top + radius);
                    cairo_arc(pCR, left + radius, top + radius, radius, M_PI, 1.5f*M_PI);
                }
                else
                    cairo_move_to(pCR, left, top);

                if (mask & SURFMASK_RT_CORNER)
                    cairo_arc(pCR, left + width - radius, top + radius, radius, 1.5f * M_PI, 2.0f * M_PI);
                else
                    cairo_line_to(pCR, left + width, top);

                if (mask & SURFMASK_RB_CORNER)
                    cairo_arc(pCR, left + width - radius, top + height - radius, radius, 0.0f, 0.5f * M_PI);
                else
                    cairo_line_to(pCR, left + width, top + height);

                if (mask & SURFMASK_LB_CORNER)
                    cairo_arc(pCR, left + radius, top + height - radius, radius, 0.5f * M_PI, M_PI);
                else
                    cairo_line_to(pCR, left, top + height);

                cairo_close_path(pCR);
            }

            void X11CairoSurface::wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, const Color &color)
            {
                if (pCR == NULL)
                    return;
                setSourceRGBA(color);
                double w = cairo_get_line_width(pCR);
                cairo_set_line_width(pCR, line_width);
                drawRoundRect(left, top, width, height, radius, mask);
                cairo_stroke(pCR);
                cairo_set_line_width(pCR, w);
            }

            void X11CairoSurface::wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, IGradient *g)
            {
                if (pCR == NULL)
                    return;
                X11CairoGradient *cg = static_cast<X11CairoGradient *>(g);

                double w = cairo_get_line_width(pCR);
                cairo_set_line_width(pCR, line_width);
                cg->apply(pCR);
                drawRoundRect(left, top, width, height, radius, mask);
                cairo_stroke(pCR);
                cairo_set_line_width(pCR, w);
            }

            void X11CairoSurface::fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, const Color &color)
            {
                if (pCR == NULL)
                    return;
                setSourceRGBA(color);
                drawRoundRect(left, top, width, height, radius, mask);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, IGradient *g)
            {
                if (pCR == NULL)
                    return;
                X11CairoGradient *cg = static_cast<X11CairoGradient *>(g);
                cg->apply(pCR);
                drawRoundRect(left, top, width, height, radius, mask);
                cairo_fill(pCR);
            }

            void X11CairoSurface::full_rect(float left, float top, float width, float height, float line_width, const Color &color)
            {
                if (pCR == NULL)
                    return;
                setSourceRGBA(color);
                cairo_set_line_width(pCR, line_width);
                cairo_rectangle(pCR, left, top, width, height);
                cairo_stroke_preserve(pCR);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_round_rect(float left, float top, float width, float height, float r, const Color &color)
            {
                if (pCR == NULL)
                    return;

                float x0 = left, y0 = top, x1 = left + width, y1 = top + height;

                setSourceRGBA(color);
                cairo_move_to(pCR, x1 - r, y0);
                cairo_curve_to(pCR, x1, y0, x1, y0, x1, y0 + r);
                cairo_line_to(pCR, x1, y1 - r);
                cairo_curve_to(pCR, x1, y1, x1, y1, x1 - r, y1);
                cairo_line_to(pCR, x0 + r, y1);
                cairo_curve_to(pCR, x0, y1, x0, y1, x0, y1 - r);
                cairo_line_to(pCR, x0, y0 + r);
                cairo_curve_to(pCR, x0, y0, x0, y0, x0 + r, y0);
                cairo_close_path(pCR);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_round_rect(float left, float top, float width, float height, float r, IGradient *g)
            {
                if (pCR == NULL)
                    return;
                X11CairoGradient *cg = static_cast<X11CairoGradient *>(g);

                float x0 = left, y0 = top, x1 = left + width, y1 = top + height;

                cg->apply(pCR);
                cairo_move_to(pCR, x1 - r, y0);
                cairo_curve_to(pCR, x1, y0, x1, y0, x1, y0 + r);
                cairo_line_to(pCR, x1, y1 - r);
                cairo_curve_to(pCR, x1, y1, x1, y1, x1 - r, y1);
                cairo_line_to(pCR, x0 + r, y1);
                cairo_curve_to(pCR, x0, y1, x0, y1, x0, y1 - r);
                cairo_line_to(pCR, x0, y0 + r);
                cairo_curve_to(pCR, x0, y0, x0, y0, x0 + r, y0);
                cairo_close_path(pCR);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_sector(float cx, float cy, float radius, float angle1, float angle2, const Color &color)
            {
                if (pCR == NULL)
                    return;

                setSourceRGBA(color);
                cairo_move_to(pCR, cx, cy);
                cairo_arc(pCR, cx, cy, radius, angle1, angle2);
                cairo_close_path(pCR);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, IGradient *g)
            {
                if (pCR == NULL)
                    return;

                X11CairoGradient *cg = static_cast<X11CairoGradient *>(g);
                cg->apply(pCR);
                cairo_move_to(pCR, x0, y0);
                cairo_line_to(pCR, x1, y1);
                cairo_line_to(pCR, x2, y2);
                cairo_close_path(pCR);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, const Color &color)
            {
                if (pCR == NULL)
                    return;

                setSourceRGBA(color);
                cairo_move_to(pCR, x0, y0);
                cairo_line_to(pCR, x1, y1);
                cairo_line_to(pCR, x2, y2);
                cairo_close_path(pCR);
                cairo_fill(pCR);
            }

            bool X11CairoSurface::get_font_parameters(const Font &f, font_parameters_t *fp)
            {
                if ((pCR == NULL) || (f.get_name() == NULL))
                    return false;

                cairo_select_font_face(pCR, f.get_name(),
                    (f.is_italic()) ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
                    (f.is_bold()) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL
                );
                cairo_set_font_size(pCR, f.get_size());

                cairo_font_extents_t fe;
                cairo_font_extents(pCR, &fe);

                fp->Ascent          = fe.ascent;
                fp->Descent         = fe.descent;
                fp->Height          = fe.height;
                fp->MaxXAdvance     = fe.max_x_advance;
                fp->MaxYAdvance     = fe.max_y_advance;

                return true;
            }

            bool X11CairoSurface::get_text_parameters(const Font &f, text_parameters_t *tp, const char *text)
            {
                if ((pCR == NULL) || (f.get_name() == NULL))
                    return false;

                cairo_select_font_face(pCR, f.get_name(),
                    (f.is_italic()) ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
                    (f.is_bold()) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL
                );
                cairo_set_font_size(pCR, f.get_size());

                cairo_text_extents_t te;
                cairo_text_extents(pCR, text, &te);

                tp->XBearing        = te.x_bearing;
                tp->YBearing        = te.y_bearing;
                tp->Width           = te.width;
                tp->Height          = te.height;
                tp->XAdvance        = te.x_advance;
                tp->YAdvance        = te.y_advance;

                return true;
            }

            void X11CairoSurface::out_text(const Font &f, float x, float y, const char *text, const Color &color)
            {
                if ((pCR == NULL) || (f.get_name() == NULL) || (text == NULL))
                    return;

                cairo_select_font_face(pCR, f.get_name(),
                    (f.is_italic()) ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
                    (f.is_bold()) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL
                );
                cairo_set_font_size(pCR, f.get_size());

                cairo_move_to(pCR, x, y);
                setSourceRGBA(color);
                cairo_show_text(pCR, text);

                if (f.is_underline())
                {
                    cairo_text_extents_t te;
                    cairo_text_extents(pCR, text, &te);
                    cairo_set_line_width(pCR, 1.0f);

                    cairo_move_to(pCR, x, y + te.y_advance + 2);
                    cairo_line_to(pCR, x + te.x_advance, y + te.y_advance + 2);
                    cairo_stroke(pCR);
                }
            }

            void X11CairoSurface::out_text_relative(const Font &f, float x, float y, float dx, float dy, const char *text, const Color &color)
            {
                if ((pCR == NULL) || (f.get_name() == NULL) || (text == NULL))
                    return;

                // Draw text border
                cairo_text_extents_t extents;

                cairo_select_font_face(pCR, f.get_name(),
                    (f.is_italic()) ? CAIRO_FONT_SLANT_ITALIC : CAIRO_FONT_SLANT_NORMAL,
                    (f.is_bold()) ? CAIRO_FONT_WEIGHT_BOLD : CAIRO_FONT_WEIGHT_NORMAL
                );
                cairo_set_font_size(pCR, f.get_size());
                cairo_text_extents(pCR, text, &extents);

                float r_w   = extents.x_advance - extents.x_bearing;
                float r_h   = extents.y_advance - extents.y_bearing;
                float fx    = x - extents.x_bearing + (r_w + 4) * 0.5f * dx - r_w * 0.5f;
                float fy    = y - extents.y_advance + (r_h + 4) * 0.5f * (1.0f - dy) - r_h * 0.5f + 1.0f;

                cairo_move_to(pCR, fx, fy);
                cairo_show_text(pCR, text);
            }

            void X11CairoSurface::square_dot(float x, float y, float width, const Color &color)
            {
                if (pCR == NULL)
                    return;

                double ow = cairo_get_line_width(pCR);
                cairo_line_cap_t cap = cairo_get_line_cap(pCR);
                setSourceRGBA(color);
                cairo_set_line_width(pCR, width);
                cairo_set_line_cap(pCR, CAIRO_LINE_CAP_SQUARE);
                cairo_move_to(pCR, x + 0.5f, y + 0.5f);
                cairo_line_to(pCR, x + 1.5f, y + 0.5f);
                cairo_stroke(pCR);
                cairo_set_line_width(pCR, ow);
                cairo_set_line_cap(pCR, cap);
            }

            void X11CairoSurface::square_dot(float x, float y, float width, float r, float g, float b, float a)
            {
                if (pCR == NULL)
                    return;

                double ow = cairo_get_line_width(pCR);
                cairo_line_cap_t cap = cairo_get_line_cap(pCR);
                cairo_set_source_rgba(pCR, r, g, b, 1.0f - a);
                cairo_set_line_width(pCR, width);
                cairo_set_line_cap(pCR, CAIRO_LINE_CAP_SQUARE);
                cairo_move_to(pCR, x + 0.5f, y + 0.5f);
                cairo_line_to(pCR, x + 1.5f, y + 0.5f);
                cairo_stroke(pCR);
                cairo_set_line_width(pCR, ow);
                cairo_set_line_cap(pCR, cap);
            }

            void X11CairoSurface::line(float x0, float y0, float x1, float y1, float width, const Color &color)
            {
                if (pCR == NULL)
                    return;

                double ow = cairo_get_line_width(pCR);
                setSourceRGBA(color);
                cairo_set_line_width(pCR, width);
                cairo_move_to(pCR, x0, y0);
                cairo_line_to(pCR, x1, y1);
                cairo_stroke(pCR);
                cairo_set_line_width(pCR, ow);
            }

            void X11CairoSurface::line(float x0, float y0, float x1, float y1, float width, IGradient *g)
            {
                if (pCR == NULL)
                    return;

                X11CairoGradient *cg = static_cast<X11CairoGradient *>(g);
                cg->apply(pCR);

                double ow = cairo_get_line_width(pCR);
                cairo_set_line_width(pCR, width);
                cairo_move_to(pCR, x0, y0);
                cairo_line_to(pCR, x1, y1);
                cairo_stroke(pCR);
                cairo_set_line_width(pCR, ow);
            }

            void X11CairoSurface::parametric_line(float a, float b, float c, float width, const Color &color)
            {
                if (pCR == NULL)
                    return;

                double ow = cairo_get_line_width(pCR);
                setSourceRGBA(color);
                cairo_set_line_width(pCR, width);

                if (fabs(a) > fabs(b))
                {
                    cairo_move_to(pCR, - c / a, 0.0f);
                    cairo_line_to(pCR, -(c + b*nHeight)/a, nHeight);
                }
                else
                {
                    cairo_move_to(pCR, 0.0f, - c / b);
                    cairo_line_to(pCR, nWidth, -(c + a*nWidth)/b);
                }

                cairo_stroke(pCR);
                cairo_set_line_width(pCR, ow);
            }

            void X11CairoSurface::parametric_line(float a, float b, float c, float left, float right, float top, float bottom, float width, const Color &color)
            {
                if (pCR == NULL)
                    return;

                double ow = cairo_get_line_width(pCR);
                setSourceRGBA(color);
                cairo_set_line_width(pCR, width);

                if (fabs(a) > fabs(b))
                {
                    cairo_move_to(pCR, ssize_t(-(c + b*top)/a), ssize_t(top));
                    cairo_line_to(pCR, ssize_t(-(c + b*bottom)/a), ssize_t(bottom));
                }
                else
                {
                    cairo_move_to(pCR, ssize_t(left), ssize_t(-(c + a*left)/b));
                    cairo_line_to(pCR, ssize_t(right), ssize_t(-(c + a*right)/b));
                }

                cairo_stroke(pCR);
                cairo_set_line_width(pCR, ow);
            }

            void X11CairoSurface::parametric_bar(float a1, float b1, float c1, float a2, float b2, float c2,
                    float left, float right, float top, float bottom, IGradient *gr)
            {
                if (pCR == NULL)
                    return;

                X11CairoGradient *cg = static_cast<X11CairoGradient *>(gr);
                cg->apply(pCR);

                if (fabs(a1) > fabs(b1))
                {
                    cairo_move_to(pCR, ssize_t(-(c1 + b1*top)/a1), ssize_t(top));
                    cairo_line_to(pCR, ssize_t(-(c1 + b1*bottom)/a1), ssize_t(bottom));
                }
                else
                {
                    cairo_move_to(pCR, ssize_t(left), ssize_t(-(c1 + a1*left)/b1));
                    cairo_line_to(pCR, ssize_t(right), ssize_t(-(c1 + a1*right)/b1));
                }

                if (fabs(a2) > fabs(b2))
                {
                    cairo_line_to(pCR, ssize_t(-(c2 + b2*bottom)/a2), ssize_t(bottom));
                    cairo_line_to(pCR, ssize_t(-(c2 + b2*top)/a2), ssize_t(top));
                }
                else
                {
                    cairo_line_to(pCR, ssize_t(right), ssize_t(-(c2 + a2*right)/b2));
                    cairo_line_to(pCR, ssize_t(left), ssize_t(-(c2 + a2*left)/b2));
                }

                cairo_close_path(pCR);
                cairo_fill(pCR);
            }

            void X11CairoSurface::wire_arc(float x, float y, float r, float a1, float a2, float width, const Color &color)
            {
                if (pCR == NULL)
                    return;

                double ow = cairo_get_line_width(pCR);
                setSourceRGBA(color);
                cairo_set_line_width(pCR, width);
                cairo_arc(pCR, x, y, r, a1, a2);
                cairo_stroke(pCR);
                cairo_set_line_width(pCR, ow);
            }

            void X11CairoSurface::fill_poly(const float *x, const float *y, size_t n, const Color & color)
            {
                if ((n < 2) || (pCR == NULL))
                    return;

                cairo_move_to(pCR, *(x++), *(y++));
                for (size_t i=1; i < n; ++i)
                    cairo_line_to(pCR, *(x++), *(y++));

                setSourceRGBA(color);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_poly(const float *x, const float *y, size_t n, IGradient *gr)
            {
                if ((n < 2) || (pCR == NULL) || (gr == NULL))
                    return;

                cairo_move_to(pCR, *(x++), *(y++));
                for (size_t i=1; i < n; ++i)
                    cairo_line_to(pCR, *(x++), *(y++));

                X11CairoGradient *cg = static_cast<X11CairoGradient *>(gr);
                cg->apply(pCR);
                cairo_fill(pCR);
            }

            void X11CairoSurface::wire_poly(const float *x, const float *y, size_t n, float width, const Color & color)
            {
                if ((n < 2) || (pCR == NULL))
                    return;

                cairo_move_to(pCR, *(x++), *(y++));
                for (size_t i=1; i < n; ++i)
                    cairo_line_to(pCR, *(x++), *(y++));

                setSourceRGBA(color);
                cairo_set_line_width(pCR, width);
                cairo_stroke(pCR);
            }

            void X11CairoSurface::draw_poly(const float *x, const float *y, size_t n, float width, const Color &fill, const Color &wire)
            {
                if ((n < 2) || (pCR == NULL))
                    return;

                cairo_move_to(pCR, *(x++), *(y++));
                for (size_t i=1; i < n; ++i)
                    cairo_line_to(pCR, *(x++), *(y++));

                setSourceRGBA(fill);
                cairo_fill_preserve(pCR);

                cairo_set_line_width(pCR, width);
                setSourceRGBA(wire);
                cairo_stroke(pCR);
            }

            void X11CairoSurface::fill_circle(float x, float y, float r, const Color & color)
            {
                if (pCR == NULL)
                    return;

                setSourceRGBA(color);
                cairo_arc(pCR, x, y, r, 0.0f, M_PI * 2.0f);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_circle(float x, float y, float r, IGradient *g)
            {
                if (pCR == NULL)
                    return;
                X11CairoGradient *cg = static_cast<X11CairoGradient *>(g);
                cg->apply(pCR);
                cairo_arc(pCR, x, y, r, 0, M_PI * 2.0f);
                cairo_fill(pCR);
            }

            void X11CairoSurface::fill_frame(
                float fx, float fy, float fw, float fh,
                float ix, float iy, float iw, float ih,
                const Color &color
            )
            {
                if (pCR == NULL)
                    return;

                float fxe = fx + fw, fye = fy + fh, ixe = ix + iw, iye = iy + ih;

                if ((ix >= fxe) || (ixe < fx) || (iy >= fye) || (iye < fy))
                {
                    setSourceRGBA(color);
                    cairo_rectangle(pCR, fx, fy, fw, fh);
                    cairo_fill(pCR);
                    return;
                }
                else if ((ix <= fx) && (ixe >= fxe) && (iy <= fy) && (iye >= fye))
                    return;

                #define MOVE_TO(p, x, y) \
                    /*lsp_trace("move_to: %d, %d", int(x), int(y));*/ \
                    cairo_move_to(p, (x), (y));
                #define LINE_TO(p, x, y) \
                    /*lsp_trace("line_to: %d, %d", int(x), int(y)); */\
                    cairo_move_to(p, (x), (y));

                #define RECTANGLE(p, x, y, w, h) \
                    /*lsp_trace("rectangle: %d, %d, %d, %d", int(x), int(y), int(w), int(h)); */ \
                    cairo_rectangle(p, (x), (y), (w), (h));

                setSourceRGBA(color);
                if (ix <= fx)
                {
                    if (iy <= fy)
                    { // OK
                        RECTANGLE(pCR, ixe, fy, fxe - ixe, iye - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iye, fw, fye - iye);
                        cairo_fill(pCR);
                    }
                    else if (iye >= fye)
                    { // OK
                        RECTANGLE(pCR, fx, fy, fw, iy - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, ixe, iy, fxe - ixe, fye - iy);
                        cairo_fill(pCR);
                    }
                    else
                    { // OK
                        RECTANGLE(pCR, fx, fy, fw, iy - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, ixe, iy, fxe - ixe, ih);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iye, fw, fye - iye);
                        cairo_fill(pCR);
                    }
                }
                else if (ixe >= fxe)
                {
                    if (iy <= fy)
                    { // OK ?
                        RECTANGLE(pCR, fx, fy, ix - fx, iye - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iye, fw, fye - iye);
                        cairo_fill(pCR);
                    }
                    else if (iye >= fye)
                    { // OK
                        RECTANGLE(pCR, fx, fy, fw, iy - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iy, ix - fx, fye - iy);
                        cairo_fill(pCR);
                    }
                    else
                    { // OK
                        RECTANGLE(pCR, fx, fy, fw, iy - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iy, ix - fx, ih);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iye, fw, fye - iye);
                        cairo_fill(pCR);
                    }
                }
                else
                {
                    if (iy <= fy)
                    { // OK
                        RECTANGLE(pCR, fx, fy, ix - fx, iye - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, ixe, fy, fxe - ixe, iye - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iye, fw, fye - iye);
                        cairo_fill(pCR);
                    }
                    else if (iye >= fye)
                    { // OK
                        RECTANGLE(pCR, fx, fy, fw, iy - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iy, ix - fx, fye - iy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, ixe, iy, fxe - ixe, fye - iy);
                        cairo_fill(pCR);
                    }
                    else
                    { // OK
                        RECTANGLE(pCR, fx, fy, fw, iy - fy);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iy, ix - fx, ih);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, ixe, iy, fxe - ixe, ih);
                        cairo_fill(pCR);
                        RECTANGLE(pCR, fx, iye, fw, fye - iye);
                        cairo_fill(pCR);
                    }
                }
//                cairo_close_path(pCR);
//                cairo_fill(pCR);
            }

            bool X11CairoSurface::get_antialiasing()
            {
                if (pCR == NULL)
                    return false;
                return cairo_get_antialias(pCR) != CAIRO_ANTIALIAS_NONE;
            }

            bool X11CairoSurface::set_antialiasing(bool set)
            {
                if (pCR == NULL)
                    return false;

                bool old = cairo_get_antialias(pCR) != CAIRO_ANTIALIAS_NONE;
                cairo_set_antialias(pCR, (set) ? CAIRO_ANTIALIAS_DEFAULT : CAIRO_ANTIALIAS_NONE);

                return old;
            }

            surf_line_cap_t X11CairoSurface::get_line_cap()
            {
                if (pCR == NULL)
                    return SURFLCAP_BUTT;

                cairo_line_cap_t old = cairo_get_line_cap(pCR);

                return
                    (old == CAIRO_LINE_CAP_BUTT) ? SURFLCAP_BUTT :
                    (old == CAIRO_LINE_CAP_ROUND) ? SURFLCAP_ROUND : SURFLCAP_SQUARE;
            }

            surf_line_cap_t X11CairoSurface::set_line_cap(surf_line_cap_t lc)
            {
                if (pCR == NULL)
                    return SURFLCAP_BUTT;

                cairo_line_cap_t old = cairo_get_line_cap(pCR);

                cairo_line_cap_t cap =
                    (lc == SURFLCAP_BUTT) ? CAIRO_LINE_CAP_BUTT :
                    (lc == SURFLCAP_ROUND) ? CAIRO_LINE_CAP_ROUND :
                    CAIRO_LINE_CAP_SQUARE;

                cairo_set_line_cap(pCR, cap);

                return
                    (old == CAIRO_LINE_CAP_BUTT) ? SURFLCAP_BUTT :
                    (old == CAIRO_LINE_CAP_ROUND) ? SURFLCAP_ROUND : SURFLCAP_SQUARE;
            }

            void *X11CairoSurface::start_direct()
            {
                if ((pCR == NULL) || (pSurface == NULL) || (nType != ST_IMAGE))
                    return NULL;

                nStride = cairo_image_surface_get_stride(pSurface);
                return pData = reinterpret_cast<uint8_t *>(cairo_image_surface_get_data(pSurface));
            }

            void X11CairoSurface::end_direct()
            {
                if ((pCR == NULL) || (pSurface == NULL) || (nType != ST_IMAGE) || (pData == NULL))
                    return;

                cairo_surface_mark_dirty(pSurface);
                pData = NULL;
            }

        }
    }

} /* namespace lsp */

#endif /* USE_X11_DISPLAY */
