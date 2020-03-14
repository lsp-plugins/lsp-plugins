/*
 * CtlComboBox.cpp
 *
 *  Created on: 21 авг. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlComboBox::metadata = { "CtlComboBox", &CtlWidget::metadata };

        CtlComboBox::CtlComboBox(CtlRegistry *src, LSPComboBox *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pPort           = NULL;
            fMin            = 0.0f;
            fMax            = 0.0f;
            fStep           = 0.0f;
            idChange        = -1;
        }

        CtlComboBox::~CtlComboBox()
        {
            do_destroy();
        }

        void CtlComboBox::init()
        {
            CtlWidget::init();

            LSPComboBox *cbox = widget_cast<LSPComboBox>(pWidget);
            if (cbox == NULL)
                return;

            // Initialize color controllers
            sColor.init_hsl(pRegistry, cbox, cbox->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);

            // Bind slots
            idChange = cbox->slots()->bind(LSPSLOT_CHANGE, slot_change, this);
        }

        void CtlComboBox::do_destroy()
        {
            LSPComboBox *cbox = widget_cast<LSPComboBox>(pWidget);
            if (cbox == NULL)
                return;

            if (idChange >= 0)
            {
                cbox->slots()->unbind(LSPSLOT_CHANGE, idChange);
                idChange = -1;
            }
        }

        void CtlComboBox::destroy()
        {
            CtlWidget::destroy();
            do_destroy();
        }

        status_t CtlComboBox::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlComboBox *_this    = static_cast<CtlComboBox *>(ptr);
            if (_this != NULL)
                _this->submit_value();
            return STATUS_OK;
        }

        void CtlComboBox::submit_value()
        {
            if (pPort == NULL)
                return;

            LSPComboBox *cbox = widget_cast<LSPComboBox>(pWidget);
            if (cbox == NULL)
                return;
            ssize_t index = cbox->selected();

            float value = fMin + fStep * index;
            lsp_trace("index = %d, value=%f", int(index), value);

            pPort->set_value(value);
            pPort->notify_all();
        }

        void CtlComboBox::set(widget_attribute_t att, const char *value)
        {
            LSPComboBox *cbox = widget_cast<LSPComboBox>(pWidget);

            switch (att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_WIDTH:
                    if (cbox != NULL)
                        PARSE_INT(value, cbox->set_min_width(__));
                    break;
                case A_HEIGHT:
                    if (cbox != NULL)
                        PARSE_INT(value, cbox->set_min_height(__));
                    break;
                default:
                {
                    sColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
                }
            }
        }

        void CtlComboBox::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if ((pPort == port) && (pWidget != NULL))
            {
                ssize_t index = (pPort->get_value() - fMin) / fStep;

                LSPComboBox *cbox = static_cast<LSPComboBox *>(pWidget);
                cbox->set_selected(index);
            }
        }

        void CtlComboBox::sync_metadata(CtlPort *port)
        {
            LSPComboBox *cbox = widget_cast<LSPComboBox>(pWidget);
            if (cbox == NULL)
                return;

            if (port == pPort)
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
                        lst->clear();

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
        }

        void CtlComboBox::end()
        {
            if (pPort != NULL)
                sync_metadata(pPort);

            CtlWidget::end();
        }
    } /* namespace ctl */
} /* namespace lsp */
