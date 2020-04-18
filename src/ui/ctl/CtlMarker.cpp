/*
 * CtlMarker.cpp
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlMarker::metadata = { "CtlMarker", &CtlWidget::metadata };

        CtlMarker::CtlMarker(CtlRegistry *src, LSPMarker *mark): CtlWidget(src, mark)
        {
            pClass          = &metadata;
            pPort           = NULL;
            fTransparency   = 0.0f;
        }

        CtlMarker::~CtlMarker()
        {
        }

        status_t CtlMarker::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlMarker *_this    = static_cast<CtlMarker *>(ptr);
            if (_this != NULL)
                _this->submit_values();
            return STATUS_OK;
        }

        status_t CtlMarker::slot_graph_resize(LSPWidget *sender, void *ptr, void *data)
        {
            CtlMarker *_this    = static_cast<CtlMarker *>(ptr);
            if (_this != NULL)
                _this->trigger_expr();
            return STATUS_OK;
        }

        void CtlMarker::submit_values()
        {
            if (pPort == NULL)
                return;
            LSPMarker *mark   = widget_cast<LSPMarker>(pWidget);
            if (mark == NULL)
                return;

            if (mark->editable())
            {
                float v = mark->value();
                if (v == pPort->get_value())
                    return;

                pPort->set_value(v);
                pPort->notify_all();
            }
        }

        void CtlMarker::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPMarker *mark   = widget_cast<LSPMarker>(pWidget);
            if (mark == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, mark, mark->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);

            // Bind slots
            mark->slots()->bind(LSPSLOT_CHANGE, slot_change, this);
            mark->slots()->bind(LSPSLOT_RESIZE_PARENT, slot_graph_resize, this);

            sAngle.init(pRegistry, this);
            sDX.init(pRegistry, this);
            sDY.init(pRegistry, this);
        }

        void CtlMarker::set(widget_attribute_t att, const char *value)
        {
            LSPMarker *mark = widget_cast<LSPMarker>(pWidget);

            switch (att)
            {
                case A_ID:
                    if (mark != NULL)
                        BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_VALUE:
                    if (mark != NULL)
                        PARSE_FLOAT(value, mark->set_value(__));
                    break;
                case A_OFFSET:
                    if (mark != NULL)
                        PARSE_FLOAT(value, mark->set_offset(__));
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
                case A_SMOOTH:
                    if (mark != NULL)
                        PARSE_BOOL(value, mark->set_smooth(__));
                    break;
                case A_FILL:
                    PARSE_FLOAT(value, fTransparency = __);
                    break;
                case A_BASIS:
                    if (mark != NULL)
                        PARSE_INT(value, mark->set_basis_id(__));
                    break;
                case A_PARALLEL:
                    if (mark != NULL)
                        PARSE_INT(value, mark->set_parallel_id(__));
                    break;
                case A_WIDTH:
                    if (mark != NULL)
                        PARSE_INT(value, mark->set_width(__));
                    break;
                case A_CENTER:
                    if (mark != NULL)
                        PARSE_INT(value, mark->set_center(__));
                    break;
                case A_BORDER:
                    if (mark != NULL)
                        PARSE_INT(value, mark->set_border(__));
                    break;
                case A_EDITABLE:
                    if (mark != NULL)
                        PARSE_BOOL(value, mark->set_editable(__));
                    break;
                case A_MIN:
                    if (mark != NULL)
                        PARSE_FLOAT(value, mark->set_minimum(__));
                    break;
                case A_MAX:
                    if (mark != NULL)
                        PARSE_FLOAT(value, mark->set_maximum(__));
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

        void CtlMarker::end()
        {
            if (pPort != NULL)
                notify(pPort);
            sColor.set_alpha(fTransparency);

            LSPMarker *mark = widget_cast<LSPMarker>(pWidget);
            if ((mark != NULL) && (mark->editable()))
            {
                const port_t *p = (pPort != NULL) ? pPort->metadata() : NULL;
                if (p != NULL)
                {
                    if (p->flags & F_LOWER)
                        mark->set_minimum(p->min);
                    if (p->flags & F_UPPER)
                        mark->set_maximum(p->max);
                }
            }

            trigger_expr();

            CtlWidget::end();
        }

        float CtlMarker::eval_expr(CtlExpression *expr)
        {
            LSPMarker *mark   = widget_cast<LSPMarker>(pWidget);
            if (mark == NULL)
                return 0.0f;

            LSPGraph *g = mark->graph();
            if (g == NULL)
                return 0.0f;

            expr->params()->clear();
            expr->params()->set_int("_g_width", g->width());
            expr->params()->set_int("_g_height", g->height());
            expr->params()->set_int("_a_width", g->area_width());
            expr->params()->set_int("_a_height", g->area_height());

            return expr->evaluate();
        }

        void CtlMarker::trigger_expr()
        {
            LSPMarker *mark   = widget_cast<LSPMarker>(pWidget);
            if (mark == NULL)
                return;

            if (sAngle.valid())
            {
                float angle = eval_expr(&sAngle);
                mark->set_angle(angle * M_PI);
            }

            if (sDX.valid())
            {
                float dx = eval_expr(&sDX);
                if (sDY.valid())
                {
                    float dy = eval_expr(&sDY);
                    mark->set_direction(dx, dy);
                }
                else
                    mark->set_dir_x(dx);
            }
            else if (sDY.valid())
            {
                float dy = eval_expr(&sDY);
                mark->set_dir_y(dy);
            }
        }

        void CtlMarker::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if (pPort == port)
            {
                LSPMarker *mark = widget_cast<LSPMarker>(pWidget);
                if (mark != NULL)
                    mark->set_value(pPort->get_value());
            }

            trigger_expr();
        }
    } /* namespace ctl */
} /* namespace lsp */
