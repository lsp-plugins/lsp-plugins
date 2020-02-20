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

        protected:
            status_t iterate(ssize_t value)
            {
                status_t res;
                if ((res = pBuilder->vars()->set_int(pID, value)) != STATUS_OK)
                    return res;
                return playback();
            }

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
                        LSPString tmp;
                        if ((res = pBuilder->eval_string(&tmp, value)) != STATUS_OK)
                            return res;
                        if ((pID = tmp.release()) == NULL)
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
                    else
                    {
                        lsp_error("Unknown attribute: %s", name->get_utf8());
                        return STATUS_CORRUPTED;
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
                if (nFirst <= nLast)
                {
                    for (ssize_t value = nFirst; value <= nLast; value += nStep)
                        if ((res = iterate(value)) != STATUS_OK)
                            break;
                }
                else
                {
                    for (ssize_t value = nFirst; value >= nLast; value += nStep)
                        if ((res = iterate(value)) != STATUS_OK)
                            break;
                }

                // Pop scope and return
                return (res == STATUS_OK) ? pBuilder->pop_scope() : res;
            }
    };

    class ui_attribute_handler: public ui_recording_handler
    {
        private:
            cvector<LSPString>      vAtts;
            size_t                  nLevel;

        public:
            explicit ui_attribute_handler(ui_builder *bld, XMLNode *handler) : ui_recording_handler(bld, handler)
            {
                nLevel = 0;
            }

            virtual ~ui_attribute_handler()
            {
                for (size_t i=0, n=vAtts.size(); i<n; ++i)
                {
                    LSPString *s = vAtts.at(i);
                    if (s != NULL)
                        delete s;
                }
                vAtts.flush();
            }

        public:
            virtual status_t init(const LSPString * const *atts)
            {
                status_t res;
                LSPString *attr;

                // Generate list of appended properties
                for (size_t i=0; *atts != NULL; ++atts, ++i)
                {
                    if (i & 1)
                    {
                        // Evaluate attribute value
                        attr    = new LSPString;
                        if (attr != NULL)
                        {
                            if ((res = pBuilder->eval_string(attr, *atts)) != STATUS_OK)
                            {
                                delete attr;
                                return res;
                            }
                        }
                    }
                    else // Copy attribute name
                        attr     = (*atts)->clone();

                    // Append attribute
                    if (attr == NULL)
                        return STATUS_NO_MEM;
                    else if (!vAtts.add(attr))
                    {
                        delete attr;
                        return STATUS_NO_MEM;
                    }
                }

                return STATUS_OK;
            }

            virtual status_t playback_start_element(xml::IXMLHandler *handler, const LSPString *name, const LSPString * const *atts)
            {
                cvector<LSPString> tmp;

                // Need to override attributes?
                if ((nLevel++) == 0)
                {
                    // Copy attributes
                    for (size_t i=0; atts[i] != NULL; ++i)
                        if (!tmp.add(const_cast<LSPString *>(atts[i])))
                            return STATUS_NO_MEM;

                    // Append unexisting attributes
                    LSPString **vatts = vAtts.get_array();
                    for (size_t i=0, n=vAtts.size(); i<n; i += 2)
                    {
                        LSPString *name   = vatts[i];
                        LSPString *value  = vatts[i+1];

                        // Check for duplicate
                        for (size_t j=0; atts[j] != NULL; j+=2)
                            if (atts[j]->equals(name))
                            {
                                name = NULL;
                                break;
                            }

                        // Append property if it does not exist
                        if (name == NULL)
                            continue;

                        if (!tmp.add(name))
                            return STATUS_NO_MEM;
                        if (!tmp.add(value))
                            return STATUS_NO_MEM;
                    }

                    // Append argument terminator
                    if (!tmp.add(NULL))
                        return STATUS_NO_MEM;

                    // Override properties with our own list
                    atts = tmp.get_array();
                }
                return ui_recording_handler::playback_start_element(handler, name, atts);
            }

            virtual status_t playback_end_element(xml::IXMLHandler *handler, const LSPString *name)
            {
                --nLevel;
                return ui_recording_handler::playback_end_element(handler, name);
            }
    };

    class ui_set_handler: public XMLNode
    {
        private:
            ui_set_handler & operator = (const ui_set_handler &src);

        protected:
            ui_builder             *pBuilder;

        public:
            explicit ui_set_handler(ui_builder *bld)
            {
                pBuilder    = bld;
            }

            virtual ~ui_set_handler()
            {
                pBuilder    = NULL;
            }

        public:
            virtual status_t init(const LSPString * const *atts)
            {
                status_t res;
                size_t flags = 0;
                LSPString v_name;
                calc::value_t v_value;
                calc::init_value(&v_value);

                for ( ; *atts != NULL; atts += 2)
                {
                    const LSPString *name   = atts[0];
                    const LSPString *value  = atts[1];

                    if ((name == NULL) || (value == NULL))
                        continue;

                    if (name->equals_ascii("id"))
                    {
                        if ((res = pBuilder->eval_string(&v_name, value)) != STATUS_OK)
                            return res;
                        flags      |= 1;
                    }
                    else if (name->equals_ascii("value"))
                    {
                        if ((res = pBuilder->evaluate(&v_value, value)) != STATUS_OK)
                            return res;
                        flags      |= 2;
                    }
                    else
                    {
                        lsp_error("Unknown attribute: %s", name->get_utf8());
                        return STATUS_CORRUPTED;
                    }
                }

                if (flags != 3)
                {
                    lsp_error("Not all attributes are set");
                    return STATUS_CORRUPTED;
                }

                // Set variable and destroy value
                res = pBuilder->vars()->set(&v_name, &v_value);
                calc::destroy_value(&v_value);
                return res;
            }
    };

    class ui_if_handler: public XMLNode
    {
        private:
            ui_if_handler & operator = (const ui_if_handler &src);

        protected:
            ui_builder             *pBuilder;
            XMLNode                *pHandler;
            bool                    bPass;

        public:
            explicit ui_if_handler(ui_builder *bld, XMLNode *handler)
            {
                pBuilder    = bld;
                pHandler    = handler;
                bPass       = true;
            }

            virtual ~ui_if_handler()
            {
                pBuilder    = NULL;
                pHandler    = NULL;
            }

        public:
            virtual status_t init(const LSPString * const *atts)
            {
                status_t res;
                bool valid = false;

                for ( ; *atts != NULL; atts += 2)
                {
                    const LSPString *name   = atts[0];
                    const LSPString *value  = atts[1];

                    if ((name == NULL) || (value == NULL))
                        continue;

                    if (name->equals_ascii("test"))
                    {
                        if ((res = pBuilder->eval_bool(&bPass, value)) != STATUS_OK)
                            return res;
                        valid = true;
                    }
                    else
                    {
                        lsp_error("Unknown attribute: %s", name->get_utf8());
                        return STATUS_CORRUPTED;
                    }
                }

                if (!valid)
                {
                    lsp_error("Not all attributes are set");
                    return STATUS_CORRUPTED;
                }

                return STATUS_OK;
            }

            virtual status_t start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts)
            {
                return (bPass) ? pHandler->start_element(child, name, atts) : STATUS_OK;
            }

            virtual status_t end_element(const LSPString *name)
            {
                return (bPass) ? pHandler->end_element(name) : STATUS_OK;
            }

            virtual status_t completed(XMLNode *child)
            {
                return (bPass) ? pHandler->completed(child) : STATUS_OK;
            }

            virtual status_t quit()
            {
                return (bPass) ? pHandler->quit() : STATUS_OK;
            }

            virtual status_t enter()
            {
                return (bPass) ? pHandler->enter() : STATUS_OK;
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
            XMLNode                *pOther;

        public:
            explicit ui_widget_handler(ui_builder *bld, CtlWidget *widget)
            {
                pBuilder    = bld;
                pWidget     = widget;
                pChild      = NULL;
                pSpecial    = NULL;
                pOther      = NULL;
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
                    {
                        if ((pSpecial = new ui_for_handler(pBuilder, this)) == NULL)
                            return STATUS_NO_MEM;
                        if ((res = pSpecial->init(atts)) != STATUS_OK)
                            return res;
                        *child  = pSpecial;
                    }
                    else if (name->equals_ascii("ui:attributes"))
                    {
                        if ((pSpecial = new ui_attribute_handler(pBuilder, this)) == NULL)
                            return STATUS_NO_MEM;
                        if ((res = pSpecial->init(atts)) != STATUS_OK)
                            return res;
                        *child  = pSpecial;
                    }
                    else if (name->equals_ascii("ui:set"))
                    {
                        ui_set_handler *h = new ui_set_handler(pBuilder);
                        if (h == NULL)
                            return STATUS_NO_MEM;
                        if ((res = h->init(atts)) != STATUS_OK)
                            return res;
                        *child  = pOther    = h;
                    }
                    else if (name->equals_ascii("ui:if"))
                    {
                        ui_if_handler *h = new ui_if_handler(pBuilder, this);
                        if (h == NULL)
                            return STATUS_NO_MEM;
                        if ((res = h->init(atts)) != STATUS_OK)
                            return res;
                        *child  = pOther    = h;
                    }
                    else
                        res         = STATUS_CORRUPTED;

                    return res;
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
                    LSPString aname, avalue;
                    if ((res = pBuilder->eval_string(&aname, atts[0])) != STATUS_OK)
                        return res;
                    if ((res = pBuilder->eval_string(&avalue, atts[1])) != STATUS_OK)
                        return res;

                    // Set widget attribute
                    widget->set(aname.get_utf8(), avalue.get_utf8());
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
                    ui_recording_handler * special = pSpecial;
                    pSpecial = NULL;

                    res = special->execute();
                    delete special;
                }
                if ((child == pOther) && (pOther != NULL))
                {
                    delete pOther;
                    pOther = NULL;
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
                status_t res;

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
                    LSPString aname, avalue;
                    if ((res = pBuilder->eval_string(&aname, atts[0])) != STATUS_OK)
                        return res;
                    if ((res = pBuilder->eval_string(&avalue, atts[1])) != STATUS_OK)
                        return res;

                    // Set widget attribute
                    widget->set(aname.get_utf8(), avalue.get_utf8());
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
        {
            lsp_error("Could not parse expression: %s", expr->get_utf8());
            return res;
        }
        e.set_resolver(vars());

        // Evaluate expression
        res = e.evaluate(value);
        if (res != STATUS_OK)
            lsp_error("Could not evaluate expression: %s", expr->get_utf8());

        return res;
    }

    status_t ui_builder::push_scope()
    {
        // Create variables
        calc::Variables *v = new calc::Variables();
        if (v == NULL)
            return STATUS_NO_MEM;

        // Bind resolver, push to stack and quit
        v->set_resolver(vars());
        if (!vStack.push(v))
        {
            delete v;
            return STATUS_NO_MEM;
        }

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

        init_value(&v);
        status_t res = evaluate(&v, expr);
        if (res != STATUS_OK)
            return res;

        if ((res = calc::cast_string(&v)) == STATUS_OK)
        {
            if (v.type == calc::VT_STRING)
                value->swap(v.v_str);
            else
            {
                lsp_error("Evaluation error: bad return type of expression %s", expr->get_utf8());
                res = STATUS_BAD_TYPE;
            }
        }
        destroy_value(&v);
        return res;
    }

    status_t ui_builder::eval_bool(bool *value, const LSPString *expr)
    {
        calc::value_t v;
        init_value(&v);

        status_t res = evaluate(&v, expr);
        if (res != STATUS_OK)
            return res;

        if ((res = calc::cast_bool(&v)) == STATUS_OK)
        {
            if (v.type == calc::VT_BOOL)
                *value  = v.v_bool;
            else
            {
                lsp_error("Evaluation error: bad return type of expression %s", expr->get_utf8());
                res = STATUS_BAD_TYPE;
            }
        }
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
        {
            lsp_error("Evaluation error: bad return type of expression %s", expr->get_utf8());
            return STATUS_INVALID_VALUE;
        }

        // Store value
        *value = v;
        return STATUS_OK;
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
