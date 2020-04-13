/*
 * CtlAxis.cpp
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlAxis::metadata = { "CtlAxis", &CtlWidget::metadata };

        CtlAxis::CtlAxis(CtlRegistry *src, LSPAxis *axis): CtlWidget(src, axis)
        {
            pClass          = &metadata;
            nFlags          = 0;
            pPort           = NULL;
        }

        CtlAxis::~CtlAxis()
        {
        }

        status_t CtlAxis::slot_graph_resize(LSPWidget *sender, void *ptr, void *data)
        {
            CtlAxis *_this    = static_cast<CtlAxis *>(ptr);
            if (_this != NULL)
                _this->trigger_expr();
            return STATUS_OK;
        }

        void CtlAxis::init()
        {
            CtlWidget::init();
            sMin.init(pRegistry, this);
            sMax.init(pRegistry, this);
            sAngle.init(pRegistry, this);
            sLength.init(pRegistry, this);
            sDX.init(pRegistry, this);
            sDY.init(pRegistry, this);

            LSPAxis *axis   = widget_cast<LSPAxis>(pWidget);
            if (axis == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, axis, axis->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);

            axis->slots()->bind(LSPSLOT_RESIZE_PARENT, slot_graph_resize, this);
        }

        void CtlAxis::set(widget_attribute_t att, const char *value)
        {
            LSPAxis *axis   = widget_cast<LSPAxis>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_ANGLE:
                    BIND_EXPR(sAngle, value);
                    break;
                case A_DX:
                    BIND_EXPR(sDX, value);
                    break;
                case A_DY:
                    BIND_EXPR(sDY, value);
                    break;
                case A_LENGTH:
                    BIND_EXPR(sLength, value);
                    break;
                case A_MIN:
                    BIND_EXPR(sMin, value);
                    nFlags |= F_MIN_SET;
                    break;
                case A_MAX:
                    BIND_EXPR(sMax, value);
                    nFlags |= F_MAX_SET;
                    break;
                case A_CENTER:
                    if (axis != NULL)
                        PARSE_INT(value, axis->set_center_id(__));
                    break;
                case A_BASIS:
                    if (axis != NULL)
                        PARSE_BOOL(value, axis->set_basis(__));
                    break;
                case A_LOGARITHMIC:
                    PARSE_BOOL(value, {
                            nFlags |= F_LOG_SET;
                            if (__)
                                nFlags |= F_LOG;
                            else
                                nFlags &= ~F_LOG;
                        });
                    break;
                case A_WIDTH:
                    if (axis != NULL)
                        PARSE_INT(value, axis->set_line_width(__));
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

        void CtlAxis::end()
        {
            CtlWidget::end();
            update_axis();
            trigger_expr();
        }

        float CtlAxis::eval_expr(CtlExpression *expr)
        {
            LSPAxis *axis = widget_cast<LSPAxis>(pWidget);
            if (axis == NULL)
                return 0.0f;

            LSPGraph *g = axis->graph();
            if (g == NULL)
                return 0.0f;

            expr->params()->clear();
            expr->params()->set_int("_g_width", g->width());
            expr->params()->set_int("_g_height", g->height());
            expr->params()->set_int("_a_width", g->area_width());
            expr->params()->set_int("_a_height", g->area_height());

            return expr->evaluate();
        }

        void CtlAxis::trigger_expr()
        {
            LSPAxis *axis   = widget_cast<LSPAxis>(pWidget);
            if (axis == NULL)
                return;

            if (sAngle.valid())
            {
                float angle = eval_expr(&sAngle);
                axis->set_angle(angle * M_PI);
            }

            if (sLength.valid())
            {
                float length = eval_expr(&sLength);
                axis->set_length(length);
            }

            if (sDX.valid())
            {
                float dx = eval_expr(&sDX);
                if (sDY.valid())
                {
                    float dy = eval_expr(&sDY);
                    axis->set_direction(dx, dy);
                }
                else
                    axis->set_dir_x(dx);
            }
            else if (sDY.valid())
            {
                float dy = eval_expr(&sDY);
                axis->set_dir_y(dy);
            }
        }

        void CtlAxis::update_axis()
        {
            LSPAxis *axis   = widget_cast<LSPAxis>(pWidget);
            if (axis == NULL)
                return;

            float amin = (sMin.valid()) ? sMin.evaluate() : 0.0f;
            float amax = (sMax.valid()) ? sMax.evaluate() : 0.0f;

            const port_t *mdata = (pPort != NULL) ? pPort->metadata() : NULL;
            if (mdata == NULL)
            {
                if (nFlags & F_MIN_SET)
                    axis->set_min_value(amin);
                if (nFlags & F_MAX_SET)
                    axis->set_max_value(amax);
                if (nFlags & F_LOG_SET)
                    axis->set_log_scale(nFlags & F_LOG);
                return;
            }

            axis->set_min_value((nFlags & F_MIN_SET) ? pPort->get_value() * amin : mdata->min);
            axis->set_max_value((nFlags & F_MAX_SET) ? pPort->get_value() * amax : mdata->max);
            if (nFlags & F_LOG_SET)
                axis->set_log_scale(nFlags & F_LOG);
            else
                axis->set_log_scale(((is_decibel_unit(mdata->unit)) || (mdata->flags & F_LOG)));
        }

        void CtlAxis::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            update_axis();
            trigger_expr();
        }

    } /* namespace ctl */
} /* namespace lsp */
