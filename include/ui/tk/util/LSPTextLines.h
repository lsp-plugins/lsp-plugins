/*
 * LSPTextLines.h
 *
 *  Created on: 26 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPTEXTLINES_H_
#define UI_TK_LSPTEXTLINES_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPTextLines
        {
            protected:
                LSPWidget      *pWidget;
                char           *sText;
                char           *sLines;
                cvector<char>   vLines;

            public:
                explicit LSPTextLines(LSPWidget *widget);
                ~LSPTextLines();

                void        flush();

            public:
                inline bool is_null() const { return sText == NULL; }
                inline size_t lines() const { return vLines.size(); }
                inline const char *text() const { return sText; }
                inline const char *line(size_t idx) const { LSPTextLines *_this=const_cast<LSPTextLines *>(this); return _this->vLines[idx]; }
                status_t get_text(LSPString *dst) const;

            public:
                status_t    set_text(const char *text);

                void        calc_text_params(ISurface *s, Font *f, ssize_t *w, ssize_t *h);
                void        calc_text_params(ISurface *s, LSPFont *f, ssize_t *w, ssize_t *h);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_TK_LSPTEXTLINES_H_ */
