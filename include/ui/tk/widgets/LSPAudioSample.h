/*
 * LSPAudioSample.h
 *
 *  Created on: 9 июл. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPAUDIOSAMPLE_H_
#define UI_TK_WIDGETS_LSPAUDIOSAMPLE_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPAudioSample: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

            protected:
                typedef struct channel_t
                {
                    size_t          nSamples;       // Number of samples
                    size_t          nCapacity;      // Capacity
                    float          *vSamples;       // Sample values

                    float           nFadeIn;
                    float           nFadeOut;
                    LSPColor        sColor;
                    LSPColor        sFadeColor;
                    LSPColor        sLineColor;

                    explicit channel_t(LSPWidget *parent);
                } channel_t;

                enum flags_t
                {
                    AF_SHOW_DATA        = 1 << 0,
                    AF_SHOW_HINT        = 1 << 1,
                    AF_SHOW_CURR_LEN    = 1 << 2,
                    AF_SHOW_MAX_LEN     = 1 << 3
                };

            protected:
                LSPColor            sColor;
                LSPColor            sAxisColor;

                LSPString           sHint;

                LSPFont             sFont;
                LSPFont             sHintFont;
                LSPSizeConstraints  sConstraints;

                ISurface           *pGlass;
                ISurface           *pGraph;
                cvector<channel_t>  vChannels;

                size_t              nDecimSize;     // Decimation buffer size
                float              *vDecimX;        // Decimation buffer data for x
                float              *vDecimY;        // Decimation buffer data for y
                size_t              nBorder;
                size_t              nRadius;
                size_t              nStatus;
                float               fCurrLen;
                float               fMaxLen;

            protected:
                channel_t          *create_channel(color_t color);
                void                destroy_channel(channel_t *channel);
                void                destroy_data();
                ISurface           *render_graph(ISurface *s, ssize_t w, ssize_t h);
                void                drop_glass();
                void                render_channel(ISurface *s, channel_t *c, ssize_t y, ssize_t w, ssize_t h);

            public:
                explicit LSPAudioSample(LSPDisplay *dpy);
                virtual ~LSPAudioSample();

                virtual status_t init();
                virtual void destroy();

            public:
                inline const char      *hint() const { return sHint.get_utf8(); }
                inline status_t         get_hint(LSPString *dst) const { return (dst->set(&sHint)) ? STATUS_OK : STATUS_NO_MEM; };

                inline LSPSizeConstraints  *constraints()   { return &sConstraints; }

                inline LSPFont         *font() { return &sFont; }
                inline LSPFont         *hint_font() { return &sHintFont; }

                inline LSPColor        *color() { return &sColor; }
                inline LSPColor        *bg_color() { return &sBgColor; }
                inline LSPColor        *axis_color() { return &sAxisColor; }

                inline size_t           channels() const { return vChannels.size(); }
                inline ssize_t          channel_samples(size_t i) const { const channel_t *c = (const_cast<LSPAudioSample *>(this))->vChannels.get(i); return (c != NULL) ? c->nSamples : -1; }
                inline float            channel_fade_in(size_t i) const { const channel_t *c = (const_cast<LSPAudioSample *>(this))->vChannels.get(i); return (c != NULL) ? c->nFadeIn : -1.0f; }
                inline float            channel_fade_out(size_t i) const { const channel_t *c = (const_cast<LSPAudioSample *>(this))->vChannels.get(i); return (c != NULL) ? c->nFadeOut : -1.0f; }
                inline const float     *channel_data(size_t i) const { const channel_t *c = (const_cast<LSPAudioSample *>(this))->vChannels.get(i); return ((c != NULL) && (c->vSamples != NULL)) ? c->vSamples : NULL; }
                inline LSPColor        *channel_color(size_t i) { channel_t *c = vChannels.get(i); return (c != NULL) ? &c->sColor : NULL; }
                inline LSPColor        *channel_fade_color(size_t i) { channel_t *c = vChannels.get(i); return (c != NULL) ? &c->sFadeColor : NULL; }
                inline LSPColor        *channel_line_color(size_t i) { channel_t *c = vChannels.get(i); return (c != NULL) ? &c->sLineColor : NULL; }

                inline bool             show_data() const           { return nStatus & AF_SHOW_DATA; }
                inline bool             show_hint() const           { return nStatus & AF_SHOW_HINT; }
                inline bool             show_curr_length() const    { return nStatus & AF_SHOW_CURR_LEN; }
                inline bool             show_max_length() const     { return nStatus & AF_SHOW_CURR_LEN; }

                inline size_t           radius() const { return nRadius; }
                inline size_t           border() const { return nBorder; }
                inline float            curr_length() const         { return fCurrLen; }
                inline float            max_length() const          { return fMaxLen; }

            public:
                status_t        set_hint(const char *text);
                status_t        set_hint(const LSPString *text);

                status_t        set_channels(size_t n);
                status_t        add_channel();
                status_t        add_channels(size_t n);
                status_t        remove_channel(size_t i);
                status_t        swap_channels(size_t a, size_t b);

                status_t        set_channel_fade_in(size_t i, float value);
                status_t        set_channel_fade_out(size_t i, float value);
                status_t        set_channel_data(size_t i, size_t samples, const float *data);
                status_t        clear_channel_data(size_t i);
                status_t        clear_all_channel_data();

                status_t        set_radius(size_t radius);
                status_t        set_border(size_t border);

                void            set_show_data(bool value = true);
                void            set_show_hint(bool value = true);
                void            set_show_curr_length(bool value = true);
                void            set_show_max_length(bool value = true);
                void            set_curr_length(float value);
                void            set_max_length(float value);

            public:
                virtual void draw(ISurface *s);

                virtual void size_request(size_request_t *r);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPAUDIOSAMPLE_H_ */
