/*
 * CtlAudioSample.cpp
 *
 *  Created on: 9 июл. 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <core/port_data.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlAudioSample::metadata = { "CtlAudioSample", &CtlWidget::metadata };
        
        CtlAudioSample::CtlAudioSample(CtlRegistry *src, LSPAudioSample *as):
            CtlWidget(src, as)
        {
            pClass          = &metadata;
            pMesh           = NULL;
            pStatus         = NULL;
            pLength         = NULL;
            pHeadCut        = NULL;
            pTailCut        = NULL;
            pFadeIn         = NULL;
            pFadeOut        = NULL;
            pCurrLen        = NULL;
            pMaxLen         = NULL;
        }
        
        CtlAudioSample::~CtlAudioSample()
        {
        }

        void CtlAudioSample::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPAudioSample *as      = widget_cast<LSPAudioSample>(pWidget);
            if (as == NULL)
                return;

            // Initialize color controllers
            sColor.init_basic(pRegistry, as, as->color(), A_COLOR);
            sPadding.init(as->padding());
        }

        void CtlAudioSample::set(widget_attribute_t att, const char *value)
        {
            LSPAudioSample *as      = widget_cast<LSPAudioSample>(pWidget);

            switch (att)
            {
                case A_SPACING:
                    if (as != NULL)
                        PARSE_INT(value, as->set_radius(__));
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
                case A_DURATION_ID:
                    BIND_PORT(pRegistry, pCurrLen, value);
                    break;
                case A_MAX_DURATION_ID:
                    BIND_PORT(pRegistry, pMaxLen, value);
                    break;
                case A_WIDTH:
                    if (as != NULL)
                        PARSE_INT(value, as->constraints()->set_width(__, __));
                    break;
                case A_HEIGHT:
                    if (as != NULL)
                        PARSE_INT(value, as->constraints()->set_height(__, __));
                    break;
                default:
                {
                    sColor.set(att, value);
                    sPadding.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlAudioSample::end()
        {
            sync_status();
            sync_mesh();

            CtlWidget::end();
        }

        void CtlAudioSample::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if ((port == pStatus) ||
                (port == pCurrLen) ||
                (port == pMaxLen))
                sync_status();
            if (port == pMesh)
                sync_mesh();
            if ((port == pLength) ||
                (port == pHeadCut) ||
                (port == pTailCut) ||
                (port == pFadeIn) ||
                (port == pFadeOut))
                sync_fades();
        }

        void CtlAudioSample::sync_status()
        {
            LSPAudioSample *as      = widget_cast<LSPAudioSample>(pWidget);
            if (as == NULL)
                return;

            // Analyze status
            size_t status = (pStatus != NULL) ? pStatus->get_value() : STATUS_UNSPECIFIED;

            if ((status == STATUS_UNSPECIFIED) || (status == STATUS_NO_DATA))
            {
                init_color(C_STATUS_OK, as->hint_font()->color());
                as->set_show_data(false);
                as->set_show_hint(true);
                as->set_hint("No data");
            }
            else if ((status == STATUS_LOADING) || (status == STATUS_IN_PROCESS))
            {
                init_color(C_STATUS_WARN, as->hint_font()->color());
                as->set_show_data(false);
                as->set_show_hint(true);
                if (status == STATUS_LOADING)
                    as->set_hint("Loading...");
                else
                    as->set_hint("In process...");
            }
            else if (status == STATUS_OK)
            {
                as->set_show_data(true);
                as->set_show_hint(false);
            }
            else
            {
                init_color(C_STATUS_ERROR, as->hint_font()->color());
                as->set_show_data(false);
                as->set_show_hint(true);
                as->set_hint(get_status(status_t(status)));
            }

            if (pCurrLen != NULL)
            {
                as->set_show_curr_length(true);
                as->set_curr_length(pCurrLen->get_value());
            }
            if (pMaxLen != NULL)
            {
                as->set_show_max_length(true);
                as->set_max_length(pMaxLen->get_value());
            }
        }

        void CtlAudioSample::sync_mesh()
        {
            LSPAudioSample *as      = widget_cast<LSPAudioSample>(pWidget);
            if (as == NULL)
                return;

            // Get mesh
            mesh_t *mesh        = (pMesh != NULL) ? pMesh->get_buffer<mesh_t>() : NULL;
            if (mesh == NULL)
            {
                as->set_channels(0);
                return;
            }

            // Apply mesh data
            as->set_channels(mesh->nBuffers);
            for (size_t i=0; i<mesh->nBuffers; ++i)
            {
                color_t color = (i & 1) ? C_RIGHT_CHANNEL :
                                ((i + 1) >= mesh->nBuffers) ? C_MIDDLE_CHANNEL : C_LEFT_CHANNEL;
                init_color(color, as->channel_color(i));
                init_color(color, as->channel_line_color(i));
                as->channel_color(i)->alpha(0.5f);
                as->set_channel_data(i, mesh->nItems, mesh->pvData[i]);
            }

            sync_fades();
        }

        void CtlAudioSample::sync_fades()
        {
            mesh_t *mesh        = (pMesh != NULL) ? pMesh->get_buffer<mesh_t>() : NULL;
            if (mesh == NULL)
                return;

            LSPAudioSample *as      = widget_cast<LSPAudioSample>(pWidget);
            if (as == NULL)
                return;

            float length    = (pLength != NULL) ?   pLength->get_value()    : 0.0f;
            float head_cut  = (pHeadCut != NULL) ?  pHeadCut->get_value()   : 0.0f;
            float tail_cut  = (pTailCut != NULL) ?  pTailCut->get_value()   : 0.0f;
            float from_time = head_cut;
            float to_time   = length - tail_cut;

            length          = to_time - from_time;

            size_t n        = as->channels();
            if (n > mesh->nBuffers)
                n               = mesh->nBuffers;

            if (length <= 0)
                length      = (pHeadCut != NULL) ? pHeadCut->metadata()->step :
                              (pTailCut != NULL) ? pTailCut->metadata()->step : 0.1f;

            for (size_t i=0; i<n; ++i)
            {
                init_color(C_YELLOW, as->channel_fade_color(i));

                float fade_in   = (pFadeIn != NULL)     ? pFadeIn->get_value()  : 0.0f;
                float fade_out  = (pFadeOut != NULL)    ? pFadeOut->get_value() : 0.0f;

                as->set_channel_fade_in (i, mesh->nItems * (fade_in  / length));
                as->set_channel_fade_out(i, mesh->nItems * (fade_out / length));
            }
        }
    
    } /* namespace ctl */
} /* namespace lsp */
