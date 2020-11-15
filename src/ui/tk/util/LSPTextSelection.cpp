/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 4 сент. 2017 г.
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

#include <ui/tk/tk.h>
#include <limits.h>

namespace lsp
{
    namespace tk
    {
        LSPTextSelection::LSPTextSelection()
        {
            nFirst      = -1;
            nLast       = -1;
        }

        LSPTextSelection::~LSPTextSelection()
        {
        }

        ssize_t LSPTextSelection::limit(ssize_t value)
        {
            return value;
        }

        void LSPTextSelection::on_change()
        {
        }

        void LSPTextSelection::set(ssize_t first, ssize_t last)
        {
            first = (first >= 0) ? limit(first) : -1;
            last  = (last >= 0) ? limit(last) : -1;

            if (first <= last)
            {
                if ((nFirst != first) || (nLast != last))
                {
                    nFirst      = first;
                    nLast       = last;
                    on_change();
                }
            }
            else
            {
                if ((nFirst != last) || (nLast != first))
                {
                    nFirst      = last;
                    nLast       = first;
                    on_change();
                }
            }
        }

        void LSPTextSelection::set_all()
        {
            ssize_t first = limit(0);
            ssize_t last  = limit(SSIZE_MAX);

            if ((nFirst != last) || (nLast != first))
            {
                nFirst      = last;
                nLast       = first;
                on_change();
            }
        }

        void LSPTextSelection::set(ssize_t first)
        {
            first = (first >= 0) ? limit(first) : -1;
            if ((nFirst != first) || (nLast != first))
            {
                nFirst      = first;
                nLast       = first;
                on_change();
            }
        }

        void LSPTextSelection::set_first(ssize_t value)
        {
            value = (value >= 0) ? limit(value) : -1;
            if (value != nFirst)
            {
                nFirst      = value;
                on_change();
            }
        }

        void LSPTextSelection::set_last(ssize_t value)
        {
            value = (value >= 0) ? limit(value) : -1;
            if (value != nLast)
            {
                nLast      = value;
                on_change();
            }
        }

        void LSPTextSelection::truncate()
        {
            if (nLast == nFirst)
                return;

            nLast   = nFirst;
            on_change();
        }

        void LSPTextSelection::unset()
        {
            if ((nLast == -1) && (nFirst == -1))
                return;

            nLast   = -1;
            nFirst  = -1;
            on_change();
        }

        bool LSPTextSelection::intersection(ssize_t first, ssize_t last) const
        {
            if (invalid())
                return false;

            ssize_t xfirst, xlast;
            if (nFirst < nLast)
            {
                xfirst  = nFirst;
                xlast   = nLast;
            }
            else
            {
                xfirst  = nLast;
                xlast   = nFirst;
            }

            ssize_t yfirst, ylast;
            if (first < last)
            {
                yfirst  = first;
                ylast   = last;
            }
            else
            {
                yfirst  = last;
                ylast   = first;
            }

            if (xlast <= yfirst)
                return false;
            if (xfirst >= ylast)
                return false;
            return true;
        }

        bool LSPTextSelection::intersection(const LSPTextSelection *src) const
        {
            if ((invalid()) || (src->invalid()))
                return false;

            ssize_t xfirst, xlast;
            if (nFirst < nLast)
            {
                xfirst  = nFirst;
                xlast   = nLast;
            }
            else
            {
                xfirst  = nLast;
                xlast   = nFirst;
            }

            ssize_t yfirst; //, ylast;
            if (src->nFirst < src->nLast)
            {
                yfirst  = src->nFirst;
//                ylast   = src->nLast;
            }
            else
            {
                yfirst  = src->nLast;
//                ylast   = src->nFirst;
            }

            if (xlast <= yfirst)
                return false;
            if (xfirst >= xlast)
                return false;
            return true;
        }

        void LSPTextSelection::read_range(ssize_t *first, ssize_t *last) const
        {
            if (nFirst <= nLast)
            {
                *first      = nFirst;
                *last       = nLast;
            }
            else
            {
                *first      = nLast;
                *last       = nFirst;
            }
        }

        bool LSPTextSelection::contains(ssize_t value) const
        {
            if (invalid())
                return false;

            ssize_t xfirst, xlast;
            if (nFirst < nLast)
            {
                xfirst  = nFirst;
                xlast   = nLast;
            }
            else
            {
                xfirst  = nLast;
                xlast   = nFirst;
            }

            return (value >= xfirst) && (value <= xlast);
        }

    } /* namespace tk */
} /* namespace lsp */
