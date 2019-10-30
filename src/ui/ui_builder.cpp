/*
 * ui_builder.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <string.h>
#include <core/debug.h>
#include <core/alloc.h>
#include <core/calc/Expression.h>
#include <core/calc/Variables.h>
#include <errno.h>

#include <ui/XMLHandler.h>

namespace lsp
{
    class ui_recording_handler: public XMLPlaybackNode
    {
        private:
            ui_recording_handler & operator = (const ui_recording_handler &src);

        protected:
            ui_builder             *pBuilder;

        public:
            explicit ui_recording_handler(ui_builder *bld, XMLNode *handler):
                XMLPlaybackNode(handler)
            {
                pBuilder    = bld;
            }

            virtual ~ui_recording_handler()
            {
                pBuilder    = NULL;
            }
    };

    class ui_for_handler: public ui_recording_handler
    {
        private:
            LSPString      *pID;
            ssize_t         nFirst;
            ssize_t         nLast;
            ssize_t         nStep;

        public:
            explicit ui_for_handler(ui_builder *bld, XMLNode *handler) : ui_recording_handler(bld, handler)
            {
                pID         = NULL;
                nFirst      = 0;
                nLast       = 0;
                nStep       = 1;
            }

            virtual status_t init(const LSPString * const *atts)
            {
                bool increment_set = false;
                status_t res;

                for ( ; *atts != NULL; atts += 2)
                {
                    const LSPString *name   = atts[0];
                    const LSPString *value  = atts[1];

                    if ((name == NULL) || (value == NULL))
                        continue;

                    if (name->equals_ascii("id"))
                    {
                        if (pID != NULL)
                            return STATUS_CORRUPTED;
                        if ((pID = value->clone()) == NULL)
                            return STATUS_NO_MEM;
                    }
                    else if (name->equals_ascii("first"))
                    {
                        if ((res = pBuilder->eval_int(&nFirst, value)) != STATUS_OK)
                            return res;
                    }
                    else if (name->equals_ascii("last"))
                    {
                        if ((res = pBuilder->eval_int(&nLast, value)) != STATUS_OK)
                            return res;
                    }
                    else if (name->equals_ascii("step"))
                    {
                        if ((res = pBuilder->eval_int(&nStep, value)) != STATUS_OK)
                            return res;
                        increment_set = true;
                    }
                }

                // Compute increment
                if (!increment_set)
                    nStep       = (nFirst <= nLast) ? 1 : -1;

                return STATUS_OK;
            }

            virtual ~ui_for_handler()
            {
                if (pID != NULL)
                {
                    delete pID;
                    pID     = NULL;
                }
            }

        public:
            virtual status_t execute()
            {
                status_t res;
                if (pID == NULL)
                    return STATUS_OK;

                // Create new scope
                if ((res = pBuilder->push_scope()) != STATUS_OK)
                    return res;

                // Perform a loop
                for (ssize_t value = nFirst; value <= nLast; value += nStep)
                {
                    if ((res = pBuilder->set_int(pID, value)) != STATUS_OK)
                        return res;
                    if ((res = playback()) != STATUS_OK)
                        return res;
                }

                // Pop scope and return
                return pBuilder->pop_scope();
            }
    };

    class ui_widget_handler: public XMLNode
    {
        private:
            ui_widget_handler & operator = (const ui_widget_handler *);

        private:
            ui_builder             *pBuilder;
            CtlWidget              *pWidget;
            ui_widget_handler      *pChild;
            ui_recording_handler   *pSpecial;

        public:
            explicit ui_widget_handler(ui_builder *bld, CtlWidget *widget)
            {
                pBuilder    = bld;
                pWidget     = widget;
                pChild      = NULL;
                pSpecial    = NULL;
            }

            virtual ~ui_widget_handler()
            {
                if (pChild != NULL)
                    pChild = NULL;
            }

        public:
            virtual status_t enter()
            {
                pWidget->begin();
                return STATUS_OK;
            }

            virtual status_t start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts)
            {
                status_t res;

                // Check for special conditions
                if (name->starts_with_ascii("ui:"))
                {
                    // Build special handler
                    if (name->equals_ascii("ui:for"))
                        pSpecial    = new ui_for_handler(pBuilder, this);

                    // Is there any instance?
                    if (pSpecial == NULL)
                        return STATUS_CORRUPTED;

                    if ((res = pSpecial->init(atts)) != STATUS_OK)
                        return res;
                    *child = pSpecial;
                    return STATUS_OK;
                }

                // Get UI
                plugin_ui *ui               = pBuilder->get_ui();

                // Create widget
                CtlWidget *widget           = ui->create_widget(name->get_utf8());
                if (widget == NULL)
                    return STATUS_OK;       // No handler
                widget->init();

                // Initialize pWidget parameters
                for ( ; *atts != NULL; atts += 2)
                {
                    const char *a_name      = atts[0]->get_utf8();
                    const char *a_value     = atts[1]->get_utf8();
                    if ((a_name != NULL) && (a_value != NULL))
                        widget->set(a_name, a_value);
                }

                // Create handler
                *child = pChild = new ui_widget_handler(pBuilder, widget);
                return (pChild != NULL) ? STATUS_OK : STATUS_NO_MEM;
            }

            virtual status_t quit()
            {
                pWidget->end();
                return STATUS_OK;
            }

            virtual status_t completed(XMLNode *child)
            {
                status_t res = STATUS_OK;
                if ((child == pChild) && (pChild != NULL))
                {
                    if ((pWidget != NULL) && (pChild->pWidget != NULL))
                    {
                        CtlWidget *w = pChild->pWidget;
                        if (w != NULL)
                            res = pWidget->add(w);
                    }

                    // Remove child
                    delete pChild;
                    pChild  = NULL;
                }
                else if ((child == pSpecial) && (pSpecial != NULL))
                {
                    delete pSpecial;
                    pSpecial = NULL;
                }
                return res;
            }
    };

    class ui_root_handler: public XMLNode
    {
        private:
            ui_root_handler & operator = (const ui_root_handler &);

        private:
            ui_builder         *pBuilder;
            ui_widget_handler  *pChild;

        public:
            explicit ui_root_handler(ui_builder *bld)
            {
                pBuilder    = bld;
                pChild      = NULL;
            }

            virtual ~ui_root_handler()
            {
                if (pChild != NULL)
                {
                    delete pChild;
                    pChild = NULL;
                }
            }

        public:
            virtual status_t start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts)
            {
                // Check that root tag is valid
                const char *root_tag = widget_ctl(WC_PLUGIN);
                if (!name->equals_ascii(root_tag))
                {
                    lsp_error("expected root element <%s>", root_tag);
                    return STATUS_CORRUPTED;
                }

                // Get UI
                plugin_ui *ui           = pBuilder->get_ui();

                // Create widget
                CtlWidget *widget       = ui->create_widget(name->get_utf8());
                if (widget == NULL)
                    return STATUS_OK;       // No handler
                widget->init();

                // Initialize widget parameters
                for ( ; *atts != NULL; atts += 2)
                {
                    const char *a_name      = atts[0]->get_utf8();
                    const char *a_value     = atts[1]->get_utf8();
                    if ((a_name != NULL) && (a_value != NULL))
                        widget->set(a_name, a_value);
                }

                // Create handler
                *child = pChild = new ui_widget_handler(pBuilder, widget);
                return (pChild != NULL) ? STATUS_OK : STATUS_NO_MEM;
            }
    };

    //-------------------------------------------------------------------------
    // UI Builder implementation
    ui_builder::ui_builder(plugin_ui *ui)
    {
        pUI         = ui;
    }

    ui_builder::~ui_builder()
    {
        for (size_t i=0, n=vStack.size(); i<n; ++i)
        {
            calc::Resolver *r = vStack.at(i);
            if (r != NULL)
                delete r;
        }
        vStack.flush();
    }

    // Evaluate
    status_t ui_builder::evaluate(calc::value_t *value, const LSPString *expr)
    {
        status_t res;
        calc::Expression e;

        // Parse expression
        if ((res = e.parse(expr, calc::Expression::FLAG_STRING)) != STATUS_OK)
            return res;
        e.set_resolver(vars());

        // Evaluate expression
        return e.evaluate(value);
    }

    status_t ui_builder::push_scope()
    {
        // Create variables
        calc::Variables *v = new calc::Variables();
        if (v == NULL)
            return STATUS_NO_MEM;
        if (!vStack.push(v))
        {
            delete v;
            return STATUS_NO_MEM;
        }

        // Bind resolver and quit
        v->set_resolver(vars());
        return STATUS_OK;
    }

    status_t ui_builder::pop_scope()
    {
        calc::Variables *r = NULL;
        if (!vStack.pop(&r))
            return STATUS_BAD_STATE;
        if (r != NULL)
            delete r;
        return STATUS_OK;
    }

    status_t ui_builder::eval_string(LSPString *value, const LSPString *expr)
    {
        calc::value_t v;
        status_t res = evaluate(&v, expr);
        if (res != STATUS_OK)
            return res;
        else if (v.type != calc::VT_STRING)
            res     = STATUS_BAD_TYPE;
        else
            value->swap(v.v_str);
        destroy_value(&v);
        return res;
    }

    status_t ui_builder::eval_int(ssize_t *value, const LSPString *expr)
    {
        LSPString tmp;
        status_t res = eval_string(&tmp, expr);
        if (res != STATUS_OK)
            return res;

        // Parse string as integer value
        errno = 0;
        char *eptr = NULL;
        const char *p = tmp.get_utf8();
        long v = ::strtol(p, &eptr, 10);
        if ((errno != 0) || (eptr == NULL) || (*eptr != '\0'))
            return STATUS_INVALID_VALUE;

        // Store value
        *value = v;
        return STATUS_OK;
    }

    status_t ui_builder::set_int(const LSPString *var, ssize_t value)
    {
        calc::Variables *r = vars();
        return (r != NULL) ? r->set_int(var, value) : STATUS_BAD_STATE;
    }

    status_t ui_builder::build(const LSPString *path)
    {
        ui_root_handler root(this);
        XMLHandler handler;
        return handler.parse(path, &root);
    }

    status_t ui_builder::build(const char *path)
    {
        ui_root_handler root(this);
        XMLHandler handler;
        return handler.parse(path, &root);
    }
}
