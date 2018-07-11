/*
 * ui_builder.h
 *
 *  Created on: 21 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <string.h>
#include <core/debug.h>

namespace lsp
{
    class ui_widget_handler: public XMLHandler
    {
        private:
            plugin_ui      *pUI;
            IWidget        *pWidget;
            ui_widget_handler *pChild;

        public:
            ui_widget_handler(plugin_ui *ui, IWidget *widget)
            {
                pUI         = ui;
                pWidget     = widget;
                pChild      = NULL;
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
                IWidget *widget              = pUI->createWidget(name);
                if (widget == NULL)
                    return NULL;

                // Initialize pWidget parameters
                while (*atts != NULL)
                {
                    const char *a_name      = *atts++;
                    const char *a_value     = *atts++;
                    if ((a_name != NULL) && (a_value != NULL))
                        widget->set(a_name, a_value);
                }

                // Create handler
                pChild = new ui_widget_handler(pUI, widget);
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
                        pWidget->add(pChild->pWidget);

                    // Remove child
                    delete pChild;
                    pChild = NULL;
                }
            }
    };

    class ui_root_handler: public XMLHandler
    {
        private:
            plugin_ui      *pUI;
            ui_widget_handler *pChild;

        public:
            ui_root_handler(plugin_ui *ui)
            {
                pUI         = ui;
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
                IWidget *widget = NULL;

                const char *root_tag = widget_type(W_PLUGIN);
                if (!strcmp(name, root_tag))
                {
                    // Create widget
                    widget              = pUI->createWidget(name);
                    if (widget == NULL)
                        return NULL;

                    // Initialize widget parameters
                    while (*atts != NULL)
                    {
                        const char *a_name      = *atts++;
                        const char *a_value     = *atts++;
                        if ((a_name != NULL) && (a_value != NULL))
                            widget->set(a_name, a_value);
                    }

                    // Create handler
                    pChild = new ui_widget_handler(pUI, widget);
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
    }

    bool ui_builder::build(const char *path)
    {
        ui_root_handler root(pUI);
        XMLParser parser;

        return parser.parse(path, &root);
    }
}
