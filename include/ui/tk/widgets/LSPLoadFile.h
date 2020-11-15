/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 07 мая 2019 г.
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

#ifndef UI_TK_WIDGETS_LSPLOADFILE_H_
#define UI_TK_WIDGETS_LSPLOADFILE_H_

namespace lsp
{
    namespace tk
    {
        enum load_file_state_t
        {
            LFS_SELECT,
            LFS_LOADING,
            LFS_LOADED,
            LFS_ERROR
        };

        class LSPLoadFile: public LSPWidget
        {
            public:
                static const w_class_t    metadata;

                enum _load_file_state_t
                {
                    LFS_TOTAL       = LFS_ERROR + 1,

                    S_PRESSED       = 1 << 0
                };

            protected:
                typedef struct state_t
                {
                    LSPColor       *pColor;
                    LSPString       sText;
                } state_t;

            protected:
                class LoadFileSink: public LSPUrlSink
                {
                    protected:
                        LSPLoadFile         *pWidget;

                    public:
                        explicit LoadFileSink(LSPLoadFile *w);
                        virtual ~LoadFileSink();

                        void unbind();
                        virtual status_t    commit_url(const LSPString *url);
                };

            protected:
                load_file_state_t   nState;
                state_t             vStates[LFS_TOTAL];
                float               fProgress;
                size_t              nButtons;
                size_t              nBtnState;
                ssize_t             nSize;
                LoadFileSink       *pSink;
                LSPFont             sFont;
                LSPFileDialog       sDialog;
                ISurface           *pDisk;
                LSPString           sPath;

            protected:
                ISurface           *render_disk(ISurface *s, ssize_t w, const Color &c, const Color &bg);
                static status_t     slot_on_activate(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_close(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_file_submit(LSPWidget *sender, void *ptr, void *data);
                static status_t     slot_on_dialog_close(LSPWidget *sender, void *ptr, void *data);

            public:
                explicit LSPLoadFile(LSPDisplay *dpy);
                virtual ~LSPLoadFile();

                virtual status_t init();
                virtual void destroy();

            public:
                inline load_file_state_t    state() const { return nState; }
                LSPColor                   *state_color(size_t i);
                const char                 *state_text(size_t i) const;
                status_t                    get_state_text(size_t i, LSPString *dst);
                inline float                progress() const { return fProgress; };
                const char                 *file_name() const;
                status_t                    get_file_name(LSPString *dst);
                inline LSPFont             *font() { return &sFont; }
                inline LSPFileFilter       *filter() { return sDialog.filter(); }
                inline status_t             get_path(LSPString *dst) const { return (dst->set(&sPath)) ? STATUS_OK : STATUS_NO_MEM; }
                inline const char          *get_path() const { return sPath.get_native(); }
                inline ssize_t              size() const { return nSize; }

            public:
                status_t    set_state(load_file_state_t state);
                status_t    set_state_text(size_t i, const char *s);
                status_t    set_state_text(size_t i, const LSPString *s);
                status_t    set_progress(float value);
                status_t    set_path(const LSPString *path);
                status_t    set_path(const char *path);
                void        set_size(ssize_t size);

            public:
                virtual void draw(ISurface *s);
                virtual void size_request(size_request_t *r);
                virtual status_t on_mouse_down(const ws_event_t *e);
                virtual status_t on_mouse_up(const ws_event_t *e);
                virtual status_t on_mouse_move(const ws_event_t *e);
                virtual status_t on_activate();
                virtual status_t on_submit();
                virtual status_t on_close();
                virtual status_t on_drag_request(const ws_event_t *e, const char * const *ctype);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_WIDGETS_LSPLOADFILE_H_ */
