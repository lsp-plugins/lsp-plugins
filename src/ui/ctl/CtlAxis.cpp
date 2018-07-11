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
        CtlAxis::CtlAxis(CtlRegistry *src, LSPAxis *axis): CtlWidget(src, axis)
        {
            nFlags      = 0;
            fMin        = 0.0f;
            fMax        = 0.0f;
            pPort       = NULL;
        }

        CtlAxis::~CtlAxis()
        {
        }

        void CtlAxis::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPAxis *axis   = static_cast<LSPAxis *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, axis, axis->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
        }

        void CtlAxis::set(widget_attribute_t att, const char *value)
        {
            LSPAxis *axis   = (pWidget != NULL) ? static_cast<LSPAxis *>(pWidget) : NULL;

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_ANGLE:
                    if (axis != NULL)
                        PARSE_FLOAT(value, axis->set_angle(__ * M_PI));
                    break;
                case A_MIN:
                    PARSE_FLOAT(value, { fMin = __; nFlags |= F_MIN_SET; } );
                    break;
                case A_MAX:
                    PARSE_FLOAT(value, { fMax = __; nFlags |= F_MAX_SET; } );
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
        }

        void CtlAxis::update_axis()
        {
            LSPAxis *axis   = widget_cast<LSPAxis>(pWidget);
            if (axis == NULL)
                return;

            const port_t *mdata = (pPort != NULL) ? pPort->metadata() : NULL;
            if (mdata == NULL)
            {
                if (nFlags & F_MIN_SET)
                    axis->set_min_value(fMin);
                if (nFlags & F_MAX_SET)
                    axis->set_max_value(fMax);
                if (nFlags & F_LOG_SET)
                    axis->set_log_scale(nFlags & F_LOG);
                return;
            }

            axis->set_min_value((nFlags & F_MIN_SET) ? pPort->get_value() * fMin : mdata->min);
            axis->set_max_value((nFlags & F_MAX_SET) ? pPort->get_value() * fMax : mdata->max);
            if (nFlags & F_LOG_SET)
                axis->set_log_scale(nFlags & F_LOG);
            else
                axis->set_log_scale(((is_decibel_unit(mdata->unit)) || (mdata->flags & F_LOG)));
        }

        void CtlAxis::notify(CtlPort *port)
        {
            CtlWidget::notify(port);
            if (pPort == port)
                update_axis();
        }

    } /* namespace ctl */
} /* namespace lsp */
