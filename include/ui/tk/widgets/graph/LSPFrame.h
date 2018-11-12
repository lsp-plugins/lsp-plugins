/*
 * LSPFrame.h
 *
 *  Created on: 12 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_GRAPH_LSPFRAME_H_
#define UI_TK_WIDGETS_GRAPH_LSPFRAME_H_

namespace lsp
{
    namespace tk
    {
        class LSPFrame: public LSPGraphItem
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
                ssize_t     nHPos;      // Horizontal position
                ssize_t     nVPos;      // Vertical position
                ssize_t     nWidth;     // Width in pixels
                ssize_t     nHeight;    // Height in pixels
                bool        bClear;     // Clear flag
                Color       sBgColor;   // Background color

            protected:
                void        drop_data();
                float      *get_buffer();

            public:
                explicit LSPFrame(LSPDisplay *dpy);
                virtual ~LSPFrame();

                virtual status_t init();
                virtual void destroy();

            public:
                size_t  get_rows() const { return nRows; }
                size_t  get_cols() const { return nCols; }
                size_t  get_angle() const { return nAngle; }
                ssize_t get_hpos() const { return nHPos; }
                ssize_t get_vpos() const { return nVPos; }
                ssize_t get_width() const { return nWidth; }
                ssize_t get_height() const { return nHeight; }
                float   get_opacity() const { return fOpacity; }
                inline Color *bg_color() { return &sBgColor; }

            public:
                status_t append_data(const float *data);
                void set_rows(size_t rows);
                void set_cols(size_t cols);
                void set_angle(size_t angle);
                void set_hpos(ssize_t value);
                void set_vpos(ssize_t value);
                void set_width(ssize_t value);
                void set_height(ssize_t value);
                void set_opacity(float value);

            public:
                virtual void render(ISurface *s, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_GRAPH_LSPFRAME_H_ */
