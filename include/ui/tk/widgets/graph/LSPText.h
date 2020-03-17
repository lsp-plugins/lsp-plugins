/*
 * LSPText.h
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPTEXT_H_
#define UI_TK_LSPTEXT_H_

namespace lsp
{
    namespace tk
    {
        class LSPText: public LSPGraphItem
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct coord_t
                {
                    size_t      nBasis;
                    float       fCoord;
                } coord_t;

            protected:
                LSPLocalString  sText;
                size_t          nCoords;
                coord_t        *vCoords;
                float           fHAlign;
                float           fVAlign;
                size_t          nCenter;
                LSPFont         sFont;

            protected:
                void do_destroy();

            public:
                explicit LSPText(LSPDisplay *dpy);
                virtual ~LSPText();

                virtual status_t init();
                virtual void destroy();

            public:
                inline size_t get_axes() const { return nCoords; }
                float get_coord(size_t axis) const;
                size_t get_basis(size_t axis) const;

                inline const LSPLocalString *text() const { return &sText; }
                inline LSPLocalString *text()       { return &sText; }

                inline float halign() const { return fHAlign; }
                inline float valign() const { return fVAlign; }
                inline LSPFont *font() { return &sFont; }
                inline size_t center() const { return nCenter; }

            public:
                status_t set_axes(size_t axes);
                status_t set_coord(size_t axis, float value);
                status_t set_basis(size_t axis, size_t value);
                void set_halign(float value);
                void set_valign(float value);
                void set_center(size_t value);

            public:
                virtual void render(ISurface *s, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPTEXT_H_ */
