/*
 * CtlAudioFile.cpp
 *
 *  Created on: 28 окт. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <ui/common.h>
#include <core/files/config.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlAudioFile::CtlAudioFile(CtlRegistry *src, LSPAudioFile *af):
            CtlWidget(src, af),
            sMenu(af->display())
        {
            pFile           = NULL;
            pMesh           = NULL;
            pPathID         = NULL;

            pStatus         = NULL;
            pLength         = NULL;
            pHeadCut        = NULL;
            pTailCut        = NULL;
            pFadeIn         = NULL;
            pFadeOut        = NULL;
            pPath           = NULL;

            for (size_t i=0; i<N_MENU_ITEMS; ++i)
                vMenuItems[i]   = NULL;
        }
        
        CtlAudioFile::~CtlAudioFile()
        {
            if (pPathID != NULL)
            {
                free(pPathID);
                pPathID = NULL;
            }
            sMenu.destroy();

            for (size_t i=0; i<N_MENU_ITEMS; ++i)
                if (vMenuItems[i] != NULL)
                {
                    vMenuItems[i]->destroy();
                    delete vMenuItems[i];
                    vMenuItems[i] = NULL;
                }
        }

        void CtlAudioFile::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPAudioFile *af    = widget_cast<LSPAudioFile>(pWidget);
            if (af == NULL)
                return;

            // Initialize color controllers
            sColor.init_basic(pRegistry, af, af->color(), A_COLOR);
            sBgColor.init_basic(pRegistry, af, af->bg_color(), A_BG_COLOR);
            sPadding.init(af->padding());

            af->slots()->bind(LSPSLOT_ACTIVATE, slot_on_activate, this);
            af->slots()->bind(LSPSLOT_SUBMIT, slot_on_submit, this);
            af->slots()->bind(LSPSLOT_CLOSE, slot_on_close, this);

            // Initialize menu
            LSP_VSTATUS_ASSERT(sMenu.init());

            // Fill items
            size_t off = 0;
            ui_handler_id_t id = 0;
            LSP_VSTATUS_ASSERT(sMenu.init());
            LSPMenuItem *mi = new LSPMenuItem(af->display());
            if (mi == NULL)
                return;
            vMenuItems[off++] = mi;
            LSP_VSTATUS_ASSERT(mi->init());
            LSP_VSTATUS_ASSERT(sMenu.add(mi));
            LSP_VSTATUS_ASSERT(mi->set_text("Cut"));
            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_cut_action, this);
            if (id < 0)
                return;

            mi = new LSPMenuItem(af->display());
            if (mi == NULL)
                return;
            vMenuItems[off++] = mi;
            LSP_VSTATUS_ASSERT(mi->init());
            LSP_VSTATUS_ASSERT(sMenu.add(mi));
            LSP_VSTATUS_ASSERT(mi->set_text("Copy"));
            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_copy_action, this);
            if (id < 0)
                return;

            mi = new LSPMenuItem(af->display());
            if (mi == NULL)
                return;
            vMenuItems[off++] = mi;
            LSP_VSTATUS_ASSERT(mi->init());
            LSP_VSTATUS_ASSERT(sMenu.add(mi));
            LSP_VSTATUS_ASSERT(mi->set_text("Paste"));
            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_paste_action, this);
            if (id < 0)
                return;

            mi = new LSPMenuItem(af->display());
            if (mi == NULL)
                return;
            vMenuItems[off++] = mi;
            LSP_VSTATUS_ASSERT(mi->init());
            LSP_VSTATUS_ASSERT(sMenu.add(mi));
            LSP_VSTATUS_ASSERT(mi->set_text("Clear"));
            id = mi->slots()->bind(LSPSLOT_SUBMIT, slot_popup_clear_action, this);
            if (id < 0)
                return;

            // Bind menu
            af->set_popup(&sMenu);
        }

        void CtlAudioFile::sync_status()
        {
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(pWidget);
            if (af == NULL)
                return;

            // Analyze status
            size_t status = (pStatus != NULL) ? pStatus->get_value() : STATUS_UNSPECIFIED;

            if (status == STATUS_UNSPECIFIED)
            {
                init_color(C_STATUS_OK, af->hint_font()->color());
                af->set_show_data(false);
                af->set_show_file_name(false);
                af->set_show_hint(true);
                af->set_hint("Click to load");
            }
            else if (status == STATUS_LOADING)
            {
                init_color(C_STATUS_WARN, af->hint_font()->color());
                af->set_show_data(false);
                af->set_show_file_name(false);
                af->set_show_hint(true);
                af->set_hint("Loading...");
            }
            else if (status == STATUS_OK)
            {
                af->set_show_data(true);
                af->set_show_file_name(true);
                af->set_show_hint(false);
            }
            else
            {
                init_color(C_STATUS_ERROR, af->hint_font()->color());
                af->set_show_data(false);
                af->set_show_file_name(false);
                af->set_show_hint(true);
                af->set_hint(get_status(status_t(status)));
            }
        }

        void CtlAudioFile::sync_file()
        {
            if (pFile == NULL)
                return;
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(pWidget);
            if (af == NULL)
                return;

            const char *fname   = pFile->get_buffer<const char>();
            af->set_file_name(fname);
        }

        void CtlAudioFile::sync_mesh()
        {
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(pWidget);
            if (af == NULL)
                return;

            // Get mesh
            mesh_t *mesh        = (pMesh != NULL) ? pMesh->get_buffer<mesh_t>() : NULL;
            if (mesh == NULL)
            {
                af->set_channels(0);
                return;
            }

            // Apply mesh data
            af->set_channels(mesh->nBuffers);
            for (size_t i=0; i<mesh->nBuffers; ++i)
            {
                color_t color = (i & 1) ? C_RIGHT_CHANNEL :
                                ((i + 1) >= mesh->nBuffers) ? C_MIDDLE_CHANNEL : C_LEFT_CHANNEL;
                init_color(color, af->channel_color(i));
                init_color(color, af->channel_line_color(i));
                af->channel_color(i)->alpha(0.5f);
                af->set_channel_data(i, mesh->nItems, mesh->pvData[i]);
            }

            sync_fades();
        }

        void CtlAudioFile::sync_fades()
        {
            mesh_t *mesh        = (pMesh != NULL) ? pMesh->get_buffer<mesh_t>() : NULL;
            if (mesh == NULL)
                return;

            LSPAudioFile *af    = widget_cast<LSPAudioFile>(pWidget);
            if (af == NULL)
                return;

            float length    = (pLength != NULL) ?   pLength->get_value()    : 0.0f;
            float head_cut  = (pHeadCut != NULL) ?  pHeadCut->get_value()   : 0.0f;
            float tail_cut  = (pTailCut != NULL) ?  pTailCut->get_value()   : 0.0f;
            float from_time = head_cut;
            float to_time   = length - tail_cut;

            length          = to_time - from_time;

            size_t n        = af->channels();
            if (n > mesh->nBuffers)
                n               = mesh->nBuffers;

            if (length <= 0)
                length      = (pHeadCut != NULL) ? pHeadCut->metadata()->step :
                              (pTailCut != NULL) ? pTailCut->metadata()->step : 0.1f;

            for (size_t i=0; i<n; ++i)
            {
                init_color(C_YELLOW, af->channel_fade_color(i));

                float fade_in   = (pFadeIn != NULL)     ? pFadeIn->get_value()  : 0.0f;
                float fade_out  = (pFadeOut != NULL)    ? pFadeOut->get_value() : 0.0f;

                af->set_channel_fade_in (i, mesh->nItems * (fade_in  / length));
                af->set_channel_fade_out(i, mesh->nItems * (fade_out / length));
            }
        }

        void CtlAudioFile::set(widget_attribute_t att, const char *value)
        {
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(pWidget);

            switch (att)
            {
                case A_SPACING:
                    if (af != NULL)
                        PARSE_INT(value, af->set_radius(__));
                    break;
                case A_ID:
                    BIND_PORT(pRegistry, pFile, value);
                    break;
                case A_HEAD_ID:
                    BIND_PORT(pRegistry, pHeadCut, value);
                    break;
                case A_TAIL_ID:
                    BIND_PORT(pRegistry, pTailCut, value);
                    break;
                case A_FADEIN_ID:
                    BIND_PORT(pRegistry, pFadeIn, value);
                    break;
                case A_FADEOUT_ID:
                    BIND_PORT(pRegistry, pFadeOut, value);
                    break;
                case A_LENGTH_ID:
                    BIND_PORT(pRegistry, pLength, value);
                    break;
                case A_STATUS_ID:
                    BIND_PORT(pRegistry, pStatus, value);
                    break;
                case A_MESH_ID:
                    BIND_PORT(pRegistry, pMesh, value);
                    break;
                case A_PATH_ID:
                    if (pPathID != NULL)
                        free(pPathID);
                    pPathID = (value != NULL) ? strdup(value) : NULL;
                    break;
                case A_WIDTH:
                    if (af != NULL)
                        PARSE_INT(value, af->constraints()->set_width(__, __));
                    break;
                case A_HEIGHT:
                    if (af != NULL)
                        PARSE_INT(value, af->constraints()->set_height(__, __));
                    break;
                case A_FORMAT:
                    if (af != NULL)
                        parse_file_formats(value, af->filter());
                    break;
                case A_FORMAT_ID:
                    BIND_EXPR(sFormat, value);
                    break;
                case A_BIND:
                    sBind.set_native(value);
                    break;
                default:
                {
                    bool set = sColor.set(att, value);
                    set |= sBgColor.set(att, value);
                    set |= sPadding.set(att, value);

                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlAudioFile::end()
        {
            sync_status();
            sync_file();
            sync_mesh();

            const char *path = (pPathID != NULL) ? pPathID : DEFAULT_PATH_PORT;
            BIND_PORT(pRegistry, pPath, path);

            CtlWidget::end();
        }

        status_t CtlAudioFile::slot_on_activate(LSPWidget *sender, void *ptr, void *data)
        {
            CtlAudioFile *ctl = static_cast<CtlAudioFile *>(ptr);
            if ((ctl == NULL) || (ctl->pPath == NULL))
                return STATUS_BAD_ARGUMENTS;
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(ctl->pWidget);
            if (af == NULL)
                return STATUS_BAD_STATE;

            af->set_path(ctl->pPath->get_buffer<char>());
            return STATUS_OK;
        }

        status_t CtlAudioFile::slot_on_submit(LSPWidget *sender, void *ptr, void *data)
        {
            CtlAudioFile *ctl = static_cast<CtlAudioFile *>(ptr);
            if (ctl == NULL)
                return STATUS_BAD_ARGUMENTS;
            ctl->commit_file();
            return STATUS_OK;
        }

        status_t CtlAudioFile::slot_on_close(LSPWidget *sender, void *ptr, void *data)
        {
            CtlAudioFile *ctl = static_cast<CtlAudioFile *>(ptr);
            if (ctl == NULL)
                return STATUS_BAD_ARGUMENTS;
            ctl->update_path();
            return STATUS_OK;
        }

        status_t CtlAudioFile::slot_popup_cut_action(LSPWidget *sender, void *ptr, void *data)
        {
            LSP_STATUS_ASSERT(slot_popup_copy_action(sender, ptr, data));
            return slot_popup_clear_action(sender, ptr, data);
        }

        status_t CtlAudioFile::slot_popup_copy_action(LSPWidget *sender, void *ptr, void *data)
        {
            CtlAudioFile *ctl = static_cast<CtlAudioFile *>(ptr);
            if (ctl == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(ctl->pWidget);
            if (af == NULL)
                return STATUS_BAD_STATE;

            LSPString str;
            CtlConfigSource src;

            LSP_STATUS_ASSERT(ctl->bind_ports(&src));
            LSP_STATUS_ASSERT(config::serialize(&str, &src, false));

            lsp_trace("Serialized config: \n%s", str.get_native());

            // Copy data to clipboard
            LSPTextClipboard *cb = new LSPTextClipboard();
            if (cb == NULL)
                return STATUS_NO_MEM;

            status_t result = cb->update_text(&str);
            if (result == STATUS_OK)
                af->display()->write_clipboard(CBUF_CLIPBOARD, cb);
            cb->close();

            return STATUS_OK;
        }

        status_t CtlAudioFile::slot_popup_paste_action(LSPWidget *sender, void *ptr, void *data)
        {
            CtlAudioFile *ctl = static_cast<CtlAudioFile *>(ptr);
            if (ctl == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(ctl->pWidget);
            if (af == NULL)
                return STATUS_BAD_STATE;

            return af->display()->fetch_clipboard(CBUF_CLIPBOARD, "UTF8_STRING", clipboard_handler, ctl);
        }

        status_t CtlAudioFile::clipboard_handler(void *arg, status_t s, io::IInputStream *is)
        {
            if (s != STATUS_OK)
                return s;
            else if (is == NULL)
                return STATUS_BAD_STATE;

            CtlAudioFile *ctl = static_cast<CtlAudioFile *>(arg);
            if (ctl == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(ctl->pWidget);
            if (af == NULL)
                return STATUS_BAD_STATE;

            LSPString str;
            CtlConfigHandler dst;

            LSP_STATUS_ASSERT(ctl->bind_ports(&dst));
            LSP_STATUS_ASSERT(config::load(is, &dst));

            return STATUS_OK;
        }

        status_t CtlAudioFile::bind_ports(CtlPortHandler *h)
        {
            LSP_STATUS_ASSERT(h->add_port("file", pFile));
            LSP_STATUS_ASSERT(h->add_port("head_cut", pHeadCut));
            LSP_STATUS_ASSERT(h->add_port("tail_cut", pTailCut));
            LSP_STATUS_ASSERT(h->add_port("fade_in", pFadeIn));
            LSP_STATUS_ASSERT(h->add_port("fade_out", pFadeOut));

            if (sBind.length() <= 0)
                return STATUS_OK;

            LSPString tmp, value;
            ssize_t first = 0;

            while (first >= 0)
            {
                // Fetch next pair
                ssize_t next = sBind.index_of(first, ',');
                if (next > 0)
                {
                    if (!tmp.set(&sBind, first, next))
                        return STATUS_NO_MEM;
                }
                else if (!tmp.set(&sBind, first))
                    return STATUS_NO_MEM;
                lsp_trace("pair = %s", tmp.get_native());

                first = (next >= 0) ? next + 1 : -1;

                // Split pair
                tmp.trim();
                next = tmp.index_of('=');
                if (next < 0)
                    LSP_BOOL_ASSERT(value.set(&tmp), STATUS_NO_MEM)
                else
                {
                    LSP_BOOL_ASSERT(value.set(&tmp, next + 1), STATUS_NO_MEM);
                    tmp.truncate(next);
                }
                lsp_trace("alias = %s, port = %s", tmp.get_native(), value.get_native());
                tmp.trim();
                value.trim();
                lsp_trace("trimmed alias = %s, port = %s", tmp.get_native(), value.get_native());

                // Now add port
                CtlPort *p = pRegistry->port(value.get_native());
                if (p == NULL)
                    continue;
                LSP_STATUS_ASSERT(h->add_port(&tmp, p));
            }

            return STATUS_OK;
        }

        status_t CtlAudioFile::slot_popup_clear_action(LSPWidget *sender, void *ptr, void *data)
        {
            CtlAudioFile *ctl = static_cast<CtlAudioFile *>(ptr);
            if (ctl == NULL)
                return STATUS_BAD_ARGUMENTS;
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(ctl->pWidget);
            if (af == NULL)
                return STATUS_BAD_STATE;

            af->set_file_name("");
            ctl->commit_file();
            return STATUS_OK;
        }

        void CtlAudioFile::commit_file()
        {
            if (pFile == NULL)
                return;

            LSPAudioFile *af    = widget_cast<LSPAudioFile>(pWidget);
            const char *fname   = (af != NULL) ? af->file_name() : NULL;

            // Write new path request
            pFile->write(fname, (fname != NULL) ? strlen(fname) : 0);
            pFile->notify_all();
        }

        void CtlAudioFile::update_path()
        {
            LSPAudioFile *af    = widget_cast<LSPAudioFile>(pWidget);
            if ((af == NULL) || (pPath == NULL))
                return;

            LSPString path;
            if (af->get_path(&path) != STATUS_OK)
                return;
            if (path.length() <= 0)
                return;

            pPath->write(path.get_native(), path.length());
            pPath->notify_all();
        }

        void CtlAudioFile::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if (port == pStatus)
                sync_status();
            if (port == pFile)
                sync_file();
            if (port == pMesh)
                sync_mesh();
            if ((port == pLength) ||
                (port == pHeadCut) ||
                (port == pTailCut) ||
                (port == pFadeIn) ||
                (port == pFadeOut))
                sync_fades();

            LSPAudioFile *af    = widget_cast<LSPAudioFile>(pWidget);
            if (af == NULL)
                return;

            if (sFormat.valid())
                af->filter()->set_default(sFormat.evaluate());
        }
    
    } /* namespace ctl */
} /* namespace lsp */
