/*
 * CtlDot.cpp
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlDot::metadata = { "CtlDot", &CtlWidget::metadata };

        CtlDot::CtlDot(CtlRegistry *src, LSPDot *dot): CtlWidget(src, dot)
        {
            pClass          = &metadata;
            pTop            = NULL;
            pLeft           = NULL;
            pScroll         = NULL;
            nFlags          = 0;
            fTop            = 0.0f;
            fLeft           = 0.0f;
            bEditable       = false;
        }

        CtlDot::~CtlDot()
        {
        }

        void CtlDot::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPDot *dot   = static_cast<LSPDot *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, dot, dot->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);

            // Bind slots
            dot->slots()->bind(LSPSLOT_CHANGE, slot_change, this);
        }

        status_t CtlDot::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlDot *_this       = static_cast<CtlDot *>(ptr);
            if (_this != NULL)
                _this->submit_values();
            return STATUS_OK;
        }

        void CtlDot::submit_values()
        {
            LSPDot *dot   = widget_cast<LSPDot>(pWidget);
            if (dot == NULL)
                return;

            if (dot->x_editable())
                submit_value(pLeft, dot->x_value());
            if (dot->y_editable())
                submit_value(pTop, dot->y_value());
            if (dot->z_editable())
            {
                const port_t *p = pScroll->metadata();
                float value = dot->z_value();

                if ((p != NULL) && (is_log_rule(p)))
                {
                    float min = fabs(p->min);

                    min = (min < GAIN_AMP_M_120_DB) ? logf(GAIN_AMP_M_80_DB) : logf(min);
                    value = (value < min) ? p->min : expf(value);

                }

                submit_value(pScroll, value);
            }
        }

        void CtlDot::submit_value(CtlPort *port, float value)
        {
            if (port == NULL)
                return;
            if (value == port->get_value())
                return;

            port->set_value(value);
            port->notify_all();
        }

        void CtlDot::set(widget_attribute_t att, const char *value)
        {
            LSPDot *dot = widget_cast<LSPDot>(pWidget);

            switch (att)
            {
                case A_VPOS:
                    PARSE_FLOAT(value, fTop = __);
                    break;
                case A_HPOS:
                    PARSE_FLOAT(value, fLeft = __);
                    break;
                case A_SIZE:
                    if (dot != NULL)
                        PARSE_INT(value, dot->set_size(__));
                    break;
                case A_BORDER:
                    if (dot != NULL)
                        PARSE_INT(value, dot->set_border(__));
                    break;
                case A_PADDING:
                    if (dot != NULL)
                        PARSE_INT(value, dot->set_padding(__));
                    break;

                case A_BASIS:
                    if (dot != NULL)
                        PARSE_INT(value, dot->set_basis_id(__));
                    break;
                case A_PARALLEL:
                    if (dot != NULL)
                        PARSE_INT(value, dot->set_parallel_id(__));
                    break;
                case A_CENTER:
                    if (dot != NULL)
                        PARSE_INT(value, dot->set_center_id(__));
                    break;

                case A_SCROLL_ID:
                    BIND_PORT(pRegistry, pScroll, value);
                    break;
                case A_VPOS_ID:
                    BIND_PORT(pRegistry, pTop, value);
                    break;
                case A_HPOS_ID:
                    BIND_PORT(pRegistry, pLeft, value);
                    break;

                case A_EDITABLE:
                    PARSE_BOOL(value, bEditable = __);
                    break;
                case A_LED:
                    if (dot != NULL)
                        PARSE_BOOL(value, dot->set_highlight(__));
                    break;

                default:
                {
                    bool set = sColor.set(att, value);
                    if (!set)
                        CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlDot::end()
        {
            CtlWidget::end();

            // Get widget
            LSPDot *dot   = widget_cast<LSPDot>(pWidget);
            if (dot == NULL)
                return;

            // Tune parameters
            const port_t *p = NULL;

            if (pLeft != NULL)
            {
                dot->set_x_editable(bEditable);
                fLeft = pLeft->get_value();

                p = pLeft->metadata();
                if (p != NULL)
                {
                    if (p->flags & F_LOWER)
                        dot->set_x_minimum(p->min);
                    if (p->flags & F_UPPER)
                        dot->set_x_maximum(p->max);
                    if (p->flags & F_STEP)
                        dot->set_x_step(p->step);
                }
            }
            else
            {
                dot->set_x_minimum(fLeft);
                dot->set_x_maximum(fLeft);
                dot->set_x_value(fLeft);
            }

            if (pTop != NULL)
            {
                dot->set_y_editable(bEditable);
                fTop = pTop->get_value();

                p = pTop->metadata();
                if (p != NULL)
                {
                    if (p->flags & F_LOWER)
                        dot->set_y_minimum(p->min);
                    if (p->flags & F_UPPER)
                        dot->set_y_maximum(p->max);
                    if (p->flags & F_STEP)
                        dot->set_y_step(p->step);
                }
            }
            else
            {
                dot->set_y_minimum(fTop);
                dot->set_y_maximum(fTop);
                dot->set_y_value(fTop);
            }

            if (pScroll != NULL)
            {
                dot->set_z_editable(bEditable);
                p = pScroll->metadata();

                if (p != NULL)
                {
                    float min, max;
                    if (is_log_rule(p))
                    {
                        min = fabs(p->min);
                        max = fabs(p->max);

                        min = (min < GAIN_AMP_M_80_DB) ? logf(GAIN_AMP_M_80_DB) - p->step : logf(min);
                        max = (max < GAIN_AMP_M_80_DB) ? logf(GAIN_AMP_M_80_DB) - p->step : logf(max);
                    }
                    else
                    {
                        min = p->min;
                        max = p->max;
                    }

                    if (p->flags & F_LOWER)
                        dot->set_z_minimum(min);
                    if (p->flags & F_UPPER)
                        dot->set_z_maximum(max);
                    if (p->flags & F_STEP)
                    {
                        dot->set_z_tiny_step(p->step);
                        dot->set_z_step(p->step * 10.0f);
                        dot->set_z_big_step(p->step * 100.0f);
                    }
                }
            }

            // Update mouse pointer for dot
            if (dot->x_editable() && dot->y_editable())
                dot->set_cursor(MP_DRAG);
            else if (dot->x_editable())
                dot->set_cursor(MP_HSIZE);
            else if (dot->y_editable())
                dot->set_cursor(MP_VSIZE);
            else
                dot->set_cursor(MP_DEFAULT);

            notify(pLeft);
            notify(pTop);
            notify(pScroll);
        }

        void CtlDot::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            LSPDot *dot = widget_cast<LSPDot>(pWidget);
            if (dot == NULL)
                return;

            if ((pLeft != NULL) && (port == pLeft))
            {
                fLeft   = pLeft->get_value();
                dot->set_x_value(fLeft);
            }
            if ((pTop != NULL) && (port == pTop))
            {
                fTop    = pTop->get_value();
                dot->set_y_value(fTop);
            }
            if ((pScroll != NULL) && (port == pScroll))
            {
                const port_t *p = pScroll->metadata();
                float min;
                float value = pScroll->get_value();

                if (is_log_rule(p))
                {
                    min = fabs(p->min);
//                    max = fabs(p->max);

                    min = (min < GAIN_AMP_M_80_DB) ? logf(GAIN_AMP_M_80_DB) - p->step : logf(min);
//                    max = (max < GAIN_AMP_M_80_DB) ? logf(GAIN_AMP_M_80_DB) - p->step : logf(max);

                    value = (value < GAIN_AMP_M_80_DB) ? min : logf(value);
                }

                dot->set_z_value(value);
            }
        }
    } /* namespace ctl */
} /* namespace lsp */
