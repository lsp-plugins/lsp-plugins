/*
 * Theme.cpp
 *
 *  Created on: 09 нояб. 2015 г.
 *      Author: sadko
 */

#include <ui/ui.h>

#include <string.h>
#include <stdlib.h>
#include <libconfig.h>

namespace lsp
{
    class theme_handler: public XMLHandler
    {
        protected:
            Theme          *pTheme;

        public:
            theme_handler(Theme *theme)
            {
                pTheme      = theme;
            }

            virtual void completed(XMLHandler *child)
            {
                if (child != NULL)
                    delete child;
            }
    };

    class theme_color_handler: public theme_handler
    {
        public:
            theme_color_handler(Theme *theme) : theme_handler(theme) {}

        public:
            virtual XMLHandler *startElement(const char *name, const char **atts)
            {
                const char *value = findAttribute(atts, "value");
                if (value != NULL)
                    pTheme->addColor(name, value);
                else
                    lsp_error("\"value\" attribute expected for tag <%s>", name);

                return NULL;
            }
    };

    class theme_body_handler: public theme_handler
    {
        public:
            theme_body_handler(Theme *theme) : theme_handler(theme) {}

        public:
            virtual XMLHandler *startElement(const char *name, const char **atts)
            {
                if (!strcmp(name, "colors"))
                    return new theme_color_handler(pTheme);

                lsp_error("unexpected tag <%s>", name);
                return NULL;
            }
    };

    class theme_root_handler: public theme_handler
    {
        public:
            theme_root_handler(Theme *theme) : theme_handler(theme) {}

        public:
            virtual XMLHandler *startElement(const char *name, const char **atts)
            {
                if (!strcmp(name, "theme"))
                    return new theme_body_handler(pTheme);

                lsp_error("expected root tag <theme>");
                return NULL;
            }

            virtual void completed(XMLHandler *child)
            {
                if (child != NULL)
                    delete child;
            }
    };

    Theme::~Theme()
    {
        while (pColors != NULL)
        {
            color_data_t *next   = pColors->next;
            if (pColors->name != NULL)
            {
                free(pColors->name);
                pColors->name   = NULL;
            }

            delete pColors;
            pColors         = next;
        }
    }

    bool Theme::findColor(const char *name, Color *dst)
    {
        color_data_t *p      = pColors;
        while (p != NULL)
        {
            if (p->name != NULL)
            {
                if (!strcmp(p->name, name))
                {
                    dst->copy(p->color);
                    return true;
                }
            }
            p   = p->next;
        }

        return false;
    }

    ssize_t Theme::readComponent(const char *text, size_t digits)
    {
        ssize_t result = 0;
        while (digits--)
        {
            result <<= 4;
            char c = *(text++);

            if ((c >= '0') && (c <= '9'))
                result += c - '0';
            else if ((c >= 'a') && (c <= 'f'))
                result += c - 'a' + 10;
            else if ((c >= 'A') && (c <= 'F'))
                result += c - 'A' + 10;
            else
                return -1;
        }

        return result;
    }

    bool Theme::getComponents(const char *text, float &c1, float &c2, float &c3)
    {
        // Validate length of the text
        size_t len = strlen(text);
        if ((len % 3) != 0)
            return false;

        // Validate number of color digits
        size_t digits = len / 3;
        if (digits <= 0)
            return false;

        // Get components
        ssize_t v1 = readComponent(text, digits);
        if (v1 < 0) return false;
        text += digits;

        ssize_t v2 = readComponent(text, digits);
        if (v2 < 0) return false;
        text += digits;

        ssize_t v3 = readComponent(text, digits);
        if (v3 < 0) return false;

        // Calculate multiplier
        size_t max = 1;
        while (digits--)
            max <<= 4;
        float multiply = 1.0f / (max - 1);

        // Store result
        c1      = v1 * multiply;
        c2      = v2 * multiply;
        c3      = v3 * multiply;

        return true;
    }

    bool Theme::parseRGBColor(const char *text, Color *dst)
    {
        float r, g, b;
        if (!getComponents(text, r, g, b))
            return false;
        dst->set_rgb(r, g, b);
        return true;
    }

    bool Theme::parseHSLColor(const char *text, Color *dst)
    {
        float h, s, l;
        if (!getComponents(text, h, s, l))
            return false;
        dst->set_hsl(h, s, l);
        return true;
    }

    bool Theme::getColor(const char *name, Color *dst)
    {
        if (name != NULL)
        {
            // Skip spaces
            while ((*name == ' ') && (*name != '\0'))
                name++;

            switch (*name)
            {
                case '@': // HSL color
                    if (parseHSLColor(++name, dst))
                        return true;
                    break;
                case '#': // RGB color
                    if (parseRGBColor(++name, dst))
                        return true;
                    break;

                case '\0':
                    break;

                default:
                    if (findColor(name, dst))
                        return true;
                    break;
            }
        }

        if (!findColor("default", dst))
            dst->set_rgb(0.0f, 0.0f, 0.0f);
        return false;
    }

    bool Theme::load(const char *path)
    {
        theme_root_handler root(this);
        XMLParser parser;

        return parser.parse(path, &root);
    }

    /** Add color to theme
     *
     * @param name
     * @param value
     * @return
     */
    bool Theme::addColor(const char *name, const char *value)
    {
        lsp_trace("add_color name=%s value=%s", name, value);
        // Skip spaces
        while ((*value == ' ') && (*value != '\0'))
            value++;

        Color color;

        switch (*value)
        {
            case '@': // HSL color
                if (!parseHSLColor(++value, &color))
                    return false;
                break;
            case '#': // RGB color
                if (!parseRGBColor(++value, &color))
                    return false;
                break;
            default:
                return false;
        }
        return addColor(name, color);
    }

    /** Add color to theme
     *
     * @param name color name
     * @param color color value
     * @return true if added
     */
    bool Theme::addColor(const char *name, const Color *color)
    {
        color_data_t *c  = new color_data_t;
        if (c == NULL)
            return false;

        // Copy color name
        c->name     = strdup(name);
        if (c->name == NULL)
        {
            delete c;
            return false;
        }

        // Copy color
        c->color.copy(*color);

        // Add to list of colors
        c->next     = pColors;
        pColors     = c;

        return true;
    }

} /* namespace lsp */
