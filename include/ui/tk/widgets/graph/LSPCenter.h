/*
 * LSPCenter.h
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPCENTER_H_
#define UI_TK_LSPCENTER_H_

namespace lsp
{
    namespace tk
    {
        class LSPCenter: public LSPGraphItem
        {
            public:
                static const w_class_t    metadata;

            private:
                float           fLeft;
                float           fTop;
                float           fRadius;
                LSPColor        sColor;

            public:
                explicit LSPCenter(LSPDisplay *dpy);
                virtual ~LSPCenter();

                virtual status_t init();

            public:
                inline LSPColor *color()            { return &sColor;   };
                inline float radius() const         { return fRadius;   };
                inline float canvas_left() const    { return fLeft;     };
                inline float canvas_top() const     { return fTop;      };

            public:
                void set_radius(float value);
                void set_canvas_left(float value);
                void set_canvas_top(float value);

            public:
                virtual void render(ISurface *s, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPCENTER_H_ */
