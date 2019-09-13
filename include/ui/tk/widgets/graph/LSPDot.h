/*
 * LSPDot.h
 *
 *  Created on: 19 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPDOT_H_
#define UI_TK_LSPDOT_H_

namespace lsp
{
    namespace tk
    {
        class LSPDot: public LSPGraphItem
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum flags_t
                {
                    F_X_EDITABLE    = 1 << 0,
                    F_Y_EDITABLE    = 1 << 1,
                    F_Z_EDITABLE    = 1 << 2,
                    F_HIGHLIGHT     = 1 << 3,
                    F_EDITING       = 1 << 4,
                    F_FINE_TUNE     = 1 << 5
                };

                typedef struct param_t
                {
                    float           fMin;
                    float           fMax;
                    float           fValue;
                    float           fStep;
                    float           fTinyStep;
                    float           fBigStep;
                    float           fLast;
                } param_t;

            protected:
                param_t         sLeft;
                param_t         sTop;
                param_t         sScroll;

                size_t          nCenter;
                size_t          nFlags;
                size_t          nBasisID;
                size_t          nParallelID;
                ssize_t         nRealX;
                ssize_t         nRealY;
                ssize_t         nMouseX;
                ssize_t         nMouseY;
                ssize_t         nDMouseX;
                ssize_t         nDMouseY;
                size_t          nMouseBtn;
                size_t          nBorder;
                size_t          nPadding;
                size_t          nSize;
                LSPColor        sColor;

            protected:
                void apply_motion(ssize_t dx, ssize_t dy);
                void set_value(float *v, float value);
                void set_limit_value(param_t *v, size_t flag, float value);
                void set_flag(size_t flag, bool value);
                static float limit_value(const param_t *param, float value);

            public:
                explicit LSPDot(LSPDisplay *dpy);
                virtual ~LSPDot();

                virtual status_t init();

            public:
                inline LSPColor    *color()             { return &sColor; }
                inline float        x_minimum() const   { return sLeft.fMin;        }
                inline float        x_maximum() const   { return sLeft.fMax;        }
                inline float        x_value() const     { return sLeft.fValue;      }
                inline float        x_step() const      { return sLeft.fStep;       }
                inline float        x_tiny_step() const { return sLeft.fTinyStep;   }
                inline float        x_big_step() const  { return sLeft.fBigStep;    }
                inline bool         x_editable() const  { return nFlags & F_X_EDITABLE; }

                inline float        y_minimum() const   { return sTop.fMin;         }
                inline float        y_maximum() const   { return sTop.fMax;         }
                inline float        y_value() const     { return sTop.fValue;       }
                inline float        y_step() const      { return sTop.fStep;        }
                inline float        y_tiny_step() const { return sTop.fTinyStep;    }
                inline float        y_big_step() const  { return sTop.fBigStep;     }
                inline bool         y_editable() const  { return nFlags & F_Y_EDITABLE; }

                inline float        z_minimum() const   { return sScroll.fMin;      }
                inline float        z_maximum() const   { return sScroll.fMax;      }
                inline float        z_value() const     { return sScroll.fValue;    }
                inline float        z_step() const      { return sScroll.fStep;     }
                inline float        z_tiny_step() const { return sScroll.fTinyStep; }
                inline float        z_big_step() const  { return sScroll.fBigStep;  }
                inline bool         z_editable() const  { return nFlags & F_Z_EDITABLE; }

                inline size_t       size() const        { return nSize;             }
                inline size_t       border() const      { return nBorder;           }
                inline size_t       padding() const     { return nPadding;          }

                inline size_t       basis_id() const    { return nBasisID;          }
                inline size_t       parallel_id() const { return nParallelID;       }
                inline size_t       center_id() const   { return nCenter;           }
                inline bool         highlight() const   { return nFlags & F_HIGHLIGHT; }

            public:
                inline void         set_x_minimum(float value)  { set_value(&sLeft.fMin, value);        }
                inline void         set_x_maximum(float value)  { set_value(&sLeft.fMax, value);        }
                inline void         set_x_value(float value)    { set_limit_value(&sLeft, F_X_EDITABLE, value);  }
                inline void         set_x_step(float value)     { set_value(&sLeft.fStep, value);       }
                inline void         set_x_tiny_step(float value){ set_value(&sLeft.fTinyStep, value);   }
                inline void         set_x_big_step(float value) { set_value(&sLeft.fBigStep, value);    }
                inline void         set_x_editable(bool value=true) { set_flag(F_X_EDITABLE, value);    }

                inline void         set_y_minimum(float value)  { set_value(&sTop.fMin, value);         }
                inline void         set_y_maximum(float value)  { set_value(&sTop.fMax, value);         }
                inline void         set_y_value(float value)    { set_limit_value(&sTop, F_Y_EDITABLE, value);        }
                inline void         set_y_step(float value)     { set_value(&sTop.fStep, value);        }
                inline void         set_y_tiny_step(float value){ set_value(&sTop.fTinyStep, value);    }
                inline void         set_y_big_step(float value) { set_value(&sTop.fBigStep, value);     }
                inline void         set_y_editable(bool value=true) { set_flag(F_Y_EDITABLE, value);    }

                inline void         set_z_minimum(float value)  { set_value(&sScroll.fMin, value);      }
                inline void         set_z_maximum(float value)  { set_value(&sScroll.fMax, value);      }
                inline void         set_z_value(float value)    { set_limit_value(&sScroll, F_Z_EDITABLE, value);     }
                inline void         set_z_step(float value)     { set_value(&sScroll.fStep, value);     }
                inline void         set_z_tiny_step(float value){ set_value(&sScroll.fTinyStep, value); }
                inline void         set_z_big_step(float value) { set_value(&sScroll.fBigStep, value);  }
                inline void         set_z_editable(bool value=true) { set_flag(F_Z_EDITABLE, value);    }

                void                set_size(size_t value);
                void                set_border(size_t value);
                void                set_padding(size_t value);
                void                set_basis_id(size_t value);
                void                set_parallel_id(size_t value);
                void                set_center_id(size_t value);
                inline void         set_highlight(bool value=true) { set_flag(F_HIGHLIGHT, value);    }

            public:
                virtual void render(ISurface *s, bool force);

                virtual bool inside(ssize_t x, ssize_t y);

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_mouse_in(const ws_event_t *e);

                virtual status_t on_mouse_out(const ws_event_t *e);

                virtual status_t on_mouse_scroll(const ws_event_t *e);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPDOT_H_ */
