/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 26 июл. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
