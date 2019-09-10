/*
 * para_equalizer_ui.h
 *
 *  Created on: 10 сент. 2019 г.
 *      Author: sadko
 */

#ifndef UI_PLUGINS_PARAEQUALIZERUI_H_
#define UI_PLUGINS_PARAEQUALIZERUI_H_

#include <ui/ui.h>
#include <ui/ctl/ctl.h>

namespace lsp
{
    
    class para_equalizer_ui: public plugin_ui
    {
        public:
            explicit para_equalizer_ui(const plugin_metadata_t *mdata, void *root_widget);
            virtual ~para_equalizer_ui();

        public:
            virtual status_t    build();
    };

} /* namespace lsp */

#endif /* UI_PLUGINS_PARAEQUALIZERUI_H_ */
