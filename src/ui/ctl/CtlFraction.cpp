/*
 * CtlFraction.cpp
 *
 *  Created on: 1 июн. 2018 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlFraction::metadata = { "CtlFraction", &CtlWidget::metadata };
        
        CtlFraction::CtlFraction(CtlRegistry *src, LSPFraction *frac):
                CtlWidget(src, frac)
        {
            pClass          = &metadata;
            pPort           = NULL;
            pDenom          = NULL;
            fSig            = 0.0f;
            fMaxSig         = 2.0f;
            nNum            = 0;
            nDenom          = 4;
            nDenomMin       = 1;
            nDenomMax       = 64;
        }
        
        CtlFraction::~CtlFraction()
        {
        }

        void CtlFraction::set(widget_attribute_t att, const char *value)
        {
            switch(att)
            {
                case A_ID:
                    BIND_PORT(pRegistry, pPort, value);
                    break;
                case A_LENGTH_ID:
                case A_ID2:
                    BIND_PORT(pRegistry, pDenom, value);
                    break;
                case A_MAX:
                    PARSE_FLOAT(value, fMaxSig = __);
                    break;

                default:
                    sTextColor.set(att, value);
                    CtlWidget::set(att, value);
                    break;
            }
        }

        void CtlFraction::init()
        {
            CtlWidget::init();

            LSPFraction *frac = widget_cast<LSPFraction>(pWidget);
            if (frac == NULL)
                return;

            // Initialize color controllers
            sTextColor.init_hsl(pRegistry, frac, frac->font()->color(), A_COLOR, A_HUE_ID, A_SAT_ID, A_LIGHT_ID);
            sColor.init_hsl(pRegistry, frac, frac->font()->color(), A_SCALE_COLOR, A_SCALE_HUE_ID, A_SCALE_SAT_ID, A_SCALE_LIGHT_ID);

            // Bind slots
            frac->slots()->bind(LSPSLOT_CHANGE, slot_change, this);
        }

        void CtlFraction::begin()
        {
        }

        status_t CtlFraction::slot_change(LSPWidget *sender, void *ptr, void *data)
        {
            CtlFraction *_this      = static_cast<CtlFraction *>(ptr);
            if (_this != NULL)
                _this->submit_value();
            return STATUS_OK;
        }

        void CtlFraction::end()
        {
            LSPFraction *frac = widget_cast<LSPFraction>(pWidget);
            if (frac == NULL)
                return;

            char v[32];
            LSPItem *item = NULL;
            if (pDenom != NULL)
            {
                const port_t *p = (pDenom != NULL) ? pDenom->metadata() : NULL;
                if (p == NULL)
                    return;

                if (p->flags & F_LOWER)
                    nDenomMin   = p->min;
                if ((p->unit == U_ENUM) && (p->unit == U_ENUM))
                    nDenomMax   = nDenomMin + list_size(p->items);
                else if (p->flags & F_UPPER)
                    nDenomMax   = p->max;

                // Generate denominator list
                LSPItemList *dl = frac->denom_items();
                dl->clear();
                if (p->unit == U_ENUM)
                {
                    for (ssize_t i=nDenomMin; i<=nDenomMax; ++i)
                    {
                        if (dl->add(&item) == STATUS_OK)
                        {
                            item->text()->set_raw(p->items[i].text);
                            item->set_value(i);
                        }
                    }
                }
                else
                {
                    for (ssize_t i=nDenomMin; i<=nDenomMax; ++i)
                    {
                        if (dl->add(&item) == STATUS_OK)
                        {
                            snprintf(v, 32, "%d", int(i));
                            item->text()->set_raw(v);
                            item->set_value(i);
                        }
                    }
                }
            }
            else
            {
                LSPItemList *dl = frac->denom_items();
                dl->clear();

                for (ssize_t i=nDenomMin; i<=nDenomMax; ++i)
                {
                    if ((dl->add(&item)) == STATUS_OK)
                    {
                        snprintf(v, 32, "%d", int(i));
                        item->text()->set_raw(v);
                        item->set_value(i);
                    }
                }
            }

            if (nDenom < nDenomMin)
                nDenom = nDenomMin;
            else if (nDenom > nDenomMax)
                nDenom = nDenomMax;


            // Call for values update
            update_values();
        }

        void CtlFraction::notify(CtlPort *port)
        {
            if ((port == pPort) || (port == pDenom))
                update_values();
            CtlWidget::notify(port);
        }

        void CtlFraction::update_values()
        {
            LSPFraction *frac = widget_cast<LSPFraction>(pWidget);
            if (frac == NULL)
                return;

            if (pDenom != NULL)
                nDenom      = pDenom->get_value();
            if (pPort != NULL)
            {
                fSig        = pPort->get_value();
                if (fSig < 0.0f)
                    fSig        = 0.0f;
                else if (fSig > fMaxSig)
                    fSig        = fMaxSig;
            }
            frac->set_denom_selected(nDenom - 1);

            sync_numerator(frac);
        }

        void CtlFraction::submit_value()
        {
            LSPFraction *frac = widget_cast<LSPFraction>(pWidget);
            if (frac == NULL)
                return;

            nNum        = frac->num_selected();
            nDenom      = frac->denom_selected() + 1;

            ssize_t num_min = 0;
            ssize_t num_max = fMaxSig * nDenom;
            if (nNum < num_min)
                nNum    = num_min;
            else if (nNum > num_max)
                nNum    = num_max;

            fSig        = float(nNum) / float(nDenom);
            sync_numerator(frac);

            pPort->set_value(fSig);
            pDenom->set_value(nDenom);
            pPort->notify_all();
            pDenom->notify_all();
        }

        void CtlFraction::sync_numerator(LSPFraction *frac)
        {
            ssize_t num_max = fMaxSig * nDenom;

            LSPItemList *nl = frac->num_items();
            ssize_t n       = nl->size();

            char v[32];
            LSPItem *item = NULL;
            for (ssize_t i=n; i<=num_max; ++i)
            {
                if ((nl->add(&item)) == STATUS_OK)
                {
                    snprintf(v, 32, "%d", int(i));
                    item->text()->set_raw(v);
                    item->set_value(i);
                }
            }
            nl->truncate(num_max+1);

            nNum            = fSig * nDenom;
            frac->set_num_selected(nNum);
        }
    
    } /* namespace ctl */
} /* namespace lsp */
