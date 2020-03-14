/*
 * ComboGroup.cpp
 *
 *  Created on: 29 апр. 2018 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlComboGroup::metadata = { "CtlComboGroup", &CtlWidget::metadata };

        CtlComboGroup::CtlComboGroup(CtlRegistry *src, LSPComboGroup *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pPort           = NULL;
            fMin            = 0.0f;
            fMax            = 0.0f;
            fStep           = 0.0f;
            idChange        = -1;
            pText           = NULL;
        }

        CtlComboGroup::~CtlComboGroup()
        {
        }

        void CtlComboGroup::do_destroy()
        {
            sEmbed.destroy();

            LSPComboGroup *grp = widget_cast<LSPComboGroup>(pWidget);
            if (grp == NULL)
                return;

            if (pText != NULL)
            {
                free(pText);
                pText = NULL;
            }

            if (idChange >= 0)
            {
                grp->slots()->unbind(LSPSLOT_CHANGE, idChange);
                idChange = -1;
            }
        }

        void CtlComboGroup::destroy()
        {
            CtlWidget::destroy();
            sEmbed.destroy();
            do_destroy();
        }

        status_t CtlComboGroup::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlComboGroup *_this    = static_cast<CtlComboGroup *>(ptr);
            if (_this != NULL)
                _this->submit_value();
            return STATUS_OK;
        }

        void CtlComboGroup::submit_value()
        {
            LSPComboGroup *grp = widget_cast<LSPComboGroup>(pWidget);
            if (grp == NULL)
            {
                lsp_trace("GRP IS NULL");
                return;
            }
            lsp_trace("GRP IS NOT NULL");
            ssize_t index = grp->selected();

            float value = fMin + fStep * index;
            lsp_trace("index = %d, value=%f", int(index), value);

            pPort->set_value(value);
            pPort->notify_all();
        }


        void CtlComboGroup::init()
        {
            CtlWidget::init();

            LSPComboGroup *grp       = widget_cast<LSPComboGroup>(pWidget);
            if (grp == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, grp, grp->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sTextColor.init_basic(pRegistry, grp, grp->font()->color(), A_TEXT_COLOR);

            // Bind slots
            idChange = grp->slots()->bind(LSPSLOT_CHANGE, slot_change, this);

            // Init embed expression
            sEmbed.init(pRegistry, this);
        }

        void CtlComboGroup::set(widget_attribute_t att, const char *value)
        {
            LSPComboGroup *grp       = widget_cast<LSPComboGroup>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_BORDER:
                    if (grp != NULL)
                        PARSE_INT(value, grp->set_border(__));
                    break;
                case A_RADIUS:
                    if (grp != NULL)
                        PARSE_INT(value, grp->set_radius(__));
                    break;
                case A_TEXT:
                    PARSE_STRING(value, pText);
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

        status_t CtlComboGroup::add(CtlWidget *child)
        {
            LSPComboGroup *grp     = widget_cast<LSPComboGroup>(pWidget);
            return (grp != NULL) ? grp->add(child->widget()) : STATUS_BAD_STATE;
        }
    
        void CtlComboGroup::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            LSPComboGroup *cgroup = widget_cast<LSPComboGroup>(pWidget);
            if (cgroup == NULL)
                return;

            if (pPort == port)
            {
                ssize_t index = (pPort->get_value() - fMin) / fStep;
                cgroup->set_selected(index);
            }

            if (sEmbed.valid())
            {
                float value = sEmbed.evaluate();
                cgroup->set_embed(value >= 0.5f);
            }
        }

        void CtlComboGroup::end()
        {
            LSPComboGroup *cbox = widget_cast<LSPComboGroup>(pWidget);
            if (cbox == NULL)
                return;

            // Evaluate expression
            if (sEmbed.valid())
            {
                float value = sEmbed.evaluate();
                cbox->set_embed(value >= 0.5f);
            }

            if (pWidget != NULL)
            {
                const port_t *p = (pPort != NULL) ? pPort->metadata() : NULL;

                if (p != NULL)
                {
                    get_port_parameters(pPort->metadata(), &fMin, &fMax, &fStep);

                    if (p->unit == U_ENUM)
                    {
                        size_t value    = pPort->get_value();
                        size_t i        = 0;
                        LSPItemList *lst= cbox->items();

                        LSPString prefix, text;
                        if (pText != NULL)
                            prefix.set_native(pText);

                        LSPItem li;
                        LSPString lck;

                        for (const port_item_t *item = p->items; (item != NULL) && (item->text != NULL); ++item, ++i)
                        {
                            size_t key      = fMin + fStep * i;
                            if (item->lc_key != NULL)
                            {
                                lck.set_ascii("lists.");
                                lck.append_ascii(item->lc_key);
                                li.text()->set(&lck);
                            }
                            else
                                li.text()->set_raw(item->text);
                            li.set_value(key);
                            lst->add(&li);

                            if (key == value)
                                cbox->set_selected(i);
                        }
                    }
                }
            }

            CtlWidget::end();
        }
    } /* namespace ctl */
} /* namespace lsp */
