/*
 * LSPFrameBuffer.h
 *
 *  Created on: 12 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_GRAPH_LSPFRAMEBUFFER_H_
#define UI_TK_WIDGETS_GRAPH_LSPFRAMEBUFFER_H_

namespace lsp
{
    namespace tk
    {
        class LSPFrameBuffer: public LSPGraphItem
        {
            public:
                static const w_class_t    metadata;

                typedef struct rgba_t
                {
                    float r, g, b, a;
                } rgba_t;

            protected:
                typedef void (LSPFrameBuffer::*calc_color_t)(float *rgba, const float *value, size_t n);

            protected:
                size_t          nChanges;       // Number of changes
                size_t          nRows;          // Number of rows in frame buffer
                size_t          nCols;          // Number of columns in frame buffer
                uint32_t        nCurrRow;       // Synchronized rowId
                float          *vData;          // Frame buffer data
                float          *vTempRGBA;      // Temporary RGBA buffer data
                uint8_t        *pData;          // Allocation pointer
                float           fTransparency;  // Frame buffer transparency
                size_t          nAngle;         // Frame buffer rotation angle 0..3
                float           fHPos;          // Horizontal position on graph widget (-1 .. 1)
                float           fVPos;          // Vertical position on graph widget (-1 .. 1)
                float           fWidth;         // Width in pixels (-1 .. 1)
                float           fHeight;        // Height in pixels (-1 .. 1)
                bool            bClear;         // Clear flag: do complete redraw
                size_t          nPalette;       // Palette used for drawing
                calc_color_t    pCalcColor;     // Function for estimating color
                LSPColor        sColor;         // Base color

                rgba_t          sColRGBA, sBgRGBA;

            protected:
                void            drop_data();
                void            calc_rainbow_color(float *rgba, const float *value, size_t n);
                void            calc_fog_color(float *rgba, const float *v, size_t n);
                void            calc_color(float *rgba, const float *value, size_t n);
                void            calc_lightness(float *rgba, const float *value, size_t n);
                void            calc_lightness2(float *rgba, const float *value, size_t n);
                void            allocate_buffer();
                void            check_color_changed();
                float          *get_buffer();
                float          *get_rgba_buffer();

            public:
                explicit LSPFrameBuffer(LSPDisplay *dpy);
                virtual ~LSPFrameBuffer();

                virtual status_t init();
                virtual void destroy();

            public:
                size_t  get_rows() const { return nRows; }
                size_t  get_cols() const { return nCols; }
                size_t  get_angle() const { return nAngle; }
                size_t  get_palette() const { return nPalette; }
                float   get_hpos() const { return fHPos; }
                float   get_vpos() const { return fVPos; }
                float   get_width() const { return fWidth; }
                float   get_height() const { return fHeight; }
                float   get_transparency() const { return fTransparency; }
                float   get_opacity() const { return 1.0f - fTransparency; }
                inline  LSPColor *color() { return &sColor; }

            public:
                status_t append_data(uint32_t row_id, const float *data);
                void set_rows(size_t rows);
                void set_cols(size_t cols);
                void set_size(size_t rows, size_t cols);
                void set_angle(size_t angle);
                void set_hpos(float value);
                void set_vpos(float value);
                void set_width(float value);
                void set_height(float value);
                void set_transparency(float value);
                void set_palette(size_t value);
                inline void set_opacity(float value) { set_transparency(1.0f - value); };

            public:
                virtual void render(ISurface *s, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_GRAPH_LSPFRAMEBUFFER_H_ */
