/*
 * LSPMeter.h
 *
 *  Created on: 12 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPMETER_H_
#define UI_TK_LSPMETER_H_

namespace lsp
{
    namespace tk
    {
        class LSPMeter: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                enum mtr_flags_t
                {
                    MF_PEAK         = 1 << 0,
                    MF_BALANCE      = 1 << 1,
                    MF_REVERSIVE    = 1 << 2,
                    MF_TEXT         = 1 << 3,
                    MF_RED          = 1 << 4,
                    MF_YELLOW       = 1 << 5,
                    MF_INACTIVE     = 1 << 6,
                    MF_DZONE0       = 1 << 7,
                    MF_DZONE1       = 1 << 8,
                    MF_DZONE2       = 1 << 9
                };

                typedef struct channel_t
                {
                    float           fMin;           // Minimum displayed value
                    float           fMax;           // Maximum displayed value
                    float           fBalance;       // Balance
                    float           fPeak;          // Optional peak value drawn by single bar
                    float           fValue;         // Mandatory value
                    float           fRedZone;       // Red zone value
                    float           fYellowZone;    // Yellow zone value
                    float           fDarkZone[3];   // Dark zone values
                    char           *sText;          // Meter text
                    size_t          nFlags;         // Meter flags
                    LSPColor        sColor;         // Meter color
                    LSPColor        sYellow;        // Meter's yellow color
                    LSPColor        sRed;           // Meter's red color
                    LSPColor        sBalance;       // Meger's balance color
                    float           fDark[3];       // Dark zone amount

                    explicit channel_t(LSPWidget *widget);
                    ~channel_t();
                } channel_t;

            protected:
                size_t          nAngle;     // Angle 0..3
                size_t          nMWidth;    // Width
                size_t          nMHeight;   // Height
                ssize_t         nBorder;    // Border
                bool            bValues;    // Show values flag
                size_t          nSpacing;   // Spacing between meters
                LSPColor        sIndColor;  // Indication color
                LSPFont         sFont;
                channel_t     **vChannels;
                size_t          nChannels;

            protected:
                status_t        set_flag(size_t i, size_t flag, bool value);
                void            drop_data();
                void            draw_meter(ISurface *s, channel_t *c, float x, float y, ssize_t dx, ssize_t dy, float wx, float wy, size_t n);
                void            out_text(ISurface *s, channel_t *c, float x, float y);

            public:
                explicit LSPMeter(LSPDisplay *dpy);
                virtual ~LSPMeter();

                virtual status_t    init();
                virtual void        destroy();

            public:
                inline LSPColor    *ind_color()                         { return &sIndColor; }
                inline size_t       mtr_width() const                   { return nMWidth; }
                inline size_t       mtr_height() const                  { return nMHeight; }

                inline size_t       border() const                      { return nBorder; }

                inline float        mtr_min(size_t i) const             { return (i < nChannels) ? vChannels[i]->fMin : 0.0f; }
                inline float        mtr_max(size_t i) const             { return (i < nChannels) ? vChannels[i]->fMax : 0.0f; }
                inline float        mtr_balance(size_t i) const         { return (i < nChannels) ? vChannels[i]->fBalance : 0.0f; }
                inline float        mtr_peak(size_t i) const            { return (i < nChannels) ? vChannels[i]->fPeak : 0.0f; }
                inline float        mtr_value(size_t i) const           { return (i < nChannels) ? vChannels[i]->fValue : 0.0f; }
                inline float        mtr_rz_value(size_t i) const        { return (i < nChannels) ? vChannels[i]->fRedZone : 0.f; }
                inline float        mtr_yz_value(size_t i) const        { return (i < nChannels) ? vChannels[i]->fYellowZone : 0.0f; }
                inline float        mtr_dz0_value(size_t i) const       { return (i < nChannels) ? vChannels[i]->fDarkZone[0] : 0.0f; }
                inline float        mtr_dz1_value(size_t i) const       { return (i < nChannels) ? vChannels[i]->fDarkZone[1] : 0.0f; }
                inline float        mtr_dz2_value(size_t i) const       { return (i < nChannels) ? vChannels[i]->fDarkZone[2] : 0.0f; }
                inline const char  *mtr_text(size_t i) const            { return (i < nChannels) ? vChannels[i]->sText : NULL; }
                inline LSPColor    *mtr_color(size_t i)                 { return (i < nChannels) ? &vChannels[i]->sColor : NULL; }
                inline LSPColor    *mtr_rz_color(size_t i)              { return (i < nChannels) ? &vChannels[i]->sRed : NULL; }
                inline LSPColor    *mtr_yz_color(size_t i)              { return (i < nChannels) ? &vChannels[i]->sYellow : NULL; }
                inline float        mtr_dz0_amount(size_t i) const      { return (i < nChannels) ? vChannels[i]->fDark[0] : 0.0f; }
                inline float        mtr_dz1_amount(size_t i) const      { return (i < nChannels) ? vChannels[i]->fDark[1] : 0.0f; }
                inline float        mtr_dz2_amount(size_t i) const      { return (i < nChannels) ? vChannels[i]->fDark[2] : 0.0f; }
                inline LSPColor    *mtr_balance_color(size_t i)         { return (i < nChannels) ? &vChannels[i]->sBalance : NULL; }

                inline bool         mtr_peak_used(size_t i) const       { return (i < nChannels) ? vChannels[i]->nFlags & MF_PEAK : false;    }
                inline bool         mtr_balance_used(size_t i) const    { return (i < nChannels) ? vChannels[i]->nFlags & MF_BALANCE : false; }
                inline bool         mtr_text_used(size_t i) const       { return (i < nChannels) ? vChannels[i]->nFlags & MF_TEXT: false; }
                inline bool         mtr_is_reversive(size_t i) const    { return (i < nChannels) ? vChannels[i]->nFlags & MF_REVERSIVE : false; }
                inline bool         mtr_rz_used(size_t i) const         { return (i < nChannels) ? vChannels[i]->nFlags & MF_RED: false; }
                inline bool         mtr_yz_used(size_t i) const         { return (i < nChannels) ? vChannels[i]->nFlags & MF_YELLOW: false; }
                inline bool         mtr_dz0_used(size_t i) const        { return (i < nChannels) ? vChannels[i]->nFlags & MF_DZONE0: false; }
                inline bool         mtr_dz1_used(size_t i) const        { return (i < nChannels) ? vChannels[i]->nFlags & MF_DZONE1: false; }
                inline bool         mtr_dz2_used(size_t i) const        { return (i < nChannels) ? vChannels[i]->nFlags & MF_DZONE2: false; }
                inline bool         mtr_active(size_t i) const          { return (i < nChannels) ? !(vChannels[i]->nFlags & MF_INACTIVE): false; }
                inline bool         mtr_inactive(size_t i) const        { return (i < nChannels) ? vChannels[i]->nFlags & MF_INACTIVE: false; }

                inline size_t       angle() const                       { return nAngle; }
                inline size_t       channels() const                    { return nChannels; }
                inline bool         values_visible() const              { return bValues; }
                inline size_t       spacing() const                     { return nSpacing; }
                inline LSPFont     *font()                              { return &sFont; }

            public:
                status_t            set_mtr_min(size_t i, float value);
                status_t            set_mtr_max(size_t i, float value);
                status_t            set_mtr_balance(size_t i, float value);
                status_t            set_mtr_peak(size_t i, float value);
                status_t            set_mtr_value(size_t i, float value);
                status_t            set_mtr_rz_value(size_t i, float value);
                status_t            set_mtr_yz_value(size_t i, float value);
                status_t            set_mtr_dz0_value(size_t i, float value);
                status_t            set_mtr_dz1_value(size_t i, float value);
                status_t            set_mtr_dz2_value(size_t i, float value);
                status_t            set_mtr_dz0_amount(size_t i, float value);
                status_t            set_mtr_dz1_amount(size_t i, float value);
                status_t            set_mtr_dz2_amount(size_t i, float value);
                status_t            set_mtr_text(size_t i, const char *text);

                inline status_t     set_mtr_peak_used(size_t i, bool value = true)      { return set_flag(i, MF_PEAK, value);       }
                inline status_t     set_mtr_balance_used(size_t i, bool value = true)   { return set_flag(i, MF_BALANCE, value);    }
                inline status_t     set_mtr_text_used(size_t i, bool value = true)      { return set_flag(i, MF_TEXT, value);       }
                inline status_t     set_mtr_reversive(size_t i, bool value = true)      { return set_flag(i, MF_REVERSIVE, value);  }
                inline status_t     set_mtr_rz_used(size_t i, bool value = true)        { return set_flag(i, MF_RED, value);        }
                inline status_t     set_mtr_yz_used(size_t i, bool value = true)        { return set_flag(i, MF_YELLOW, value);     }
                inline status_t     set_mtr_dz0_used(size_t i, bool value = true)       { return set_flag(i, MF_DZONE0, value);     }
                inline status_t     set_mtr_dz1_used(size_t i, bool value = true)       { return set_flag(i, MF_DZONE1, value);     }
                inline status_t     set_mtr_dz2_used(size_t i, bool value = true)       { return set_flag(i, MF_DZONE2, value);     }
                inline status_t     set_mtr_inactive(size_t i, bool value = true)       { return set_flag(i, MF_INACTIVE, value);   }
                inline status_t     set_mtr_active(size_t i, bool value = true)         { return set_flag(i, MF_INACTIVE, !value);  }

                status_t            set_channels(size_t channels);

                void                set_angle(size_t value);
                void                set_border(size_t value);
                void                set_mtr_width(size_t value);
                void                set_mtr_height(size_t value);
                void                show_values(bool show = true);
                inline void         hide_values(bool hide = true)                       { show_values(!hide); }
                void                set_spacing(size_t spacing);

            public:
                virtual void        size_request(size_request_t *r);

                virtual void        draw(ISurface *s);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPMETER_H_ */
