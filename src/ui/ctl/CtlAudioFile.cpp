/*
 * CtlAudioFile.cpp
 *
 *  Created on: 28 окт. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <ui/common.h>

namespace lsp
{
    namespace ctl
    {
        
        CtlAudioFile::CtlAudioFile(CtlRegistry *src, LSPAudioFile *af): CtlWidget(src, af)
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
        }
        
        CtlAudioFile::~CtlAudioFile()
        {
            if (pPathID != NULL)
            {
                free(pPathID);
                pPathID = NULL;
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

            const char *path = af->get_path();
            if (path != NULL)
            {
                pPath->write(path, strlen(path));
                pPath->notify_all();
            }
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
        }
    
    } /* namespace ctl */
} /* namespace lsp */
