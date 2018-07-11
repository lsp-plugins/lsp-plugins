/*
 * LSPLed.h
 *
 *  Created on: 10 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPLED_H_
#define UI_TK_LSPLED_H_

namespace lsp
{
    namespace tk
    {
        class LSPLed: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                Color               sBgColor;
                Color               sColor;
                size_t              nSize;
                bool                bOn;

            public:
                explicit LSPLed(LSPDisplay *dpy);
                virtual ~LSPLed();

                virtual status_t init();

            public:
                inline Color *bg_color()    { return &sBgColor; }

                inline Color *color()       { return &sColor; }

                inline bool on() const      { return bOn; }

                inline bool off() const     { return !bOn; }

                inline size_t size() const  { return nSize; }

            public:
                void set_on(bool on = true);

                void set_off(bool off = true);

                void set_size(size_t size);

            public:
                virtual void draw(ISurface *s);

                virtual void size_request(size_request_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPLED_H_ */
