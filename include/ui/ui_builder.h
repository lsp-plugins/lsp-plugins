/*
 * ui_builder.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_UI_BUILDER_H_
#define _UI_UI_BUILDER_H_

#include <expat.h>

namespace lsp
{
    class ui_builder
    {
        private:
            plugin_ui      *pUI;

        public:
            explicit ui_builder(plugin_ui *ui);
            virtual ~ui_builder();

        public:
            bool build(const char *path);
    };
}

#endif /* _UI_UI_BUILDER_H_ */
