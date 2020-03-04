/*
 * LSPTheme.cpp
 *
 *  Created on: 19 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <ui/common.h>

namespace lsp
{
    namespace tk
    {
        LSPTheme::LSPTheme()
        {
            pFont       = NULL;
        }

        void LSPTheme::init(LSPDisplay *dpy)
        {
            pFont = new LSPFont(dpy);
            if (pFont == NULL)
                return;

            pFont->set_name("Sans");
            pFont->set_size(12);
            pFont->set_bold(false);
            pFont->set_italic(false);
            pFont->color()->set_rgb(1.0f, 1.0f, 1.0f);

            // Initialize root style
            sStyle.init();
            ui_atom_t lang = dpy->atom_id("language");
            if (lang >= 0)
                sStyle.set_string(lang, "us");
            sBgColor.bind(dpy, &sStyle, "bg_color");
            sGlassColor.bind(dpy, &sStyle, "glass_color");
            sHoleColor.bind(dpy, &sStyle, "hole_color");
            sBrightness.bind(dpy, &sStyle, "brightness");
        }

        status_t LSPTheme::after_load()
        {
            font_parameters_t fp;

            // Initialize default root style settings
            get_color(C_BACKGROUND, &sBgColor);
            get_color(C_GLASS, &sGlassColor);
            get_color(C_HOLE, &sHoleColor);
            sBrightness.set(1.0f); // Normal brightness

            get_color(C_LABEL_TEXT, pFont->color());
            pFont->get_parameters(&fp); // Cache font parameters for further use

            return STATUS_OK;
        }

        LSPTheme::~LSPTheme()
        {
            // Destroy style bindings
            sBgColor.unbind();
            sGlassColor.unbind();
            sHoleColor.unbind();
            sBrightness.unbind();

            // Destroy root style
            sStyle.destroy();

            // Destroy color names
            size_t n = sColors.size();

            for (size_t i=0; i<n; ++i)
            {
                color_data_t *cd = sColors.at(i);
                if (cd->name != NULL)
                {
                    free(cd->name);
                    cd->name    = NULL;
                }
            }

            sColors.flush();

            // Destroy font
            if (pFont != NULL)
            {
                delete pFont;
                pFont = NULL;
            }
        }

        bool LSPTheme::find_color(const char *name, Color *dst)
        {
            size_t n = sColors.size();

            for (size_t i=0; i<n; ++i)
            {
                color_data_t *cd = sColors.at(i);

                if (cd->name == NULL)
                    continue;

                if (!strcmp(cd->name, name))
                {
                    dst->copy(cd->color);
                    return true;
                }
            }

            return false;
        }

        ssize_t LSPTheme::read_component(const char *text, size_t digits)
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

        bool LSPTheme::get_components(const char *text, float &c1, float &c2, float &c3)
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
            ssize_t v1 = read_component(text, digits);
            if (v1 < 0)
                return false;
            text += digits;

            ssize_t v2 = read_component(text, digits);
            if (v2 < 0)
                return false;
            text += digits;

            ssize_t v3 = read_component(text, digits);
            if (v3 < 0)
                return false;

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

        bool LSPTheme::parse_rgb(const char *text, Color *dst)
        {
            float r, g, b;
            if (!get_components(text, r, g, b))
                return false;
            dst->set_rgb(r, g, b);
            return true;
        }
        
        bool LSPTheme::parse_hsl(const char *text, Color *dst)
        {
            float h, s, l;
            if (!get_components(text, h, s, l))
                return false;
            dst->set_hsl(h, s, l);
            return true;
        }

        bool LSPTheme::add_color(const char *name, const char *value)
        {
            lsp_trace("add_color name=%s value=%s", name, value);
            // Skip spaces
            while ((*value == ' ') && (*value != '\0'))
                value++;

            Color color;

            switch (*value)
            {
                case '@': // HSL color
                    if (!parse_hsl(++value, &color))
                        return false;
                    break;
                case '#': // RGB color
                    if (!parse_rgb(++value, &color))
                        return false;
                    break;
                default:
                    return false;
            }
            return add_color(name, color);
        }

        bool LSPTheme::add_color(const char *name, const Color *color)
        {
            // Create new record
            color_data_t *c  = sColors.append();
            if (c == NULL)
                return false;

            // Copy color name
            c->name     = lsp_strdup(name);
            if (c->name == NULL)
            {
                sColors.remove_last();
                return false;
            }

            // Copy color
            c->color.copy(*color);

            return true;
        }

        bool LSPTheme::get_color(const char *name, Color *dst)
        {
            if (name != NULL)
            {
                // Skip spaces
                while ((*name == ' ') && (*name != '\0'))
                    name++;

                switch (*name)
                {
                    case '@': // HSL color
                        if (parse_hsl(++name, dst))
                            return true;
                        break;
                    case '#': // RGB color
                        if (parse_rgb(++name, dst))
                            return true;
                        break;

                    case '\0':
                        break;

                    default:
                        if (find_color(name, dst))
                            return true;
                        break;
                }
            }

            if (!find_color("default", dst))
                dst->set_rgb(0.0f, 0.0f, 0.0f);
            return false;
        }

        bool LSPTheme::add_color(const char *name, const Color &color)
        {
            return add_color(name, &color);
        }

        bool LSPTheme::get_color(const char *name, Color &dst)
        {
            return get_color(name, &dst);
        }

        bool LSPTheme::get_color(color_t color, Color *dst)
        {
            return get_color(color_name(color), dst);
        }

        bool LSPTheme::get_color(color_t color, Color &dst)
        {
            return get_color(color, &dst);
        }

        bool LSPTheme::get_color(color_t color, LSPColor &dst)
        {
            Color cl;
            bool res = get_color(color, cl);
            dst.copy(cl);
            return res;
        }

        bool LSPTheme::get_color(color_t color, LSPColor *dst)
        {
            Color cl;
            bool res = get_color(color, cl);
            dst->copy(cl);
            return res;
        }
    
    } /* namespace tk */
} /* namespace lsp */
