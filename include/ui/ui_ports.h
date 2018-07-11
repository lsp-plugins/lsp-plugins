/*
 * ui_ports.h
 *
 *  Created on: 11 авг. 2016 г.
 *      Author: sadko
 */

#ifndef UI_UI_PORTS_H_
#define UI_UI_PORTS_H_

#include <metadata/metadata.h>
#include <ui/IUIPort.h>

namespace lsp
{
    class UIControlPort: public IUIPort
    {
        private:
            float       fValue;
            plugin_ui  *pUI;

        public:
            UIControlPort(const port_t *meta, plugin_ui *ui): IUIPort(meta)
            {
                fValue  = meta->start;
                pUI     = ui;
            }

            virtual ~UIControlPort()
            {
                pUI     = NULL;
            }

        public:
            virtual float getValue()
            {
                return fValue;
            }

            virtual void setValue(float value)
            {
                float v  = limit_value(pMetadata, value);
                if (fValue != v)
                {
                    fValue  = v;
                    if (pUI != NULL)
                        pUI->save_global_config();
                }
            }
    };
}

#endif /* UI_UI_PORTS_H_ */
