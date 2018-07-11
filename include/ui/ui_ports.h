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
        protected:
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

    class UIPathPort: public IUIPort
    {
        protected:
            char            sPath[PATH_MAX];
            plugin_ui      *pUI;

        public:
            UIPathPort(const port_t *meta, plugin_ui *ui): IUIPort(meta)
            {
                sPath[0]    = '\0';
                pUI         = ui;
            }

            virtual ~UIPathPort()
            {
                sPath[0]    = '\0';
                pUI         = NULL;
            }

            virtual void write(const void* buffer, size_t size)
            {
                // Check that attribute didn't change
                if ((size != strlen(sPath)) && (memcmp(sPath, buffer, size) == 0))
                    return;

                if ((buffer != NULL) && (size > 0))
                {
                    size_t copy     = (size >= PATH_MAX) ? PATH_MAX-1 : size;
                    memcpy(sPath, buffer, size);
                    sPath[copy]     = '\0';
                }
                else
                    sPath[0]        = '\0';

                // Save config (if possible)
                if (pUI != NULL)
                    pUI->save_global_config();
            }

            virtual void *getBuffer()
            {
                return sPath;
            }
    };
}

#endif /* UI_UI_PORTS_H_ */
