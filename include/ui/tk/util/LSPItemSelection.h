/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 9 авг. 2017 г.
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

#ifndef UI_TK_UTIL_LSPITEMSELECTION_H_
#define UI_TK_UTIL_LSPITEMSELECTION_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPItemSelection
        {
            protected:
                cstorage<ssize_t>   vIndexes;
                bool                bMultiple;

            protected:
                virtual void on_remove(ssize_t value);

                virtual void on_add(ssize_t value);

                virtual bool validate(ssize_t value);

                virtual void request_fill(ssize_t *first, ssize_t *last);

                virtual void on_fill();

                virtual void on_clear();

            protected:
                status_t        insert_item(ssize_t value);

                status_t        remove_item(ssize_t value);

                status_t        swap_items(ssize_t index1, ssize_t index2);

            public:
                explicit LSPItemSelection();
                virtual ~LSPItemSelection();

            public:
                inline size_t   size() const        { return vIndexes.size(); }

                ssize_t         value() const;

                bool            contains(ssize_t value) const;

                bool            multiple() const    { return bMultiple; };
                bool            single() const      { return !bMultiple; };

            public:
                status_t        put(ssize_t value);

                status_t        set_value(ssize_t value);

                status_t        toggle_value(ssize_t value);

                status_t        remove(ssize_t value);

                void            clear();

                void            fill();

                ssize_t         get(size_t index) const;

                status_t        set_multiple(bool multiple = true);

                inline status_t set_single(bool single = true) { return set_multiple(!single); };
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPITEMSELECTION_H_ */
