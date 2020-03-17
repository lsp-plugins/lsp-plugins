/*
 * LSPAudioFile.h
 *
 *  Created on: 23 окт. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_WIDGETS_LSPAUDIOFILE_H_
#define UI_TK_WIDGETS_LSPAUDIOFILE_H_

#include <core/io/OutMemoryStream.h>

namespace lsp
{
    namespace tk
    {
        class LSPFileDialog;

        class LSPAudioFile: public LSPWidget
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

                    explicit channel_t(LSPWidget *w);
                } channel_t;

                enum flags_t
                {
                    AF_PRESSED      = 1 << 0,
                    AF_SHOW_DATA    = 1 << 1,
                    AF_SHOW_HINT    = 1 << 2,
                    AF_SHOW_FNAME   = 1 << 3
                };

            protected:

                class AudioFileSink: public LSPUrlSink
                {
                    protected:
                        LSPAudioFile           *pWidget;

                    public:
                        explicit AudioFileSink(LSPAudioFile *af);
                        virtual ~AudioFileSink();

                        void unbind();
                        virtual status_t    commit_url(const LSPString *url);
                };

            protected:
                LSPString           sFileName;
                LSPLocalString      sHint;
                LSPString           sPath;
                LSPFont             sFont;
                LSPFont             sHintFont;
                LSPSizeConstraints  sConstraints;
                LSPFileDialog       sDialog;
                LSPColor            sColor;
                LSPColor            sAxisColor;

                LSPMenu            *pPopup;         // Popup menu
                size_t              nDecimSize;     // Decimation buffer size
                float              *vDecimX;        // Decimation buffer data for x
                float              *vDecimY;        // Decimation buffer data for y

                ISurface           *pGlass;
                ISurface           *pGraph;
                cvector<channel_t>  vChannels;

                size_t              nBtnWidth;
                size_t              nBtnHeight;
                size_t              nBMask;
                size_t              nBorder;
                size_t              nRadius;
                size_t              nStatus;

                AudioFileSink      *pSink;

            protected:
                channel_t          *create_channel(color_t color);
                void                destroy_channel(channel_t *channel);
                void                destroy_data();
                bool                check_mouse_over(ssize_t x, ssize_t y);
                ISurface           *render_graph(ISurface *s, ssize_t w, ssize_t h);
                void                drop_glass();
                void                render_channel(ISurface *s, channel_t *c, ssize_t y, ssize_t w, ssize_t h);

                static status_t     slot_on_dialog_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_dialog_close(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_activate(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_close(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit LSPAudioFile(LSPDisplay *dpy);
                virtual ~LSPAudioFile();

                virtual status_t init();

                virtual void destroy();

            public:
                inline const char      *file_name() const { return sFileName.get_native(); }
                inline status_t         get_file_name(LSPString *dst) const { return (dst->set(&sFileName)) ? STATUS_OK : STATUS_NO_MEM; };

                inline LSPLocalString  *hint() { return &sHint; };
                inline const LSPLocalString  *hint() const { return &sHint; };

                inline LSPSizeConstraints  *constraints()   { return &sConstraints; }

                inline LSPFont         *font() { return &sFont; }
                inline LSPFont         *hint_font() { return &sHintFont; }

                inline LSPColor        *color() { return &sColor; }
                inline LSPColor        *axis_color() { return &sAxisColor; }

                inline const char      *get_path() const { return sPath.get_native(); }
                inline status_t         get_path(LSPString *dst) const { return (dst->set(&sPath)) ? STATUS_OK : STATUS_NO_MEM; }

                inline size_t           channels() const { return vChannels.size(); }
                inline ssize_t          channel_samples(size_t i) const { const channel_t *c = (const_cast<LSPAudioFile *>(this))->vChannels.get(i); return (c != NULL) ? c->nSamples : -1; }
                inline float            channel_fade_in(size_t i) const { const channel_t *c = (const_cast<LSPAudioFile *>(this))->vChannels.get(i); return (c != NULL) ? c->nFadeIn : -1.0f; }
                inline float            channel_fade_out(size_t i) const { const channel_t *c = (const_cast<LSPAudioFile *>(this))->vChannels.get(i); return (c != NULL) ? c->nFadeOut : -1.0f; }
                inline const float     *channel_data(size_t i) const { const channel_t *c = (const_cast<LSPAudioFile *>(this))->vChannels.get(i); return ((c != NULL) && (c->vSamples != NULL)) ? c->vSamples : NULL; }
                inline LSPColor        *channel_color(size_t i) { channel_t *c = vChannels.get(i); return (c != NULL) ? &c->sColor : NULL; }
                inline LSPColor        *channel_fade_color(size_t i) { channel_t *c = vChannels.get(i); return (c != NULL) ? &c->sFadeColor : NULL; }
                inline LSPColor        *channel_line_color(size_t i) { channel_t *c = vChannels.get(i); return (c != NULL) ? &c->sLineColor : NULL; }

                inline bool             show_data() const       { return nStatus & AF_SHOW_DATA; }
                inline bool             show_hint() const       { return nStatus & AF_SHOW_HINT; }
                inline bool             show_file_name() const  { return nStatus & AF_SHOW_FNAME; }

                inline size_t           radius() const { return nRadius; }
                inline size_t           border() const { return nBorder; }

                inline LSPFileFilter   *filter() { return sDialog.filter(); }

                inline LSPMenu         *get_popup() { return pPopup;        }

            public:
                status_t        set_file_name(const char *text);
                status_t        set_file_name(const LSPString *text);

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
                status_t        set_path(const LSPString *path);
                status_t        set_path(const char *path);

                void            set_show_data(bool value = true);
                void            set_show_hint(bool value = true);
                void            set_show_file_name(bool value = true);
                inline void     set_popup(LSPMenu *popup)   { pPopup = popup; }

            public:
                virtual bool hide();

                virtual void draw(ISurface *s);

                virtual void size_request(size_request_t *r);

                virtual status_t on_mouse_down(const ws_event_t *e);

                virtual status_t on_mouse_up(const ws_event_t *e);

                virtual status_t on_mouse_move(const ws_event_t *e);

                virtual status_t on_mouse_dbl_click(const ws_event_t *e);

                virtual status_t on_submit();

                virtual status_t on_close();

                virtual status_t on_activate();

                virtual status_t on_drag_request(const ws_event_t *e, const char * const *ctype);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPAUDIOFILE_H_ */
