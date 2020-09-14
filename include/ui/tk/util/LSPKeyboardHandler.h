/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 сент. 2017 г.
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

#ifndef UI_TK_UTIL_LSPKEYBOARDHANDLER_H_
#define UI_TK_UTIL_LSPKEYBOARDHANDLER_H_

namespace lsp
{
    namespace tk
    {
        class LSPKeyboardHandler: public ws::IEventHandler
        {
            protected:
                enum constants_t
                {
                    RPT_BUF_SIZE        = 64
                };

            protected:
                size_t      nPause;                 // Pause before repeat
                size_t      nRepeat;                // Repeat
                size_t      nRepeatSize;            // Number of elements in repeat buffer
                ws_event_t  sLast;                  // Last received event
                ws_code_t   vRepeat[RPT_BUF_SIZE];  // Repeat buffer
                LSPTimer    sTimer;                 // Timer to simulate character repeat

            protected:
                status_t    process_key_down(const ws_event_t *e);
                status_t    process_key_up(const ws_event_t *e);
                static status_t simulate_repeat(timestamp_t ts, void *arg);
                void        simulate_repeat(timestamp_t ts);

            public:
                LSPKeyboardHandler();
                virtual ~LSPKeyboardHandler();

                status_t    init(LSPDisplay *dpy);

            public:
                size_t      pause_time() const       { return nPause; };
                size_t      repeat_time() const      { return nRepeat; };

            public:
                void        set_pause_time(size_t value);
                void        set_repeat_time(size_t value);

            public:
                static      ws_code_t translate_keypad(ws_code_t code);

            public:
                // Event handling callbacks
                virtual status_t handle_event(const ws_event_t *e);

                // Event handling callbacks
                virtual status_t on_key_down(const ws_event_t *e);

                virtual status_t on_key_press(const ws_event_t *e);

                virtual status_t on_key_up(const ws_event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPKEYBOARDHANDLER_H_ */
