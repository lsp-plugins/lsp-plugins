/*
 * plugin_ui.h
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_PLUGIN_UI_H_
#define _UI_PLUGIN_UI_H_

#include <core/metadata.h>
#include <data/cvector.h>

namespace lsp
{
    class plugin_ui
    {
        private:
            cvector<IWidget>        vWidgets;
            cvector<IUIPort>        vPorts;
            Theme                   sTheme;

        protected:
            const char                 *sName;
            const plugin_metadata_t    *pMetadata;
            IWidgetFactory             *pFactory;

        public:
            plugin_ui(const char *name, const plugin_metadata_t *mdata, IWidgetFactory *factory);

            virtual ~plugin_ui();

        private:
            IWidget *addWidget(IWidget *widget);

        public:
            IWidget *createWidget(const char *w_class);
            IWidget *createWidget(widget_t w_class);
            inline const plugin_metadata_t *metadata() const { return pMetadata; };
            inline Theme &theme() { return sTheme; };

        public:
            virtual void init();

            virtual void build();

            virtual void destroy();

            virtual bool add_port(IUIPort *port);

            /** Get INTERNAL port by name
             *
             * @param name port name
             * @return internal port
             */
            virtual IUIPort *port(const char *name);

            /** Get port count
             *
             * @return number of ports
             */
            inline size_t ports_count()
            {
                return vPorts.size();
            }
    };

} /* namespace lsp */

#endif /* _UI_PLUGIN_UI_H_ */
