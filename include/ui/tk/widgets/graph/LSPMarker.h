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
                enum flags_t
                {
                    F_EDITABLE      = 1 << 0,
                    F_HIGHLIGHT     = 1 << 1,
                    F_EDITING       = 1 << 2,
                    F_FINE_TUNE     = 1 << 3
                };

            protected:
                size_t          nBasisID;
                size_t          nParallelID;
                float           fValue;
                float           fLast;
                float           fOffset;
                float           fAngle;
                float           fDX;
                float           fDY;
                float           fMin;
                float           fMax;
                size_t          nWidth;
                size_t          nCenter;
                ssize_t         nBorder;
                LSPColor        sColor;
                size_t          nXFlags;
                ssize_t         nMouseX;
                ssize_t         nMouseY;
                size_t          nMouseBtn;

            protected:
                void            apply_motion(ssize_t x, ssize_t y);
                float           limit_value(float value);

            public:
                explicit LSPMarker(LSPDisplay *dpy);
                virtual ~LSPMarker();

                virtual status_t init();

            public:
                inline size_t       basis_id() const        { return nBasisID;      }
                inline size_t       parallel_id() const     { return nParallelID;   }
                inline float        value() const           { return fValue;        }
                inline float        offset() const          { return fOffset;       }
                inline float        angle() const           { return fAngle;        }
                inline size_t       line_width() const      { return nWidth;        }
                inline size_t       center() const          { return nCenter;       }
                inline LSPColor    *color()                 { return &sColor;       }
                inline size_t       border() const          { return nBorder;       }
                inline bool         editable() const        { return nXFlags & F_EDITABLE; }
                inline float        minimum() const         { return fMin;          }
                inline float        maximum() const         { return fMax;          }

            public:
                void                set_basis_id(size_t value);
                void                set_parallel_id(size_t value);
                void                set_value(float value);
                void                set_offset(float value);
                void                set_angle(float value);
                void                set_direction(float dx, float dy);
                inline void         set_dir_x(float dx)                 { set_direction(dx, fDY); }
                inline void         set_dir_y(float dy)                 { set_direction(fDX, dy); }
                void                set_width(size_t value);
                void                set_center(size_t value);
                void                set_border(ssize_t value);
                void                set_editable(bool value=true);
                void                set_minimum(float value);
                void                set_maximum(float value);

            public:
                virtual void render(ISurface *s, bool force);

                virtual bool inside(ssize_t x, ssize_t y);

                virtual status_t on_mouse_in(const ws_event_t *e);

                virtual status_t on_mouse_out(const ws_event_t *e);

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPMARKER_H_ */
