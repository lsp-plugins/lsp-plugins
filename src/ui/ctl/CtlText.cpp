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
        CtlText::CtlText(CtlRegistry *src, LSPText *text): CtlWidget(src, text)
        {
        }

        CtlText::~CtlText()
        {
        }

        void CtlText::update_coords()
        {
            if (pWidget == NULL)
                return;

            LSPText *text       = static_cast<LSPText *>(pWidget);
            if (!sCoord.valid())
                return;

            sCoord.evaluate();
            size_t n = sCoord.results();
            text->set_axes(n);
            for (size_t i=0; i<n; ++i)
                text->set_coord(i, sCoord.result(i));
        }

        void CtlText::init()
        {
            CtlWidget::init();
            if (pWidget == NULL)
                return;

            LSPText *text       = static_cast<LSPText *>(pWidget);

            // Initialize controllers
            sColor.init_hsl(pRegistry, text, text->font()->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sCoord.init(pRegistry, this);
        }

        void CtlText::end()
        {
            CtlWidget::end();
            update_coords();
        }

        void CtlText::set(widget_attribute_t att, const char *value)
        {
            LSPText *text = (pWidget != NULL) ? static_cast<LSPText *>(pWidget) : NULL;

            switch (att)
            {
                case A_COORD:
                    sCoord.parse(value, EXPR_FLAGS_MULTIPLE);
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
                case A_TEXT:
                    if (text != NULL)
                        text->set_text(value);
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

        void CtlText::notify(CtlPort *port)
        {
            CtlWidget::notify(port);
            update_coords();
        }
    } /* namespace ctl */
} /* namespace lsp */
