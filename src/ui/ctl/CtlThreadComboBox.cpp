/*
 * CtlThreadBox.cpp
 *
 *  Created on: 2 июл. 2019 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>
#include <core/ipc/Thread.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlThreadComboBox::metadata = { "CtlThreadComboBox", &CtlWidget::metadata };
        
        CtlThreadComboBox::CtlThreadComboBox(CtlRegistry *src, LSPComboBox *widget): CtlWidget(src, widget)
        {
            pClass          = &metadata;
            pPort           = NULL;
            idChange        = -1;
        }
        
        CtlThreadComboBox::~CtlThreadComboBox()
        {
            do_destroy();
        }

        void CtlThreadComboBox::init()
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

        void CtlThreadComboBox::do_destroy()
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

        void CtlThreadComboBox::destroy()
        {
            CtlWidget::destroy();
            do_destroy();
        }

        status_t CtlThreadComboBox::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlThreadComboBox *_this     = static_cast<CtlThreadComboBox *>(ptr);
            if (_this != NULL)
                _this->submit_value();
            return STATUS_OK;
        }

        void CtlThreadComboBox::submit_value()
        {
            if (pPort == NULL)
                return;
            const port_t *meta = pPort->metadata();
            if (meta == NULL)
                return;

            LSPComboBox *cbox = widget_cast<LSPComboBox>(pWidget);
            if (cbox == NULL)
                return;

            ssize_t index   = cbox->selected()+1;
            float v         = limit_value(meta, index);

            lsp_trace("index = %d, value=%f", int(index), v);

            pPort->set_value(v);
            pPort->notify_all();
        }

        void CtlThreadComboBox::set(widget_attribute_t att, const char *value)
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

        void CtlThreadComboBox::notify(CtlPort *port)
        {
            CtlWidget::notify(port);

            if ((pPort == port) && (pWidget != NULL))
            {
                ssize_t index = pPort->get_value();

                LSPComboBox *cbox = widget_cast<LSPComboBox>(pWidget);
                if (cbox != NULL)
                    cbox->set_selected(index-1);
            }
        }

        void CtlThreadComboBox::end()
        {
            LSPComboBox *cbox = widget_cast<LSPComboBox>(pWidget);
            if (cbox != NULL)
            {
                LSPString v;
                LSPItemList *list = cbox->items();
                LSPItem *item = NULL;

                for (size_t i=1, cores=ipc::Thread::system_cores(); i<=cores; ++i)
                {
                    if (!v.fmt_ascii("%d", int(i)))
                        continue;

                    if (list->add(&item) == STATUS_OK)
                    {
                        item->text()->set_raw(&v);
                        item->set_value(i);
                    }
                }
            }

            CtlWidget::end();
        }
    
    } /* namespace ctl */
} /* namespace lsp */
