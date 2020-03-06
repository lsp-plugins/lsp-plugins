/*
 * CtlText.cpp
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlText::metadata = { "CtlText", &CtlWidget::metadata };

        CtlText::CtlText(CtlRegistry *src, LSPText *text): CtlWidget(src, text)
        {
            pClass          = &metadata;
        }

        CtlText::~CtlText()
        {
        }

        void CtlText::update_coords()
        {
            LSPText *text       = widget_cast<LSPText>(pWidget);
            if (text == NULL)
                return;

            if (!sCoord.valid())
                return;

            sCoord.evaluate();
            if (sBasis.valid())
                sBasis.evaluate();

            size_t n = sCoord.results();
            text->set_axes(n);
            for (size_t i=0; i<n; ++i)
            {
                text->set_coord(i, sCoord.result(i));
                if ((sBasis.valid()) && (i < sBasis.results()))
                    text->set_basis(i, sBasis.result(i));
                else
                    text->set_basis(i, i);
            }
        }

        void CtlText::init()
        {
            CtlWidget::init();

            LSPText *text       = widget_cast<LSPText>(pWidget);
            if (text == NULL)
                return;

            // Initialize controllers
            sColor.init_hsl(pRegistry, text, text->font()->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sCoord.init(pRegistry, this);
        }

        void CtlText::end()
        {
            CtlWidget::end();
            update_coords();
        }

        void CtlText::set(const char *name, const char *value)
        {
            LSPText *text = widget_cast<LSPText>(pWidget);
            if (text != NULL)
                set_lc_attr(A_TEXT, text->text(), name, value);

            CtlWidget::set(name, value);
        }

        void CtlText::set(widget_attribute_t att, const char *value)
        {
            LSPText *text = widget_cast<LSPText>(pWidget);

            switch (att)
            {
                case A_COORD:
                    sCoord.parse(value, EXPR_FLAGS_MULTIPLE);
                    break;
                case A_BASIS:
                    sBasis.parse(value, EXPR_FLAGS_MULTIPLE);
                    break;
                case A_HALIGN:
                    if (text != NULL)
                        PARSE_FLOAT(value, text->set_halign(__));
                    break;
                case A_VALIGN:
                    if (text != NULL)
                        PARSE_FLOAT(value, text->set_valign(__));
                    break;
                case A_CENTER:
                    if (text != NULL)
                        PARSE_INT(value, text->set_center(__));
                    break;
                case A_SIZE:
                    if (text != NULL)
                        PARSE_FLOAT(value, text->font()->set_size(__));
                    break;
                default:
                {
                    sColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlText::notify(CtlPort *port)
        {
            CtlWidget::notify(port);
            update_coords();
        }
    } /* namespace ctl */
} /* namespace lsp */
