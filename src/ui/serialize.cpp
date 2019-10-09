/*
 * serialize.cpp
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ui.h>
#include <ui/serialize.h>


namespace lsp
{
    using namespace tk;

    class lsp_theme_handler: public XMLHandler
    {
        protected:
            LSPTheme        *pTheme;

        public:
            lsp_theme_handler(LSPTheme *theme)
            {
                pTheme      = theme;
            }

            virtual void completed(XMLHandler *child)
            {
                if (child != NULL)
                    delete child;
            }
    };

    class lsp_theme_color_handler: public lsp_theme_handler
    {
        public:
            lsp_theme_color_handler(LSPTheme *theme) : lsp_theme_handler(theme) {}

        public:
            virtual XMLHandler *startElement(const char *name, const char **atts)
            {
                const char *value = findAttribute(atts, "value");
                if (value != NULL)
                    pTheme->add_color(name, value);
                else
                    lsp_error("\"value\" attribute expected for tag <%s>", name);

                return NULL;
            }
    };

    class lsp_theme_body_handler: public lsp_theme_handler
    {
        public:
            lsp_theme_body_handler(LSPTheme *theme) : lsp_theme_handler(theme) {}

        public:
            virtual XMLHandler *startElement(const char *name, const char **atts)
            {
                if (!strcmp(name, "colors"))
                    return new lsp_theme_color_handler(pTheme);

                lsp_error("unexpected tag <%s>", name);
                return NULL;
            }
    };

    class lsp_theme_root_handler: public lsp_theme_handler
    {
        public:
            lsp_theme_root_handler(LSPTheme *theme) : lsp_theme_handler(theme) {}

        public:
            virtual XMLHandler *startElement(const char *name, const char **atts)
            {
                if (!strcmp(name, "theme"))
                    return new lsp_theme_body_handler(pTheme);

                lsp_error("expected root tag <theme>, received: <%s>", name);
                return NULL;
            }

            virtual void completed(XMLHandler *child)
            {
                if (child != NULL)
                    delete child;
            }
    };

    status_t load_theme(tk::LSPTheme *theme, const char *path)
    {
        lsp_theme_root_handler root(theme);
        XMLParser parser;

        status_t res = (parser.parse(path, &root)) ? STATUS_OK : STATUS_UNKNOWN_ERR;
        if (res == STATUS_OK)
            res = theme->after_load();

        return res;
    }
}


