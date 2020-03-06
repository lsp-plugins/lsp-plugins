/*
 * CtlTempoTap.cpp
 *
 *  Created on: 6 июн. 2018 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <time.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlTempoTap::metadata = { "CtlTempoTap", &CtlWidget::metadata };
        
        CtlTempoTap::CtlTempoTap(CtlRegistry *src, LSPButton *widget):
            CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pPort           = NULL;
            nThresh         = 1000;
            nLastTap        = 0;
            fTempo          = 0.0f;
        }
        
        CtlTempoTap::~CtlTempoTap()
        {
        }

        status_t CtlTempoTap::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlTempoTap *_this    = static_cast<CtlTempoTap *>(ptr);
            if (_this != NULL)
                _this->submit_value();
            return STATUS_OK;
        }

        uint64_t CtlTempoTap::time()
        {
            struct timespec ts;

            while (true)
            {
                int code = clock_gettime(CLOCK_REALTIME, &ts);
                if (code == 0)
                    break;
                else if (code != EINTR)
                    return 0;
            }

            return (ts.tv_sec * 1000) + (ts.tv_nsec / 1000000);
        }

        void CtlTempoTap::submit_value()
        {
            LSPButton *btn = widget_cast<LSPButton>(pWidget);
            if ((btn == NULL) || (!btn->is_down()))
                return;
            lsp_trace("button tap");

            // Estimate delay between two sequential taps
            uint64_t t  = time();
            int64_t d   = t - nLastTap;
            nLastTap    = t;

            if ((d >= nThresh) || (d <= 0))
            {
                fTempo   = 0.0f;
                return;
            }

            // Now calculate tempo
            float tempo = 60000.0f / d;
            fTempo = (fTempo <= 0) ? tempo : fTempo * 0.5f + tempo * 0.5f;
            lsp_trace("tempo = %.3f", fTempo);

            // Update port value
            if (pPort != NULL)
            {
                pPort->set_value(fTempo);
                pPort->notify_all();
            }
        }

        void CtlTempoTap::init()
        {
            CtlWidget::init();

            LSPButton *btn = widget_cast<LSPButton>(pWidget);
            if (btn == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, btn, btn->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sTextColor.init_basic(pRegistry, btn, btn->font()->color(), A_TEXT_COLOR);

            // Bind slots
            btn->slots()->bind(LSPSLOT_CHANGE, slot_change, this);
            btn->set_trigger();
        }

        void CtlTempoTap::set(const char *name, const char *value)
        {
            LSPButton *btn = widget_cast<LSPButton>(pWidget);
            if (btn != NULL)
                set_lc_attr(A_TEXT, btn->title(), name, value);

            CtlWidget::set(name, value);
        }

        void CtlTempoTap::set(widget_attribute_t att, const char *value)
        {
            LSPButton *btn = widget_cast<LSPButton>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_SIZE:
                    if (btn != NULL)
                        PARSE_INT(value, btn->set_min_size(__, __));
                    break;
                case A_WIDTH:
                    if (btn != NULL)
                        PARSE_INT(value, btn->set_min_width(__));
                    break;
                case A_HEIGHT:
                    if (btn != NULL)
                        PARSE_INT(value, btn->set_min_height(__));
                    break;
                case A_LED:
                    if (btn != NULL)
                        PARSE_BOOL(value, btn->set_led(__));
                    break;
                default:
                {
                    sColor.set(att, value);
                    sTextColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlTempoTap::end()
        {
            if (pPort != NULL)
            {
                const port_t *meta = pPort->metadata();
                if ((meta != NULL) && (meta->flags & F_LOWER))
                    nThresh     = (121 * 1000) / (meta->min);
            }

            CtlWidget::end();
        }
    
    } /* namespace ctl */
} /* namespace lsp */
