/*
 * ui_builder.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_UI_BUILDER_H_
#define _UI_UI_BUILDER_H_

#include <data/cvector.h>
#include <core/buffer.h>

namespace lsp
{
    typedef struct ui_variable_t
    {
        char       *sName;
        ssize_t     nValue;
    } ui_variable_t;

    class ui_builder
    {
        private:
            plugin_ui                  *pUI;
            cvector<ui_variable_t>      vVars;

        public:
            explicit ui_builder(plugin_ui *ui);
            virtual ~ui_builder();

        public:
            bool build(const char *path);

            inline plugin_ui *get_ui()   { return pUI; }

            ui_variable_t *get_variable(const char *id);
    };
}

#endif /* _UI_UI_BUILDER_H_ */
