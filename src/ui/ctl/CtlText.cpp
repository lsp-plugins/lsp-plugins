/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 27 июл. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <ui/ctl/ctl.h>

#define TMP_BUF_SIZE        128

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlText::metadata = { "CtlText", &CtlWidget::metadata };

        CtlText::CtlText(CtlRegistry *src, LSPText *text): CtlWidget(src, text)
        {
            pPort           = NULL;

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

        void CtlText::update_text()
        {
            // Update text
            if (pPort == NULL)
                return;

            LSPText *text       = widget_cast<LSPText>(pWidget);
            if (text == NULL)
                return;

            // Get port metadata and value
            const port_t *mdata = pPort->metadata();
            if (mdata == NULL)
                return;
            float value         = pPort->get_value();

            // Format the value
            char buf[TMP_BUF_SIZE];
            calc::Parameters params;
            format_value(buf, TMP_BUF_SIZE, mdata, value, -1);

            // Update text parameter
            text->text()->params()->set_cstring("value", buf);
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
            if (pPort != NULL)
                update_text();
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
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
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
            if (port == pPort)
                update_text();
        }
    } /* namespace ctl */
} /* namespace lsp */
