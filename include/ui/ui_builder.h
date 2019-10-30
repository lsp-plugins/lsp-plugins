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
#include <core/calc/Expression.h>
#include <core/calc/Variables.h>

namespace lsp
{
    class ui_builder
    {
        private:
            ui_builder & operator = (const ui_builder &);

        private:
            plugin_ui                  *pUI;
            cvector<calc::Variables>    vStack;
            calc::Variables             vRoot;

        private:
            status_t evaluate(calc::value_t *value, const LSPString *expr);

            inline calc::Variables *vars() { calc::Variables *r = vStack.last();  return (r != NULL) ? r : &vRoot; }

        public:
            explicit ui_builder(plugin_ui *ui);
            ~ui_builder();

        public:
            /**
             * Build UI
             * @param uri URI of the XML resource
             * @return status of operation
             */
            status_t build(const LSPString *uri);

            /**
             * Build UI
             * @param uri URI of the XML resource
             * @return status of operation
             */
            status_t build(const char *uri);

            /**
             * Get the pointer to plugin UI
             * @return pointer to plugin UI
             */
            inline plugin_ui *get_ui()   { return pUI; }

        public:
            /**
             * Start new nested variable scope
             * @return status of operation
             */
            status_t    push_scope();

            /**
             * Remove nested variable scope and destroy all nested variables
             * @return status of operation
             */
            status_t    pop_scope();

            /**
             * Get current variable resolver
             * @return current variable resolver
             */
            inline calc::Resolver *resolver() { calc::Variables *r = vStack.last();  return (r != NULL) ? r : &vRoot; }

            /**
             * Evaluate value and return as string
             * @param var pointer to store string
             * @param expr expression
             * @return status of operation
             */
            status_t eval_string(LSPString *value, const LSPString *expr);

            /**
             * Evaluate value and return as integer
             * @param var pointer to store integer value
             * @param expr expression
             * @return status of operation
             */
            status_t eval_int(ssize_t *value, const LSPString *expr);

            /**
             * Set integer value
             * @param var variable name
             * @param value value to set
             * @return status of operation
             */
            status_t set_int(const LSPString *var, ssize_t value);
    };
}

#endif /* _UI_UI_BUILDER_H_ */
