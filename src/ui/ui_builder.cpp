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
#include <data/cstack.h>
#include <errno.h>

namespace lsp
{
    class ui_for_handler: public XMLHandler
    {
        protected:
            enum tag_t
            {
                EVT_TAG_OPEN,
                EVT_TAG_CLOSE,

                EVT_TAG_UNKNOWN = -1
            };

            typedef struct xml_event_t
            {
                tag_t           nEvent;
                char           *sTag;
                cvector<char>   vAttributes;

                inline xml_event_t()
                {
                    nEvent      = EVT_TAG_UNKNOWN,
                    sTag        = NULL;
                }
            } xml_start_element_t;

        private:
            ui_builder             *pBuilder;
            XMLHandler             *pHandler;
            cvector<xml_event_t>    vEvents;
            size_t                  nLevel;
            char                   *pID;
            ssize_t                 nFirst;
            ssize_t                 nLast;
            ssize_t                 nStep;

        protected:
            char *get_value(const char *buf, size_t len)
            {
                // Form the string
                char *key       = lsp_strbuild(buf, len);
                if ((key == NULL) || (strcmp(key, pID) != 0))
                    return NULL;

                // Get value of the variable
                char *value     = NULL;
                ui_variable_t *var = pBuilder->get_variable(key);
                if (var != NULL)
                {
                    char tmp_buf[64];
                    snprintf(tmp_buf, sizeof(tmp_buf), "%lld", (long long)(var->nValue));
                    value   = lsp_strdup(tmp_buf);
                }

                // Drop key buffer
                lsp_free(key);

                // Return value
                return value;
            }

            char *process_string(const char *str)
            {
                if (str == NULL)
                    return NULL;

                // Initialize buffer
                buffer_t sbuf;
                size_t len          = strlen(str);
                if (!init_buf(&sbuf, len))
                    return NULL;

                // Initialize parser
                const char *end     = &str[len];
                while (str != end)
                {
                    // Find the special token
                    const char *token = strstr(str, "${");
                    if (token != NULL)
                    {
                        // Append characters before token if presented
                        append_buf(&sbuf, str, token - str);

                        // Get token body
                        token  += 2;
                        const char *token_end = strchr(token, '}');
                        if ((token_end != NULL) && (token_end > token))
                        {
                            char *value     = get_value(token, token_end - token);
                            if (value != NULL)
                            {
                                append_buf(&sbuf, value);
                                lsp_free(value);
                            }
                            else
                            {
                                append_buf(&sbuf, "${", 2);
                                append_buf(&sbuf, token, token_end - token + 1); // With ending character
                            }
                            str     = token_end + 1;
                        }
                        else
                        {
                            append_buf(&sbuf, "${", 2);
                            str     = token;
                        }
                    }
                    else
                    {
                        append_buf(&sbuf, str);
                        str     = end;
                    }
                }

                return release_buf(&sbuf);
            }

            char **process_attributes(cvector<char> &atts)
            {
                size_t count    = atts.size();
                char **result   = lsp_tmalloc(char *, count + 1);
                if (result == NULL)
                    return result;

                for (size_t i=0; i<count; ++i)
                {
                    // Process only attribute values (strings with odd index)
                    if (i & 1)
                        result[i]   = process_string(atts[i]);
                    else
                        result[i]   = lsp_strdup(atts[i]);
                }

                // The final pointer points to NULL
                result[count] = NULL;

                return result;
            }

            static inline void drop_attributes(char **atts)
            {
                for (char **ptr = atts; (ptr != NULL) && (*ptr != NULL); ++ptr)
                {
                    lsp_free(*ptr);
                    *ptr     = NULL;
                }
                lsp_free(atts);
            }

            static inline void drop_event(xml_event_t *evt)
            {
                if (evt == NULL)
                    return;

                if (evt->sTag != NULL)
                {
                    lsp_free(evt->sTag);
                    evt->sTag   = NULL;
                }

                for (size_t i=0; i<evt->vAttributes.size(); ++i)
                    lsp_free(evt->vAttributes[i]);
                evt->vAttributes.clear();

                delete evt;
            }

            void cleanup()
            {
                for (size_t i=0; i<vEvents.size(); ++i)
                {
                    xml_event_t *evt = vEvents[i];
                    if (evt != NULL)
                        drop_event(evt);
                }
                vEvents.clear();
            }

        public:
            ui_for_handler(ui_builder *bld, XMLHandler *handler, const char **atts)
            {
                pBuilder            = bld;
                pHandler            = handler;
                nLevel              = 0;
                pID                 = NULL;
                nFirst              = 0;
                nLast               = 0;
                nStep               = 1;
                bool increment_set = false;

                while (*atts != NULL)
                {
                    const char *a_name      = *atts++;
                    const char *a_value     = *atts++;

                    if ((a_name != NULL) && (a_value != NULL))
                    {
                        if (strcmp(a_name, "id") == 0)
                        {
                            if (pID != NULL)
                                lsp_free(pID);
                            pID = lsp_strdup(a_value);
                        }
                        else if (strcmp(a_name, "first") == 0)
                        {
                            errno = 0;
                            ssize_t tmp = strtol(a_value, NULL, 10);
                            if (errno == 0)
                                nFirst      = tmp;
                        }
                        else if (strcmp(a_name, "last") == 0)
                        {
                            errno = 0;
                            ssize_t tmp = strtol(a_value, NULL, 10);
                            if (errno == 0)
                                nLast       = tmp;
                        }
                        else if (strcmp(a_name, "step") == 0)
                        {
                            errno = 0;
                            ssize_t tmp = strtol(a_value, NULL, 10);
                            if (errno == 0)
                            {
                                nStep  = tmp;
                                increment_set   = true;
                            }
                        }
                    }
                }

                // Set-up increment
                if (!increment_set)
                    nStep       = (nFirst <= nLast) ? 1 : -1;
            }

            virtual ~ui_for_handler()
            {
                if (pID != NULL)
                {
                    lsp_free(pID);
                    pID     = NULL;
                }
                cleanup();
            }

        public:
            virtual XMLHandler *startElement(const char *name, const char **atts)
            {
                // Allocate event
                xml_event_t *evt        = new xml_event_t;
                if (evt == NULL)
                    return NULL;

                evt->nEvent             = EVT_TAG_OPEN;
                evt->sTag               = lsp_strdup(name);
                if (evt->sTag == NULL)
                {
                    drop_event(evt);
                    return NULL;
                }

                for (; (atts != NULL) && ((*atts) != NULL); atts++)
                {
                    char *att           = lsp_strdup(*atts);
                    if (att == NULL)
                    {
                        drop_event(evt);
                        return NULL;
                    }
                    evt->vAttributes.add(att);
                }

                // Add to list
                if (!vEvents.add(evt))
                    drop_event(evt);

                // Increment level
                nLevel ++;

                return this;
            }

            virtual void endElement(const char *name)
            {
                // Allocate event
                xml_event_t *evt        = new xml_event_t;

                evt->nEvent             = EVT_TAG_CLOSE;
                evt->sTag               = lsp_strdup(name);
                if (evt->sTag == NULL)
                {
                    drop_event(evt);
                    return;
                }

                // Add to list
                if (!vEvents.add(evt))
                    drop_event(evt);
            }

            virtual void quit()
            {
                if ((nLevel--) != 0)
                    return;
                if (pID == NULL)
                    return;

                // Initialize stack
                for (ssize_t value=nFirst; value<=nLast; value+=nStep)
                {
                    // Initialize variable with value
                    ui_variable_t *var      = pBuilder->get_variable(pID);
                    if (var == NULL)
                        break;
                    var->nValue             = value;

                    // Initialize call stack
                    cstack<XMLHandler> stack;
                    if (!stack.push(pHandler))
                        break;

                    // Deliver all events to child
                    for (size_t i=0; i<vEvents.size(); ++i)
                    {
                        // Compile event (pass variable values)
                        xml_event_t *evt        = vEvents[i];
                        if (evt == NULL)
                            break;

                        // Handle event
                        switch (evt->nEvent)
                        {
                            case EVT_TAG_OPEN:
                            {
                                char **atts                 = process_attributes(evt->vAttributes);
                                XMLHandler *handler         = stack.top();
                                XMLHandler *next            = handler->startElement(evt->sTag, const_cast<const char **>(atts));
                                if (next != NULL)
                                    next->enter();
                                stack.push(next);

                                drop_attributes(atts);
                                break;
                            }

                            case EVT_TAG_CLOSE:
                            {
                                XMLHandler  *handler        = stack.pop();
                                XMLHandler  *prev           = stack.top();
                                if (handler != NULL)
                                    handler->quit();
                                if (prev != NULL)
                                {
                                    prev->completed(handler);
                                    prev->endElement(evt->sTag);
                                }
                                break;
                            }

                            default:
                                break;
                        }
                    }
                }

                cleanup();
            }
    };

    class ui_widget_handler: public XMLHandler
    {
        private:
            ui_builder         *pBuilder;
            CtlWidget          *pWidget;
            ui_widget_handler  *pChild;
            XMLHandler         *pSpecial;

        public:
            ui_widget_handler(ui_builder *bld, CtlWidget *widget)
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
            virtual void enter()
            {
                pWidget->begin();
            }

            virtual XMLHandler *startElement(const char *name, const char **atts)
            {
                // Check for special conditions
                if (strstr(name, "ui:") != NULL)
                {
                    if (strcmp(name, "ui:for") == 0)
                        pSpecial    = new ui_for_handler(pBuilder, this, atts);

                    return pSpecial;
                }

                // Get UI
                plugin_ui *ui               = pBuilder->get_ui();

                // Create widget
                CtlWidget *widget           = ui->create_widget(name);
                if (widget == NULL)
                    return NULL;
                widget->init();

                // Initialize pWidget parameters
                while (*atts != NULL)
                {
                    const char *a_name      = *atts++;
                    const char *a_value     = *atts++;
                    if ((a_name != NULL) && (a_value != NULL))
                        widget->set(a_name, a_value);
                }

                // Create handler
                pChild = new ui_widget_handler(pBuilder, widget);
                return pChild;
            }

            virtual void quit()
            {
                pWidget->end();
            }

            virtual void completed(XMLHandler *child)
            {
                if ((child == pChild) && (pChild != NULL))
                {
                    if ((pWidget != NULL) && (pChild->pWidget != NULL))
                    {
                        LSPWidget *w = pChild->pWidget->widget();
                        if (w != NULL)
                            pWidget->add(w);
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
            }
    };

    class ui_root_handler: public XMLHandler
    {
        private:
            ui_builder         *pBuilder;
            ui_widget_handler  *pChild;

        public:
            ui_root_handler(ui_builder *bld)
            {
                pBuilder    = bld;
                pChild      = NULL;
            }

            virtual ~ui_root_handler()
            {
                if (pChild != NULL)
                    pChild = NULL;
            }

        public:
            virtual XMLHandler *startElement(const char *name, const char **atts)
            {
                CtlWidget *widget = NULL;

                const char *root_tag = widget_ctl(WC_PLUGIN);
                if (!strcmp(name, root_tag))
                {
                    // Get UI
                    plugin_ui *ui           = pBuilder->get_ui();

                    // Create widget
                    widget                  = ui->create_widget(name);
                    if (widget == NULL)
                        return NULL;
                    widget->init();

                    // Initialize widget parameters
                    while (*atts != NULL)
                    {
                        const char *a_name      = *atts++;
                        const char *a_value     = *atts++;
                        if ((a_name != NULL) && (a_value != NULL))
                            widget->set(a_name, a_value);
                    }

                    // Create handler
                    pChild = new ui_widget_handler(pBuilder, widget);
                    return pChild;
                }

                lsp_error("expected root tag <%s>", root_tag);
                return NULL;
            }
    };

    ui_builder::ui_builder(plugin_ui *ui)
    {
        pUI         = ui;
    }

    ui_builder::~ui_builder()
    {
        for (size_t i=0; i<vVars.size(); ++i)
        {
            ui_variable_t *var  = vVars[i];
            if (var != NULL)
            {
                lsp_free(var->sName);
                delete var;
            }
        }
        vVars.clear();
    }

    ui_variable_t *ui_builder::get_variable(const char *id)
    {
        // Find variable
        for (size_t i=0; i<vVars.size(); ++i)
        {
            ui_variable_t *var  = vVars[i];
            if (var == NULL)
                continue;
            if (var->sName == NULL)
                continue;
            if (strcmp(id, var->sName) == 0)
                return var;
        }

        // Allocate new variable and add it to list
        ui_variable_t *var  = new ui_variable_t;
        if (var == NULL)
            return NULL;
        var->sName          = lsp_strdup(id);
        if (var->sName == NULL)
        {
            delete var;
            return NULL;
        }
        if (!vVars.add(var))
        {
            lsp_free(var->sName);
            delete var;
            return NULL;
        }

        return var;
    }

    bool ui_builder::build(const char *path)
    {
        ui_root_handler root(this);
        XMLParser parser;

        return parser.parse(path, &root);
    }
}
