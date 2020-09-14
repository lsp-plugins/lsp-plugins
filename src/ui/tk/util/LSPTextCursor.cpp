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

namespace lsp
{
    namespace tk
    {
        ssize_t LSPTextCursor::limit(ssize_t value)
        {
            return value;
        }

        void LSPTextCursor::on_change()
        {

        }

        void LSPTextCursor::on_blink()
        {
        }

        status_t LSPTextCursor::update_blink(timestamp_t ts, void *arg)
        {
            if (arg == NULL)
                return STATUS_BAD_ARGUMENTS;

            LSPTextCursor *c = static_cast<LSPTextCursor *>(arg);
            c->on_blink();
            c->nFlags       ^= F_SHINE;

            return STATUS_OK;
        }

        LSPTextCursor::LSPTextCursor(LSPDisplay *dpy)
        {
            nLocation       = 0;
            nFlags          = 0;
            nBlinkPeriod    = 500;

            sTimer.bind(dpy);
            sTimer.set_handler(update_blink, this);
        }

        LSPTextCursor::~LSPTextCursor()
        {
        }

        void LSPTextCursor::hide()
        {
            if (!(nFlags & F_VISIBLE))
                return;
            nFlags &= ~F_VISIBLE;
            sTimer.cancel();
            on_change();
        }

        void LSPTextCursor::set_visibility(bool visible)
        {
            size_t flags    = nFlags;
            nFlags          = (visible) ? nFlags | F_VISIBLE : nFlags & (~F_VISIBLE);
            if (flags == nFlags)
                return;

            if (visible)
            {
                nFlags         &= ~F_SHINE;
                if (nBlinkPeriod > 0)
                    sTimer.launch(-1, nBlinkPeriod);
            }
            else
                sTimer.cancel();
            on_change();
        }

        void LSPTextCursor::show()
        {
            if (nFlags & F_VISIBLE)
                return;
            nFlags |= F_VISIBLE;
            nFlags &= ~F_SHINE;
            if (nBlinkPeriod > 0)
                sTimer.launch(-1, nBlinkPeriod);
            on_change();
        }

        void LSPTextCursor::set_shining(bool blink)
        {
            if (!(nFlags & F_VISIBLE))
                return;
            nFlags          = (blink) ? nFlags | F_SHINE : nFlags & (~F_SHINE);
            if (nBlinkPeriod > 0)
                sTimer.launch(-1, nBlinkPeriod, nBlinkPeriod);
        }

        void LSPTextCursor::blink(bool blink)
        {
            set_shining(blink);
            on_change();
        }

        void LSPTextCursor::set_inserting(bool value)
        {
            size_t flags    = nFlags;
            nFlags          = (value) ? nFlags & (~F_REPLACE) : nFlags | F_REPLACE;
            if (flags == nFlags)
                return;
            on_change();
        }

        void LSPTextCursor::set_replacing(bool value)
        {
            size_t flags    = nFlags;
            nFlags          = (value) ? nFlags | F_REPLACE : nFlags & (~F_REPLACE);
            if (flags == nFlags)
                return;
            on_change();
        }

        void LSPTextCursor::toggle_mode()
        {
            nFlags         ^= F_REPLACE;
            on_change();
        }

        void LSPTextCursor::toggle_visibility()
        {
            nFlags         ^= F_VISIBLE;
            if (nFlags & F_VISIBLE)
            {
                nFlags &= ~F_SHINE;
                if (nBlinkPeriod > 0)
                    sTimer.launch(-1, nBlinkPeriod);
            }
            else
                sTimer.cancel();

            on_change();
        }

        void LSPTextCursor::set_blink_period(size_t value)
        {
            if (nBlinkPeriod == value)
                return;

            nBlinkPeriod = value;
            if (nFlags & F_VISIBLE)
            {
                sTimer.cancel();
                if (nBlinkPeriod > 0)
                    sTimer.launch(-1, nBlinkPeriod);
            }

            on_change();
        }

        ssize_t LSPTextCursor::set(ssize_t location)
        {
            location        = limit(location);
            if (location != nLocation)
            {
                nLocation       = location;
                on_change();
            }
            return location;
        }

        ssize_t LSPTextCursor::move(ssize_t distance)
        {
            distance        = limit(nLocation + distance);
            if (distance != nLocation)
            {
                nLocation       = distance;
                on_change();
            }
            return distance;
        }
    } /* namespace tk */
} /* namespace lsp */
