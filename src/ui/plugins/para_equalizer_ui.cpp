/*
 * para_equalizer_ui.cpp
 *
 *  Created on: 10 сент. 2019 г.
 *      Author: sadko
 */

#include <ui/plugins/para_equalizer_ui.h>

namespace lsp
{
    
    para_equalizer_ui::para_equalizer_ui(const plugin_metadata_t *mdata, void *root_widget):
        plugin_ui(mdata, root_widget)
    {
    }
    
    para_equalizer_ui::~para_equalizer_ui()
    {
    }

    status_t para_equalizer_ui::build()
    {
        status_t res = plugin_ui::build();
        if (res != STATUS_OK)
            return res;

        return STATUS_OK;
    }

} /* namespace lsp */
