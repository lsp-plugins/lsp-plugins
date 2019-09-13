/*
 * CtlProgressBar.cpp
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlProgressBar::metadata = { "CtlProgressBar", &CtlWidget::metadata };
        
        CtlProgressBar::CtlProgressBar(CtlRegistry *src, LSPProgressBar *widget):
            CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pPort           = NULL;
            nXFlags         = 0;
            sFormat.set_native("%.2f%%");
        }
        
        CtlProgressBar::~CtlProgressBar()
        {
        }

        void CtlProgressBar::init()
        {
            CtlWidget::init();

            sMin.init(pRegistry, this);
            sMax.init(pRegistry, this);
            sValue.init(pRegistry, this);

            // Initialize color controllers
            LSPProgressBar *bar = widget_cast<LSPProgressBar>(pWidget);
            if (bar != NULL)
            {
                sColor.init_hsl(pRegistry, bar, bar->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
                sScaleColor.init_hsl(pRegistry, bar, bar->sel_color(), A_SCALE_COLOR, A_SCALE_HUE_ID, A_SCALE_SAT_ID, A_SCALE_LIGHT_ID);
                sScaleColor.map_static_hsl(A_SCALE_HUE, -1, -1);
            }
        }

        void CtlProgressBar::set(widget_attribute_t att, const char *value)
        {
            LSPProgressBar *bar = widget_cast<LSPProgressBar>(pWidget);

            switch (att)
            {
                case A_WIDTH:
                    if (bar != NULL)
                        PARSE_INT(value, bar->set_min_width(__));
                    break;
                case A_HEIGHT:
                    if (bar != NULL)
                        PARSE_INT(value, bar->set_min_height(__));
                    break;
                case A_MIN:
                    BIND_EXPR(sMin, value);
                    nXFlags |= XF_MIN;
                    break;
                case A_MAX:
                    BIND_EXPR(sMax, value);
                    nXFlags |= XF_MAX;
                    break;
                case A_VALUE:
                    BIND_EXPR(sValue, value);
                    nXFlags |= XF_VALUE;
                    break;
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_FORMAT:
                    sFormat.set_utf8(value);
                    break;

                default:
                    sColor.set(att, value);
                    sScaleColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
            }
        }

        void CtlProgressBar::sync_state(CtlPort *port, bool force)
        {
            LSPProgressBar *bar = widget_cast<LSPProgressBar>(pWidget);
            if (bar == NULL)
                return;

            if ((nXFlags & XF_MIN) && (sMin.valid()))
            {
                if (bar->set_min_value(sMin.evaluate()))
                    force   = true;
            }
            if ((nXFlags & XF_MAX) && (sMax.valid()))
            {
                if (bar->set_max_value(sMax.evaluate()))
                    force   = true;
            }

            if ((nXFlags & XF_VALUE) && (sValue.valid()))
            {
                if (bar->set_value(sValue.evaluate()))
                    force   = true;
            }
            else if ((port == pPort) && (pPort != NULL))
            {
                const port_t *meta = pPort->metadata();
                if ((!(nXFlags & XF_MIN)) && (meta->flags & F_LOWER))
                    bar->set_min_value(meta->min);
                if ((!(nXFlags & XF_MAX)) && (meta->flags & F_UPPER))
                    bar->set_max_value(meta->max);

                if (bar->set_value(pPort->get_value()))
                    force   = true;
            }

            // Need to update text?
            if (force)
            {
                LSPString text;
                if (text.fmt_utf8(sFormat.get_utf8(), bar->get_value()))
                    bar->set_text(&text);
            }
        }

        void CtlProgressBar::notify(CtlPort *port)
        {
            sync_state(port, false);
            CtlWidget::notify(port);
        }

        void CtlProgressBar::end()
        {
            if (pPort != NULL)
                sync_metadata(pPort);

            sync_state(pPort, true);

            CtlWidget::end();
        }

        void CtlProgressBar::sync_metadata(CtlPort *port)
        {
            LSPProgressBar *bar = widget_cast<LSPProgressBar>(pWidget);

            if ((bar != NULL) && (port != NULL) && (pPort == port))
            {
                const port_t *meta = pPort->metadata();
                if (meta != NULL)
                {
                    if ((meta->flags & F_LOWER) && (!((nXFlags & XF_MIN) && (sMin.valid()))))
                        bar->set_min_value(meta->min);
                    if ((meta->flags & F_UPPER) && (!((nXFlags & XF_MAX) && (sMax.valid()))))
                        bar->set_min_value(meta->min);
                }
            }

            CtlWidget::sync_metadata(port);
        }
    
    } /* namespace ctl */
} /* namespace lsp */
