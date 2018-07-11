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
        CtlMarker::CtlMarker(CtlRegistry *src, LSPMarker *mark): CtlWidget(src, mark)
        {
            pPort           = NULL;
            fTransparency   = 0.0f;
        }

        CtlMarker::~CtlMarker()
        {
        }

        void CtlMarker::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPMarker *mark   = static_cast<LSPMarker *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, mark, mark->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
        }

        void CtlMarker::set(widget_attribute_t att, const char *value)
        {
            LSPMarker *mark = (pWidget != NULL) ? static_cast<LSPMarker *>(pWidget) : NULL;

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
                    if (mark != NULL)
                        PARSE_FLOAT(value, mark->set_angle(__));
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
            CtlWidget::end();
        }

        void CtlMarker::notify(CtlPort *port)
        {
            CtlWidget::notify(port);
            if (pWidget == NULL)
                return;

            if (pPort == port)
            {
                LSPMarker *mark = static_cast<LSPMarker *>(pWidget);
                mark->set_value(pPort->get_value());
            }
        }
    } /* namespace ctl */
} /* namespace lsp */
