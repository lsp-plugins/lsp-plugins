/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 17 июл. 2017 г.
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

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlGroup::metadata = { "CtlGroup", &CtlWidget::metadata };

        CtlGroup::CtlGroup(CtlRegistry *src, LSPGroup *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
        }

        CtlGroup::~CtlGroup()
        {
            do_destroy();
        }

        void CtlGroup::init()
        {
            CtlWidget::init();

            if (pWidget == NULL)
                return;
            LSPGroup *grp       = static_cast<LSPGroup *>(pWidget);

            // Initialize color controllers
            sColor.init_hsl(pRegistry, grp, grp->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sTextColor.init_basic(pRegistry, grp, grp->text_color(), A_TEXT_COLOR);
        }

        void CtlGroup::destroy()
        {
            CtlWidget::destroy();
            do_destroy();
        }

        void CtlGroup::do_destroy()
        {
            sEmbed.destroy();
        }

        void CtlGroup::set(const char *name, const char *value)
        {
            LSPGroup *grp       = widget_cast<LSPGroup>(pWidget);
            if (grp != NULL)
                set_lc_attr(A_TEXT, grp->text(), name, value);

            CtlWidget::set(name, value);
        }

        void CtlGroup::set(widget_attribute_t att, const char *value)
        {
            LSPGroup *grp       = widget_cast<LSPGroup>(pWidget);

            switch (att)
            {
                case A_BORDER:
                    if (grp != NULL)
                        PARSE_INT(value, grp->set_border(__));
                    break;
                case A_RADIUS:
                    if (grp != NULL)
                        PARSE_INT(value, grp->set_radius(__));
                    break;
                case A_EMBED:
                    BIND_EXPR(sEmbed, value);
                    break;
                default:
                {
                    sColor.set(att, value);
                    sTextColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        status_t CtlGroup::add(CtlWidget *child)
        {
            if (pWidget == NULL)
                return STATUS_BAD_STATE;

            LSPGroup *grp     = widget_cast<LSPGroup>(pWidget);
            return grp->add(child->widget());
        }

        void CtlGroup::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            LSPGroup *grp     = widget_cast<LSPGroup>(pWidget);
            if (grp == NULL)
                return;

            if (sEmbed.valid())
            {
                float value = sEmbed.evaluate();
                grp->set_embed(value >= 0.5f);
            }
        }

    } /* namespace ctl */
} /* namespace lsp */
