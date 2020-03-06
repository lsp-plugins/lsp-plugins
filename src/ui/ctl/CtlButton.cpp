/*
 * CtlButton.cpp
 *
 *  Created on: 28 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlButton::metadata = { "CtlButton", &CtlWidget::metadata };
        
        CtlButton::CtlButton(CtlRegistry *src, LSPButton *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            fValue          = 0;
            pPort           = NULL;
        }
        
        CtlButton::~CtlButton()
        {
        }

        status_t CtlButton::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlButton *_this    = static_cast<CtlButton *>(ptr);
            if (_this != NULL)
                _this->submit_value();
            return STATUS_OK;
        }

        float CtlButton::next_value(bool down)
        {
            const port_t *mdata = (pPort != NULL) ? pPort->metadata() : NULL;
            if (mdata == NULL)
                return (fValue >= 0.5f) ? 0.0f : 1.0f;

            // Analyze event
            if (down)
            {
                if (mdata->unit == U_ENUM)
                    return fValue;
//                if (!IS_TRIGGER_PORT(mdata))
//                    return fValue;
            }

            // Get minimum and maximum
            float min   = (mdata->flags & F_LOWER) ? mdata->min : 0.0;
            float max   = (mdata->flags & F_UPPER) ? mdata->max : min + 1.0f;
            float step  = (mdata->flags & F_STEP) ? mdata->step : 1.0;
            if ((mdata->unit == U_ENUM) && (mdata->items != NULL))
                max     = mdata->min + list_size(mdata->items) - 1.0f;

            float value = fValue + step;
            if (value > max)
                value = min;
            else if (value < min)
                value = max;

            return value;
        }

        void CtlButton::submit_value()
        {
            LSPButton *btn = widget_cast<LSPButton>(pWidget);
            if (btn == NULL)
                return;
            lsp_trace("button is down=%s", (btn->is_down()) ? "true" : "false");

            float value     = next_value(btn->is_down());
            if (value == fValue)
                return;

            if (pPort != NULL)
            {
                lsp_trace("Setting %s = %f", pPort->id(), value);
                pPort->set_value(value);
                pPort->notify_all();
            }
        }

        void CtlButton::commit_value(float value)
        {
            lsp_trace("commit value=%f", value);
            LSPButton *btn = widget_cast<LSPButton>(pWidget);
            if (btn == NULL)
                return;

            const port_t *mdata = (pPort != NULL) ? pPort->metadata() : NULL;

            if (mdata != NULL)
            {
                fValue      = value;

                float min   = (mdata->flags & F_LOWER) ? mdata->min : 0.0;
                float max   = (mdata->flags & F_UPPER) ? mdata->max : min + 1.0f;

                if (mdata->unit == U_ENUM)
                    btn->set_down(false);
                else if (!IS_TRIGGER_PORT(mdata))
                    btn->set_down(fabs(value - max) < fabs(value - min));
//                else if (fValue == max)
//                    btn->set_down(true);
//                else
//                    btn->set_down(false);
            }
            else
            {
                fValue      = (value >= 0.5f) ? 1.0f : 0.0f;
                btn->set_down(fValue >= 0.5f);
            }
        }

        void CtlButton::init()
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
        }

        void CtlButton::set(const char *name, const char *value)
        {
            LSPButton *btn = widget_cast<LSPButton>(pWidget);
            if (btn != NULL)
                set_lc_attr(A_TEXT, btn->title(), name, value);

            CtlWidget::set(name, value);
        }

        void CtlButton::set(widget_attribute_t att, const char *value)
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
                case A_VALUE:
                    PARSE_FLOAT(value, commit_value(__));
                    break;
                case A_LED:
                    if (btn != NULL)
                        PARSE_BOOL(value, btn->set_led(__));
                    break;
                case A_EDITABLE:
                    if (btn != NULL)
                        PARSE_BOOL(value, btn->set_editable(__));
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

        void CtlButton::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if (port == pPort)
                commit_value(pPort->get_value());
        }

        void CtlButton::end()
        {
            if (pWidget != NULL)
            {
                LSPButton *btn = widget_cast<LSPButton>(pWidget);
                if (btn == NULL)
                    return;

                if (pPort != NULL)
                {
                    const port_t *mdata = pPort->metadata();
                    if (mdata != NULL)
                    {
                        if (IS_TRIGGER_PORT(mdata))
                            btn->set_trigger();
                        else if (mdata->unit != U_ENUM)
                            btn->set_toggle();
                    }

                    commit_value(pPort->get_value());
                }
                else
                    commit_value(fValue);
            }

            CtlWidget::end();
        }
    
    } /* namespace ctl */
} /* namespace lsp */
