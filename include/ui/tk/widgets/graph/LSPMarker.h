/*
 * LSPMarker.h
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPMARKER_H_
#define UI_TK_LSPMARKER_H_

namespace lsp
{
    namespace tk
    {
        class LSPMarker: public LSPGraphItem
        {
            public:
                static const w_class_t    metadata;

            protected:
                size_t          nBasisID;
                size_t          nParallelID;
                float           fValue;
                float           fOffset;
                float           fAngle;
                size_t          nWidth;
                size_t          nCenter;
                Color           sColor;

            public:
                explicit LSPMarker(LSPDisplay *dpy);
                virtual ~LSPMarker();

                virtual status_t init();

            public:
                inline size_t   basis_id() const        { return nBasisID;      }
                inline size_t   parallel_id() const     { return nParallelID;   }
                inline float    value() const           { return fValue;        }
                inline float    offset() const          { return fOffset;       }
                inline float    angle() const           { return fAngle;        }
                inline size_t   line_width() const      { return nWidth;        }
                inline size_t   center() const          { return nCenter;       }
                inline Color   *color()                 { return &sColor;       }

            public:
                void set_basis_id(size_t value);
                void set_parallel_id(size_t value);
                void set_value(float value);
                void set_offset(float value);
                void set_angle(float value);
                void set_width(size_t value);
                void set_center(size_t value);

            public:
                virtual void render(ISurface *s, bool force);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPMARKER_H_ */
