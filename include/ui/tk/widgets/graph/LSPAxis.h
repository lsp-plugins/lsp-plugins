/*
 * LSPAxis.h
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPAXIS_H_
#define UI_TK_LSPAXIS_H_

namespace lsp
{
    namespace tk
    {
        class LSPAxis: public LSPGraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                enum flags_t
                {
                    F_BASIS         = 1 << 0,
                    F_LOGARITHMIC   = 1 << 1
                };

                size_t          nFlags;
                float           fAngle;
                float           fDX;
                float           fDY;
                float           fMin;
                float           fMax;
                size_t          nWidth;
                ssize_t         nLength;
                size_t          nCenter;
                LSPColor        sColor;

            protected:
                void            set_flag(size_t flag, bool value);

            public:
                explicit LSPAxis(LSPDisplay *dpy);
                virtual ~LSPAxis();

                virtual status_t init();

            public:
                /** Check whether the axis is base axis
                 *
                 * @return true if axis is a base axis
                 */
                inline bool is_basis() const        { return nFlags & F_BASIS;  };

                /** Get minimum value
                 *
                 * @return minimum
                 */
                inline float min_value() const      { return fMin;              };

                /** Get maximum value
                 *
                 * @return maximum
                 */
                inline float max_value() const      { return fMax;              };

                inline bool log_scale() const       { return nFlags & F_LOGARITHMIC; }

                inline bool linear_scale() const    { return !(nFlags & F_LOGARITHMIC); }

                inline LSPColor *color()            { return &sColor;           };

                inline size_t line_width() const    { return nWidth;            };

                inline size_t center_id() const     { return nCenter;           };

                inline float angle() const          { return fAngle;            };
                inline float dir_x() const          { return fDX;               };
                inline float dir_y() const          { return fDY;               };

                inline size_t length() const        { return nLength;           }

                /** Apply axis transformation according to x and y
                 *
                 * @param x x coordinate (in pixels) of 2D-point to transform
                 * @param y y coordinate (in pixels) of 2D-point to transform
                 * @param dv delta-vector to apply for transform
                 * @param count size of x, y and dv vector elements
                 * @return true if values were applied
                 */
                bool apply(float *x, float *y, const float *dv, size_t count);

                /** Shift point coordinates in canvas coordinates respective to x and y
                 *
                 * @param x x coordinate (in pixels) of 2D-point to transform
                 * @param y y coordinate (in pixels) of 2D-point to transform
                 * @param dv delta-vector to apply for transform
                 * @param count count size of x, y and dv vector elements
                 * @return
                 */
                bool apply_canvas(float *x, float *y, const float *dv, size_t count);

                /** Project the vector on the axis and determine it's value relative to the center
                 *
                 * @param cv canvas
                 * @param x x coordinate (in pixels) of 2D-point on canvas
                 * @param y y coordinate (in pixels) of 2D-point on canvas
                 * @return the value after projection
                 */
                float project(float x, float y);

                /** Get parallel line equation
                 *
                 * @param x dot that belongs to parallel line
                 * @param y dot that belongs to parallel line
                 * @param a line equation
                 * @param b line equation
                 * @param c line equation
                 * @return true on success
                 */
                bool parallel(float x, float y, float &a, float &b, float &c);

                /** Get parallel line equation respective to the shift
                 *
                 * @param x dot that belongs to parallel line
                 * @param y dot that belongs to parallel line
                 * @param shift shift in pixels relative to the direction of axis
                 * @param nx new dot coordinate
                 * @param ny new dot coordinate
                 * @return true on success
                 */
                void ortogonal_shift(float x, float y, float shift, float &sx, float &sy);

                /** Get equation of line rotated around the point angle
                 *
                 * @param x dot that belongs to line
                 * @param y dot that belongs to line
                 * @param angle rotation angle around dot
                 * @param a line equation
                 * @param b line equation
                 * @param c line equation
                 * @return true on success
                 */
                bool angle(float x, float y, float angle, float &a, float &b, float &c);

                /** Get equation of line rotated around the point angle and shifted
                 *
                 * @param x dot that belongs to line
                 * @param y dot that belongs to line
                 * @param angle rotation angle around dot
                 * @param shift shift in pixels relative to the direction of axis
                 * @param nx new dot coordinate
                 * @param ny new dot coordinate
                 * @return true on success
                 */
                void rotate_shift(float x, float y, float angle, float shift, float &nx, float &ny);

            public:
                inline void         set_basis(bool value = true)        { set_flag(F_BASIS, value);         };

                inline void         set_log_scale(bool value = true)    { set_flag(F_LOGARITHMIC, value);   };

                inline void         set_linear_scale(bool value = true) { set_flag(F_LOGARITHMIC, !value);  };

                void                set_min_value(float value);

                void                set_max_value(float value);

                void                set_line_width(size_t value);

                void                set_center_id(size_t value);

                void                set_angle(float value);
                void                set_direction(float dx, float dy);
                inline void         set_dir_x(float dx)                 { set_direction(dx, fDY); }
                inline void         set_dir_y(float dy)                 { set_direction(fDX, dy); }

                void                set_length(ssize_t value);

            public:
                virtual void render(ISurface *s, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPAXIS_H_ */
