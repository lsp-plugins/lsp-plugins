/*
 * LSPTextCursor.h
 *
 *  Created on: 4 сент. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPTEXTCURSOR_H_
#define UI_TK_UTIL_LSPTEXTCURSOR_H_

namespace lsp
{
    namespace tk
    {
        class LSPTextCursor
        {
            protected:
                enum flags_t
                {
                    F_VISIBLE       = 1 << 0,
                    F_REPLACE       = 1 << 1,
                    F_SHINE         = 1 << 2
                };

            protected:
                ssize_t     nLocation;
                size_t      nFlags;
                size_t      nBlinkPeriod;
                LSPTimer    sTimer;

            protected:
                static status_t update_blink(timestamp_t ts, void *arg);

                virtual ssize_t limit(ssize_t value);
                virtual void on_change();
                virtual void on_blink();

                void        set_shining(bool blink = true);

            public:
                explicit LSPTextCursor(LSPDisplay *dpy = NULL);
                virtual ~LSPTextCursor();

            public:
                inline bool visible() const     { return nFlags & F_VISIBLE; };
                inline bool inserting() const   { return !(nFlags & F_REPLACE); };
                inline bool replacing() const   { return nFlags & F_REPLACE; };
                inline bool shining() const     { return nFlags & F_SHINE; };
                inline size_t blink_period() const  { return nBlinkPeriod; }
                inline ssize_t value() const    { return nLocation; };
                inline ssize_t location() const { return nLocation; };
                inline ssize_t position() const { return nLocation; };

            public:
                void        hide();
                void        show();
                void        blink(bool blink = true);
                void        shine(bool shine = true) { blink(shine); };
                void        set_visibility(bool visible = true);
                void        set_inserting(bool value = true);
                void        set_replacing(bool value = true);
                void        toggle_mode();
                void        toggle_visibility();
                void        set_blink_period(size_t value);
                ssize_t     set(ssize_t location);
                inline ssize_t     set_value(ssize_t location)      { return set(location); };
                inline ssize_t     set_location(ssize_t location)   { return set(location); };
                inline ssize_t     set_position(ssize_t location)   { return set(location); };
                ssize_t     move(ssize_t distance);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPTEXTCURSOR_H_ */
