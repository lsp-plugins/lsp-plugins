/*
 * CtlFader.cpp
 *
 *  Created on: 19 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlFader::metadata = { "CtlFader", &CtlWidget::metadata };

        CtlFader::CtlFader(CtlRegistry *src, LSPFader *widget):
            CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pPort           = NULL;
            bLog            = false;
        }
        
        CtlFader::~CtlFader()
        {
        }

        void CtlFader::init()
        {
            CtlWidget::init();

            LSPFader *fader = widget_cast<LSPFader>(pWidget);
            if (fader == NULL)
                return;

            // Bind slots
            fader->slots()->bind(LSPSLOT_CHANGE, slot_change, this);
        }

        void CtlFader::set(widget_attribute_t att, const char *value)
        {
            LSPFader *fader    = widget_cast<LSPFader>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_SIZE:
                    if (fader != NULL)
                        PARSE_INT(value, fader->set_min_size(__));
                    break;
                case A_ANGLE:
                    if (fader != NULL)
                        PARSE_INT(value, fader->set_angle(__));
                    break;
                case A_VALUE:
                    if (fader != NULL)
                        PARSE_FLOAT(value, fader->set_value(__));
                    break;
                case A_DEFAULT:
                    if (fader != NULL)
                        PARSE_FLOAT(value, fader->set_default_value(__));
                    break;
                case A_MIN:
                    if (fader != NULL)
                        PARSE_FLOAT(value, fader->set_min_value(__));
                    break;
                case A_MAX:
                    if (fader != NULL)
                        PARSE_FLOAT(value, fader->set_max_value(__));
                    break;
                case A_LOGARITHMIC:
                    PARSE_BOOL(value, bLog = __);
                    break;
                case A_STEP:
                    if (fader != NULL)
                        PARSE_FLOAT(value, fader->set_step(__));
                    break;
                case A_TINY_STEP:
                    if (fader != NULL)
                        PARSE_FLOAT(value, fader->set_tiny_step(__));
                    break;

                default:
                {
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlFader::commit_value(float value)
        {
            LSPFader *fader = widget_cast<LSPFader>(pWidget);
            if (fader == NULL)
                return;
            const port_t *p = pPort->metadata();
            if (p == NULL)
                return;

            if (is_decibel_unit(p->unit)) // Decibels
            {
                double base = (p->unit == U_GAIN_AMP) ? 20.0 / M_LN10 : 10.0 / M_LN10;

                if (value < GAIN_AMP_M_120_DB)
                    value           = GAIN_AMP_M_120_DB;

                fader->set_value(base * log(value));
            }
            else if (is_discrete_unit(p->unit)) // Integer type
            {
                fader->set_value(truncf(value));
            }
            else if (bLog)
            {
                if (value < GAIN_AMP_M_120_DB)
                    value           = GAIN_AMP_M_120_DB;
                fader->set_value(log(value));
            }
            else
            {
                fader->set_value(value);
            }
        }

        void CtlFader::submit_value()
        {
            if (pPort == NULL)
                return;

            LSPFader *fader = widget_cast<LSPFader>(pWidget);
            if (fader == NULL)
                return;
            float value     = fader->value();

            const port_t *p = pPort->metadata();
            if (p == NULL)
            {
                pPort->set_value(value);
                pPort->notify_all();
                return;
            }

            if (is_decibel_unit(p->unit)) // Decibels
            {
                double base     = (p->unit == U_GAIN_AMP) ? M_LN10 * 0.05 : M_LN10 * 0.1;
                value           = exp(value * base);
                float min       = (p->flags & F_LOWER) ? p->min : 0.0f;
                if ((min <= 0.0f) && (value < base * log(GAIN_AMP_M_80_DB)))
                    value           = 0.0f;
            }
            else if (is_discrete_unit(p->unit)) // Integer type
            {
                 value          = truncf(value);
            }
            else if (bLog)  // Float and other values, logarithmic
            {
                value           = exp(value);
                float min       = (p->flags & F_LOWER) ? p->min : 0.0f;
                if ((min <= 0.0f) && (value < log(GAIN_AMP_M_80_DB)))
                    value           = 0.0f;
            }

            pPort->set_value(value);
            pPort->notify_all();
        }

        void CtlFader::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if (port == pPort)
                commit_value(pPort->get_value());
        }

        status_t CtlFader::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlFader *_this     = static_cast<CtlFader *>(ptr);
            if (_this != NULL)
                _this->submit_value();
            return STATUS_OK;
        }

        void CtlFader::end()
        {
            // Ensure that widget is set
            LSPFader *fader = widget_cast<LSPFader>(pWidget);
            if (fader == NULL)
                return;

            // Ensure that port is set
            const port_t *p = (pPort != NULL) ? pPort->metadata() : NULL;
            if (p == NULL)
                return;

            if (is_decibel_unit(p->unit)) // Decibels
            {
                double base = (p->unit == U_GAIN_AMP) ? 20.0 / M_LN10 : 10.0 / M_LN10;

                float min       = (p->flags & F_LOWER) ? p->min : 0.0f;
                float max       = (p->flags & F_UPPER) ? p->max : GAIN_AMP_P_12_DB;

                double step     = base * log((p->flags & F_STEP) ? p->step + 1.0f : 1.01f) * 0.1f;
                double db_min   = (fabs(min) < GAIN_AMP_M_80_DB) ? base * log(GAIN_AMP_M_80_DB) - step : base * log(min);
                double db_max   = (fabs(max) < GAIN_AMP_M_80_DB) ? base * log(GAIN_AMP_M_80_DB) - step : base * log(max);

                fader->set_min_value(db_min);
                fader->set_max_value(db_max);
                fader->set_step(step * 10.0f);
                fader->set_tiny_step(step);
                fader->set_value(base * log(p->start));
                fader->set_default_value(fader->value());
            }
            else if (is_discrete_unit(p->unit)) // Integer type
            {
                fader->set_min_value((p->flags & F_LOWER) ? p->min : 0.0f);
                if (p->unit == U_ENUM)
                    fader->set_max_value(fader->min_value() + list_size(p->items) - 1.0f);
                else
                    fader->set_max_value((p->flags & F_UPPER) ? p->max : 1.0f);

                // Get step, truncate to integer amd process value
                ssize_t step    = (p->flags & F_STEP) ? p->step : 1;
                step            = (step == 0) ? 1 : step;

                fader->set_step(step);
                fader->set_tiny_step(step);
                fader->set_value(p->start);
                fader->set_default_value(p->start);
            }
            else if (bLog)  // Float and other values, logarithmic
            {
                float min       = (p->flags & F_LOWER) ? p->min : 0.0f;
                float max       = (p->flags & F_UPPER) ? p->max : GAIN_AMP_P_12_DB;

                double step     = log((p->flags & F_STEP) ? p->step + 1.0f : 1.01f);
                double l_min    = (fabs(min) < GAIN_AMP_M_80_DB) ? log(GAIN_AMP_M_80_DB) - step : log(min);
                double l_max    = (fabs(max) < GAIN_AMP_M_80_DB) ? log(GAIN_AMP_M_80_DB) - step : log(max);

                fader->set_min_value(l_min);
                fader->set_max_value(l_max);
                fader->set_step(step * 10.0f);
                fader->set_tiny_step(step);
                fader->set_value(log(p->start));
                fader->set_default_value(fader->value());
            }
            else // Float and other values, non-logarithmic
            {
                fader->set_min_value((p->flags & F_LOWER) ? p->min : 0.0f);
                fader->set_max_value((p->flags & F_UPPER) ? p->max : 1.0f);
                fader->set_tiny_step((p->flags & F_STEP) ? p->step : (fader->max_value() - fader->min_value()) * 0.01f);
                fader->set_step(fader->tiny_step() * 10.0f);
                fader->set_value(p->start);
                fader->set_default_value(p->start);
            }
        }
    
    } /* namespace ctl */
} /* namespace lsp */
