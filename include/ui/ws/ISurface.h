/*
 * ISurface.h
 *
 *  Created on: 25 окт. 2016 г.
 *      Author: sadko
 */

#ifndef UI_WS_ISURFACE_H_
#define UI_WS_ISURFACE_H_

#include <core/types.h>
#include <core/util/Color.h>

#define SURFMASK_LT_CORNER      0x01
#define SURFMASK_RT_CORNER      0x02
#define SURFMASK_RB_CORNER      0x04
#define SURFMASK_LB_CORNER      0x08
#define SURFMASK_ALL_CORNER     0x0f
#define SURFMASK_T_CORNER       0x03
#define SURFMASK_B_CORNER       0x0c
#define SURFMASK_L_CORNER       0x09
#define SURFMASK_R_CORNER       0x06

enum surf_line_cap_t
{
    SURFLCAP_BUTT,
    SURFLCAP_ROUND,
    SURFLCAP_SQUARE
};

namespace lsp
{
    namespace ws
    {
        class ISurface;

        class IGradient
        {
            private:
                IGradient & operator = (const IGradient &);

            public:
                explicit IGradient();
                virtual ~IGradient();

            public:
                virtual void add_color(float offset, float r, float g, float b, float a=0.0f);

                inline void add_color(float offset, const Color &c)
                    { add_color(offset, c.red(), c.green(), c.blue(), c.alpha()); }

                inline void add_color(float offset, const Color &c, float a)
                    { add_color(offset, c.red(), c.green(), c.blue(), a); }

                void add_color_rgb(float offset, uint32_t color);

                void add_color_rgba(float offset, uint32_t color);
        };

        class Font
        {
            private:
                Font & operator = (const Font &);

            private:
                enum flags_t
                {
                    F_BOLD = 1 << 0,
                    F_ITALIC = 1 << 1,
                    F_UNDERLINE = 1 << 2
                };

                char       *sName;
                float       fSize;
                int         nFlags;

            public:
                explicit Font();
                explicit Font(const char *name);
                explicit Font(const char *name, float size);
                explicit Font(float size);
                explicit Font(const Font *s);

                ~Font();

            public:
                inline bool is_bold() const         { return nFlags & F_BOLD;   }
                inline bool is_italic() const       { return nFlags & F_ITALIC; }
                inline bool is_underline() const    { return nFlags & F_UNDERLINE; }
                inline float get_size() const       { return fSize; }
                inline const char *get_name() const { return sName; }

                inline void set_bold(bool b)        { if (b) nFlags |= F_BOLD; else nFlags &= ~F_BOLD; }
                inline void set_italic(bool i)      { if (i) nFlags |= F_ITALIC; else nFlags &= ~F_ITALIC; }
                inline void set_underline(bool u)   { if (u) nFlags |= F_UNDERLINE; else nFlags &= ~F_UNDERLINE; }
                inline void set_size(float s)       { fSize = s; }
                void set_name(const char *name);
                void set(const Font *s);

                bool get_parameters(ISurface *s, font_parameters_t *fp);
                bool get_text_parameters(ISurface *s, text_parameters_t *tp, const char *text);
        };

        /** Common drawing surface interface
         *
         */
        class ISurface
        {
            private:
                ISurface & operator = (const ISurface &);

            protected:
                size_t          nWidth;
                size_t          nHeight;
                size_t          nStride;
                uint8_t        *pData;
                surface_type_t  nType;

            protected:
                ISurface(size_t width, size_t height, surface_type_t type);

            public:
                explicit ISurface();
                virtual ~ISurface();

            public:
                /** Get surface width
                 *
                 * @return surface width
                 */
                inline size_t width() const { return nWidth; }

                /** Get surface height
                 *
                 * @return surface height
                 */
                inline size_t height() const { return nHeight; }

                /** Get type of surface
                 *
                 * @return type of surface
                 */
                inline surface_type_t type()  const { return nType; }

            public:
                /** Create child surface for drawing
                 * @param width surface width
                 * @param height surface height
                 * @return created surface or NULL
                 */
                virtual ISurface *create(size_t width, size_t height);

                /**
                 * Create copy of current surface
                 * @return copy of current surface
                 */
                virtual ISurface *create_copy();

                /** Create linear gradient
                 *
                 * @param x0
                 * @param y0
                 * @param x1
                 * @param y1
                 * @return
                 */
                virtual IGradient *linear_gradient(float x0, float y0, float x1, float y1);

                /** Create radial gradient
                 *
                 * @param cx0
                 * @param cy0
                 * @param r0
                 * @param cx1
                 * @param cy1
                 * @param r1
                 * @return
                 */
                virtual IGradient *radial_gradient
                (
                    float cx0, float cy0, float r0,
                    float cx1, float cy1, float r1
                );

                /** Destroy surface
                 *
                 */
                virtual void destroy();

                /** Start drawing on the surface
                 *
                 */
                virtual void begin();

                /** Complete drawing, synchronize surface with underlying device
                 *
                 */
                virtual void end();

            public:
                /** Draw surface
                 *
                 * @param s surface to draw
                 * @param x offset from left
                 * @param y offset from top
                 */
                virtual void draw(ISurface *s, float x, float y);

                /** Draw surface
                 *
                 * @param s surface to draw
                 * @param x offset from left
                 * @param y offset from top
                 * @param sx surface scale x
                 * @param sy surface scale y
                 */
                virtual void draw(ISurface *s, float x, float y, float sx, float sy);

                /** Draw surface with alpha blending
                 *
                 * @param s surface to draw
                 * @param x offset from left
                 * @param y offset from top
                 * @param sx surface scale x
                 * @param sy surface scale y
                 * @param a alpha
                 */
                virtual void draw_alpha(ISurface *s, float x, float y, float sx, float sy, float a);

                /** Draw surface with alpha blending and rotating
                 *
                 * @param s surface to draw
                 * @param x offset from left
                 * @param y offset from top
                 * @param sx surface scale x
                 * @param sy surface scale y
                 * @param ra rotation angle in radians
                 * @param a alpha
                 */
                virtual void draw_rotate_alpha(ISurface *s, float x, float y, float sx, float sy, float ra, float a);

                /** Draw clipped surface
                 *
                 * @param s surface to draw
                 * @param x position to draw at
                 * @param y position to draw at
                 * @param sx source surface starting position
                 * @param sy source surface starting position
                 * @param sw source surface width
                 * @param sh source surface height
                 */
                virtual void draw_clipped(ISurface *s, float x, float y, float sx, float sy, float sw, float sh);

                /** Draw filled rectangle
                 *
                 * @param left left-top corner x coorinate
                 * @param top left-top corner y coorinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param color color of rectangle
                 */
                virtual void fill_rect(float left, float top, float width, float height, const Color &color);

                /** Draw filled rectangle
                 *
                 * @param left left-top corner x coorinate
                 * @param top left-top corner y coorinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param g gradient to use
                 */
                virtual void fill_rect(float left, float top, float width, float height, IGradient *g);

                /** Draw wired rectangle
                 *
                 * @param left left-top corner x coorinate
                 * @param top left-top corner y coorinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param line_width width of line
                 * @param color color of rectangle
                 */
                virtual void wire_rect(float left, float top, float width, float height, float line_width, const Color &color);

                /** Wire rectangle with rounded corners
                 *
                 * @param left left-top corner x coordinate
                 * @param top left-top corner y coordinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param radius the corner radius
                 * @param mask the corner mask:
                 *      0x01 - left-top corner is rounded
                 *      0x02 - right-top corner is rounded
                 *      0x04 - right-bottom corner is rounded
                 *      0x08 - left-bottom corner is rounded
                 * @param line_width width of line
                 * @param color rectangle color
                 */
                virtual void wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, const Color &color);

                /** Wire rectangle with rounded corners
                 *
                 * @param left left-top corner x coordinate
                 * @param top left-top corner y coordinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param radius the corner radius
                 * @param mask the corner mask:
                 *      0x01 - left-top corner is rounded
                 *      0x02 - right-top corner is rounded
                 *      0x04 - right-bottom corner is rounded
                 *      0x08 - left-bottom corner is rounded
                 * @param line_width width of line
                 * @param color gradient to use
                 */
                virtual void wire_round_rect(float left, float top, float width, float height, float radius, size_t mask, float line_width, IGradient *g);

                /** Fill rectangle with rounded corners
                 *
                 * @param left left-top corner x coordinate
                 * @param top left-top corner y coordinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param radius the corner radius
                 * @param mask the corner mask:
                 *      0x01 - left-top corner is rounded
                 *      0x02 - right-top corner is rounded
                 *      0x04 - right-bottom corner is rounded
                 *      0x08 - left-bottom corner is rounded
                 * @param color rectangle color
                 */
                virtual void fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, const Color &color);

                /** Fill rectangle with rounded corners
                 *
                 * @param left left-top corner x coordinate
                 * @param top left-top corner y coordinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param radius the corner radius
                 * @param mask the corner mask:
                 *      0x01 - left-top corner is rounded
                 *      0x02 - right-top corner is rounded
                 *      0x04 - right-bottom corner is rounded
                 *      0x08 - left-bottom corner is rounded
                 * @param g gradient to use
                 */
                virtual void fill_round_rect(float left, float top, float width, float height, float radius, size_t mask, IGradient *g);

                /** Draw full rectangle
                 *
                 * @param left left-top corner x coorinate
                 * @param top left-top corner y coorinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param line_width width of line
                 * @param color color of rectangle
                 */
                virtual void full_rect(float left, float top, float width, float height, float line_width, const Color &color);

                /** Draw rectangle with rounded corners
                 *
                 * @param left left-top corner x coorinate
                 * @param top left-top corner y coorinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param radius rounding radius
                 * @param color color
                 */
                virtual void fill_round_rect(float left, float top, float width, float height, float radius, const Color &color);

                /** Draw rectangle with rounded corners
                 *
                 * @param left left-top corner x coorinate
                 * @param top left-top corner y coorinate
                 * @param width width of rectangle
                 * @param height height of rectangle
                 * @param radius rounding radius
                 * @param color color
                 */
                virtual void fill_round_rect(float left, float top, float width, float height, float radius, IGradient *g);

                /** Fill sector of the round
                 *
                 * @param cx center of the round x coordinate
                 * @param cy center of the round y coordinate
                 * @param radius the radius of the round
                 * @param angle1 starting angle of the sector
                 * @param angle2 end angle of the sector
                 * @param color color
                 */
                virtual void fill_sector(float cx, float cy, float radius, float angle1, float angle2, const Color &color);

                /** Fill rectangle
                 *
                 * @param x0 vertex 0 x-coordinate
                 * @param y0 vertex 0 y-coordinate
                 * @param x1 vertex 1 x-coordinate
                 * @param y1 vertex 1 y-coordinate
                 * @param x2 vertex 2 x-coordinate
                 * @param y2 vertex 2 y-coordinate
                 * @param g gradient
                 */
                virtual void fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, IGradient *g);

                /** Fill rectangle
                 *
                 * @param x0 vertex 0 x-coordinate
                 * @param y0 vertex 0 y-coordinate
                 * @param x1 vertex 1 x-coordinate
                 * @param y1 vertex 1 y-coordinate
                 * @param x2 vertex 2 x-coordinate
                 * @param y2 vertex 2 y-coordinate
                 * @param c color
                 */
                virtual void fill_triangle(float x0, float y0, float x1, float y1, float x2, float y2, const Color &color);

                /** Get font parameters
                 *
                 * @param f font
                 * @param fp font parameters to store
                 * @return status of operation
                 */
                virtual bool get_font_parameters(const Font &f, font_parameters_t *fp);

                /** Get text parameters
                 *
                 * @param f font
                 * @param tp text parameters to store
                 * @param text text to analyze
                 * @return status of operation
                 */
                virtual bool get_text_parameters(const Font &f, text_parameters_t *tp, const char *text);

                /** Clear surface with specified color
                 *
                 * @param color color to use for clearing
                 */
                virtual void clear(const Color &color);

                /** Clear surface with specified color
                 *
                 * @param color RGB color to use for clear
                 */
                virtual void clear_rgb(uint32_t color);

                /** Clear surface with specified color
                 *
                 * @param color RGBA color to use for clear
                 */
                virtual void clear_rgba(uint32_t color);

                /** Ouput single-line text
                 *
                 * @param f font to use
                 * @param x left position
                 * @param y top position
                 * @param text text to output
                 * @param color text color
                 */
                virtual void out_text(const Font &f, float x, float y, const char *text, const Color &color);

                /** Ouput single-line text relative to the specified position
                 *
                 * @param f font to use
                 * @param x position left
                 * @param y position top
                 * @param dx relative offset by the x position. -1 = leftmost, +1 = rightmost, 0 = middle
                 * @param dy relative offset by the y position. -1 = topmost, +1 = bottommost, 0 = middle
                 * @param text text to output
                 * @param color text color
                 */
                virtual void out_text_relative(const Font &f, float x, float y, float dx, float dy, const char *text, const Color &color);

                /**
                 * Draw square dot
                 * @param x dot location X
                 * @param y dot location Y
                 * @param width dot width
                 * @param color dot color
                 */
                virtual void square_dot(float x, float y, float width, const Color &color);

                /**
                 * Draw square dot
                 * @param x dot location X
                 * @param y dot location Y
                 * @param width dot width
                 * @param r red
                 * @param g green
                 * @param b blue
                 * @param a alpha
                 */
                virtual void square_dot(float x, float y, float width, float r, float g, float b, float a);

                /** Draw line
                 *
                 * @param x0 first point x coordinate
                 * @param y0 first point y coordinate
                 * @param x1 second point x coordinate
                 * @param y1 second point y coordinate
                 * @param width line width
                 * @param color line color
                 */
                virtual void line(float x0, float y0, float x1, float y1, float width, const Color &color);

                /** Draw line
                 *
                 * @param x0 first point x coordinate
                 * @param y0 first point y coordinate
                 * @param x1 second point x coordinate
                 * @param y1 second point y coordinate
                 * @param width line width
                 * @param g gradient
                 */
                virtual void line(float x0, float y0, float x1, float y1, float width, IGradient *g);

                /** Draw parametric line defined by equation a*x + b*y + c = 0
                 *
                 * @param a the x multiplier
                 * @param b the y multiplier
                 * @param c the shift
                 * @param width line width
                 * @param color line color
                 */
                virtual void parametric_line(float a, float b, float c, float width, const Color &color);

                /** Draw parameteric line defined by equation a*x + b*y + c = 0 and cull it by specified boundaries
                 *
                 * @param a the x multiplier
                 * @param b the y multiplier
                 * @param c the shift
                 * @param left
                 * @param right
                 * @param top
                 * @param bottom
                 * @param width line width
                 * @param color line color
                 */
                virtual void parametric_line(float a, float b, float c, float left, float right, float top, float bottom, float width, const Color &color);

                /** Draw parametric bar defined by two line equations
                 *
                 * @param a1 the x multiplier 1
                 * @param b1 the y multiplier 1
                 * @param c1 the shift 1
                 * @param a2 the x multiplier 2
                 * @param b2 the y multiplier 2
                 * @param c2 the shift 2
                 * @param left
                 * @param right
                 * @param top
                 * @param bottom
                 * @param gr gradient to fill bar
                 */
                virtual void parametric_bar(float a1, float b1, float c1, float a2, float b2, float c2,
                        float left, float right, float top, float bottom, IGradient *gr);

                /** Draw arc
                 *
                 * @param x center x
                 * @param y center y
                 * @param r radius
                 * @param a1 angle 1
                 * @param a2 angle 2
                 * @param width line width
                 * @param color line color
                 */
                virtual void wire_arc(float x, float y, float r, float a1, float a2, float width, const Color &color);

                virtual void fill_frame(
                        float fx, float fy, float fw, float fh,
                        float ix, float iy, float iw, float ih,
                        const Color &color);

                virtual void fill_round_frame(
                        float fx, float fy, float fw, float fh,
                        float ix, float iy, float iw, float ih,
                        float radius, size_t flags,
                        const Color &color);

                /** Draw polygon
                 *
                 * @param x array of x point coordinates
                 * @param y array of y point coordinates
                 * @param n number of elements in each array
                 * @param color polygon color
                 */
                virtual void fill_poly(const float *x, const float *y, size_t n, const Color & color);

                /** Draw polygon
                 *
                 * @param x array of x point coordinates
                 * @param y array of y point coordinates
                 * @param n number of elements in each array
                 * @param gr gradient to fille
                 */
                virtual void fill_poly(const float *x, const float *y, size_t n, IGradient *gr);

                /** Wire polygon
                 *
                 * @param x array of x point coordinates
                 * @param y array of y point coordinates
                 * @param n number of elements in each array
                 * @param width line width
                 * @param color polygon line color
                 */
                virtual void wire_poly(const float *x, const float *y, size_t n, float width, const Color & color);

                /** Draw filled and wired polygon
                 *
                 * @param x array of x point coordinates
                 * @param y array of y point coordinates
                 * @param n number of elements in each array
                 * @param width line width
                 * @param fill polygon fill color
                 * @param wire polygon wire color
                 */
                virtual void draw_poly(const float *x, const float *y, size_t n, float width, const Color &fill, const Color &wire);

                /** Fill circle
                 *
                 * @param x center x
                 * @param y center y
                 * @param r radius
                 * @param color color
                 */
                virtual void fill_circle(float x, float y, float r, const Color & color);

                /** Fill circle
                 *
                 * @param x center x
                 * @param y center y
                 * @param r radius
                 * @param g gradient
                 */
                virtual void fill_circle(float x, float y, float r, IGradient *g);

                /**
                 * Begin clipping of the rectangle area
                 * @param x left-top corner X coordinate
                 * @param y left-top corner Y coordinate
                 * @param w width
                 * @param h height
                 */
                virtual void clip_begin(float x, float y, float w, float h);

                /**
                 * End clipping
                 */
                virtual void clip_end();

                /** Get anti-aliasing
                 *
                 * @return anti-aliasing state
                 */
                virtual bool get_antialiasing();

                /** Set anti-aliasing
                 *
                 * @param set new anti-aliasing state
                 * @return previous anti-aliasing state
                 */
                virtual bool set_antialiasing(bool set);

                /** Get line cap
                 *
                 * @return line cap
                 */
                virtual surf_line_cap_t get_line_cap();

                /** Set line cap
                 *
                 * @param lc line cap
                 * @return line cap
                 */
                virtual surf_line_cap_t set_line_cap(surf_line_cap_t lc);

                /** Return difference (in bytes) between two sequential rows
                 *
                 * @return stride between rows
                 */
                virtual     size_t stride();

                /**
                 * Return raw buffer data
                 *
                 * @return raw buffer data
                 */
                virtual     void *data();

                /**
                 * Return pointer to the beginning of the specified row
                 * @param row row number
                 */
                virtual     void *row(size_t row);

                /**
                 * Start direct access to the surface
                 * @return pointer to surface buffer or NULL if error/not possible
                 */
                virtual     void *start_direct();

                /**
                 * End direct access to the surface
                 */
                virtual     void end_direct();
        };
    }

} /* namespace lsp */

#endif /* UI_WS_ISURFACE_H_ */
