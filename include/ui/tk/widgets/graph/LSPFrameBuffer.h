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

            protected:
                size_t      nChanges;
                size_t      nRows;
                size_t      nCols;
                size_t      nCurrRow;
                float      *vData;
                uint8_t    *pData;
                float       fOpacity;
                size_t      nAngle;     // Rotation angle 0..3
                float       fHPos;      // Horizontal position
                float       fVPos;      // Vertical position
                float       fWidth;     // Width in pixels
                float       fHeight;    // Height in pixels
                bool        bClear;     // Clear flag
                Color       sBgColor;   // Background color

            protected:
                void        drop_data();
                void        calc_color(Color &c, float value);
                float      *get_buffer();

            public:
                explicit LSPFrameBuffer(LSPDisplay *dpy);
                virtual ~LSPFrameBuffer();

                virtual status_t init();
                virtual void destroy();

            public:
                size_t  get_rows() const { return nRows; }
                size_t  get_cols() const { return nCols; }
                size_t  get_angle() const { return nAngle; }
                float   get_hpos() const { return fHPos; }
                float   get_vpos() const { return fVPos; }
                float   get_width() const { return fWidth; }
                float   get_height() const { return fHeight; }
                float   get_opacity() const { return fOpacity; }
                inline Color *bg_color() { return &sBgColor; }

            public:
                status_t append_data(const float *data);
                void set_rows(size_t rows);
                void set_cols(size_t cols);
                void set_size(size_t rows, size_t cols);
                void set_angle(size_t angle);
                void set_hpos(float value);
                void set_vpos(float value);
                void set_width(float value);
                void set_height(float value);
                void set_opacity(float value);

            public:
                virtual void render(ISurface *s, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_GRAPH_LSPFRAMEBUFFER_H_ */
