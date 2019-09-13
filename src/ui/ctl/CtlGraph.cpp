/*
 * CtlGraph.cpp
 *
 *  Created on: 26 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlGraph::metadata = { "CtlGraph", &CtlWidget::metadata };

        CtlGraph::CtlGraph(CtlRegistry *src, LSPGraph *graph): CtlWidget(src, graph)
        {
            pClass          = &metadata;
        }

        CtlGraph::~CtlGraph()
        {
        }

        void CtlGraph::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPGraph *gr    = widget_cast<LSPGraph>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, gr, gr->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sPadding.init(gr->padding());
        }

        void CtlGraph::set(widget_attribute_t att, const char *value)
        {
            LSPGraph *gr    = (pWidget != NULL) ? widget_cast<LSPGraph>(pWidget) : NULL;

            switch (att)
            {
                case A_WIDTH:
                    if (gr != NULL)
                        PARSE_INT(value, gr->set_min_width(__));
                    break;
                case A_HEIGHT:
                    if (gr != NULL)
                        PARSE_INT(value, gr->set_min_height(__));
                    break;
                case A_BORDER:
                    if (gr != NULL)
                        PARSE_INT(value, gr->set_border(__));
                    break;
                case A_SPACING:
                    if (gr != NULL)
                        PARSE_INT(value, gr->set_radius(__));
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

        status_t CtlGraph::add(CtlWidget *child)
        {
            if (pWidget == NULL)
                return STATUS_BAD_STATE;

            LSPGraph *gr    = widget_cast<LSPGraph>(pWidget);
            return gr->add(child->widget());
        }
    } /* namespace ctl */
} /* namespace lsp */
