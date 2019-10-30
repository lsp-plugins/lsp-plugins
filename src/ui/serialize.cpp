/*
 * serialize.cpp
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/serialize.h>
#include <ui/XMLNode.h>
#include <ui/XMLHandler.h>
#include <core/files/xml/PushParser.h>

namespace lsp
{
    using namespace tk;

    class lsp_theme_handler: public XMLNode
    {
        protected:
            LSPTheme        *pTheme;

        public:
            explicit lsp_theme_handler(LSPTheme *theme)
            {
                pTheme      = theme;
            }

            virtual status_t completed(XMLNode *child)
            {
                if (child != NULL)
                    delete child;
                return STATUS_OK;
            }
    };

    class lsp_theme_color_handler: public lsp_theme_handler
    {
        public:
            explicit lsp_theme_color_handler(LSPTheme *theme) : lsp_theme_handler(theme) {}

        public:
            virtual status_t start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts)
            {
                const LSPString *value = find_attribute(atts, "value");
                if (value != NULL)
                {
                    if (!pTheme->add_color(name->get_utf8(), value->get_utf8()))
                        return STATUS_NO_MEM;
                    return STATUS_OK;
                }

                lsp_error("\"value\" attribute expected for element <%s>", name->get_utf8());
                return STATUS_CORRUPTED;
            }
    };

    class lsp_theme_body_handler: public lsp_theme_handler
    {
        public:
            explicit lsp_theme_body_handler(LSPTheme *theme) : lsp_theme_handler(theme) {}

        public:
            virtual status_t start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts)
            {
                if (name->equals_ascii("colors"))
                {
                    if ((*child = new lsp_theme_color_handler(pTheme)) == NULL)
                        return STATUS_NO_MEM;
                    return STATUS_OK;
                }

                lsp_error("unexpected element <%s>", name->get_utf8());
                return STATUS_CORRUPTED;
            }
    };

    class lsp_theme_root_handler: public lsp_theme_handler
    {
        public:
            explicit lsp_theme_root_handler(LSPTheme *theme) : lsp_theme_handler(theme) {}

        public:
            virtual status_t start_element(XMLNode **child, const LSPString *name, const LSPString * const *atts)
            {
                if (name->equals_ascii("theme"))
                {
                    if ((*child = new lsp_theme_body_handler(pTheme)) == NULL)
                        return STATUS_NO_MEM;
                    return STATUS_OK;
                }

                lsp_error("expected root element <theme>, received: <%s>", name->get_utf8());
                return STATUS_CORRUPTED;
            }

            virtual status_t completed(XMLNode *child)
            {
                if (child != NULL)
                    delete child;
                return STATUS_OK;
            }
    };

    status_t load_theme(tk::LSPTheme *theme, const LSPString *uri)
    {
        lsp_theme_root_handler root(theme);
        XMLHandler handler;

        status_t res = handler.parse(uri, &root);
        if (res == STATUS_OK)
            res = theme->after_load();
        return res;
    }

    status_t load_theme(tk::LSPTheme *theme, const char *uri)
    {
        lsp_theme_root_handler root(theme);
        XMLHandler handler;
        status_t res = handler.parse(uri, &root);
        if (res == STATUS_OK)
            res = theme->after_load();
        return res;
    }
}


