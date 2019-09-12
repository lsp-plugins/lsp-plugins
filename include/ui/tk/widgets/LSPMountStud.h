/*
 * LSPMountStud.h
 *
 *  Created on: 7 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPMOUNTSTUD_H_
#define UI_TK_WIDGETS_LSPMOUNTSTUD_H_

namespace lsp
{
    namespace tk
    {
        class LSPMountStud: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                LSPFont             sFont;
                LSPString           sText;
                LSPColor            sColor;
                size_t              nAngle;
                size_t              nButtons;
                bool                bPressed;
                realize_t           sLogo;

            protected:
                static status_t slot_on_submit(LSPWidget *sender, void *ptr, void *data);

                bool mouse_over_logo(ssize_t x, ssize_t y);
                void draw_screw(ISurface *s, ssize_t x, ssize_t y, float angle);

            public:
                explicit LSPMountStud(LSPDisplay *dpy);
                virtual ~LSPMountStud();

                virtual status_t init();
                virtual void destroy();

            public:
                inline LSPFont     *font() { return &sFont; }
                inline LSPColor    *color() { return &sColor; }
                inline const char  *text() { return sText.get_native(); }
                status_t            get_text(LSPString *dst);
                inline size_t       angle() const { return nAngle; }

            public:
                status_t            set_text(const char *text);
                status_t            set_text(const LSPString *src);
                void                set_angle(size_t value);

            public:
                virtual void        draw(ISurface *s);

                virtual void        size_request(size_request_t *r);

                virtual void        realize(const realize_t *r);

                virtual status_t    on_mouse_down(const ws_event_t *e);

                virtual status_t    on_mouse_up(const ws_event_t *e);

                virtual status_t    on_mouse_move(const ws_event_t *e);

                virtual status_t    on_submit();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPMOUNTSTUD_H_ */
