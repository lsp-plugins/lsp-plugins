/*
 * IWidgetCtl.cpp
 *
 *  Created on: 15 июн. 2017 г.
 *      Author: sadko
 */

#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const ctl_class_t CtlWidget::metadata = { "CtlWidget", NULL };

        CtlWidget::CtlWidget(CtlRegistry *src, LSPWidget *widget)
        {
            pClass          = &metadata;
            pRegistry       = src;
            pWidget         = widget;

            pVisibilityID   = NULL;
            nVisible        = -1;
            nVisibilityKey  = 0;
            bVisibilitySet  = false;
            bVisibilityKeySet= false;

            nMinWidth       = -1;
            nMinHeight      = -1;
        }

        bool CtlWidget::instance_of(const ctl_class_t *wclass) const
        {
            const ctl_class_t *wc = pClass;
            while (wc != NULL)
            {
                if (wc == wclass)
                    return true;
                wc = wc->parent;
            }

            return false;
        }

        CtlWidget::~CtlWidget()
        {
            destroy();
        }

        LSPWidget *CtlWidget::widget()
        {
            return pWidget;
        };

        void CtlWidget::set_lc_attr(widget_attribute_t att, LSPLocalString *s, const char *name, const char *value)
        {
            // Get prefix
            const char *prefix = widget_attribute(att);
            size_t len = ::strlen(prefix);

            // Prefix matches?
            if (::strncmp(prefix, name, len) != 0)
                return;

            if (name[len] == ':') // Parameter ("prefix:")?
                s->params()->add_cstring(&name[len+1], value);
            else if (name[len] == '\0') // Key?
            {
                if (strchr(value, '.') == NULL) // Raw value with high probability?
                    s->set_raw(value);
                else
                    s->set_key(value);
            }
        }

        void CtlWidget::init_color(color_t value, Color *color)
        {
            LSPDisplay *dpy = (pWidget != NULL) ? pWidget->display() : NULL;
            LSPTheme *theme = (dpy != NULL) ? dpy->theme() : NULL;

            if (theme != NULL)
                theme->get_color(value, color);
        }

        void CtlWidget::init_color(color_t value, LSPColor *color)
        {
            LSPDisplay *dpy = (pWidget != NULL) ? pWidget->display() : NULL;
            LSPTheme *theme = (dpy != NULL) ? dpy->theme() : NULL;

            if (theme != NULL)
                theme->get_color(value, color);
        }

        void CtlWidget::set(const char *name, const char *value)
        {
            widget_attribute_t att = widget_attribute(name);
            if (att != A_UNKNOWN)
                set(att, value);
        }

        void CtlWidget::set(widget_attribute_t att, const char *value)
        {
            if (pWidget == NULL)
                return;

            switch (att)
            {
                case A_VISIBILITY_ID:
                    lsp_trace("deprecated property visibility_id set to value %s", value);
                    if (pVisibilityID != NULL)
                        lsp_free(pVisibilityID);
                    pVisibilityID = strdup(value);
                    break;
                case A_VISIBILITY_KEY:
                    lsp_trace("deprecated property visibility_key set to value %s", value);
                    PARSE_INT(value, nVisibilityKey = __);
                    bVisibilityKeySet = true;
                    break;
                case A_WIDTH:
                    PARSE_INT(value, nMinWidth = __);
                    break;
                case A_HEIGHT:
                    PARSE_INT(value, nMinHeight = __);
                    break;
                case A_VISIBLE:
                    PARSE_BOOL(value, nVisible = (__) ? 1 : 0);
                    break;
                case A_VISIBILITY:
                    BIND_EXPR(sVisibility, value);
                    bVisibilitySet      = true;
                    break;
                case A_BRIGHT:
                    BIND_EXPR(sBright, value);
                    break;
                case A_PADDING:
                    PARSE_INT(value, pWidget->padding()->set_all(__));
                    break;
                case A_PAD_LEFT:
                    PARSE_INT(value, pWidget->padding()->set_left(__));
                    break;
                case A_PAD_RIGHT:
                    PARSE_INT(value, pWidget->padding()->set_right(__));
                    break;
                case A_PAD_TOP:
                    PARSE_INT(value, pWidget->padding()->set_top(__));
                    break;
                case A_PAD_BOTTOM:
                    PARSE_INT(value, pWidget->padding()->set_bottom(__));
                    break;
                case A_EXPAND:
                    PARSE_BOOL(value, pWidget->set_expand(__));
                    break;
                case A_FILL:
                    PARSE_BOOL(value, pWidget->set_fill(__));
                    break;
                case A_HFILL:
                    PARSE_BOOL(value, pWidget->set_hfill(__));
                    break;
                case A_VFILL:
                    PARSE_BOOL(value, pWidget->set_vfill(__));
                    break;
                case A_WUID:
                    pWidget->set_unique_id(value);
                    break;
                default:
                    sBgColor.set(att, value);
                    break;
            }
        }

        status_t CtlWidget::add(CtlWidget *child)
        {
            return STATUS_NOT_IMPLEMENTED;
        }

        void CtlWidget::init()
        {
            sVisibility.init(pRegistry, this);
            sBright.init(pRegistry, this);
            if (pWidget != NULL)
                sBgColor.init_basic(pRegistry, pWidget, pWidget->bg_color(), A_BG_COLOR);
        }

        void CtlWidget::begin()
        {
        }

        void CtlWidget::end()
        {
            if (nVisible >= 0)
            {
                if (pWidget != NULL)
                    pWidget->set_visible(nVisible);
            }
            if ((pVisibilityID != NULL) && (!bVisibilitySet))
            {
                // Parse expression
                char *str = NULL;
                if (!bVisibilityKeySet)
                {
                    CtlPort *port = pRegistry->port(pVisibilityID);
                    const port_t *p = (port != NULL) ? port->metadata() : NULL;
                    if ((p != NULL) && (p->unit == U_BOOL))
                        nVisibilityKey = 1.0f;
                }
                int n = asprintf(&str, ":%s ieq %d", pVisibilityID, int(nVisibilityKey));
                if ((n >= 0) && (str != NULL))
                {
                    sVisibility.parse(str);
                    free(str);
                }
            }

            // Evaluate expression
            if (sVisibility.valid())
            {
                float value = sVisibility.evaluate();
                if (pWidget != NULL)
                    pWidget->set_visible(value >= 0.5f);
            }

            // Evaluate brightness
            if (sBright.valid())
            {
                float value = sBright.evaluate();
                pWidget->set_brightness(value);
            }
        }

        void CtlWidget::notify(CtlPort *port)
        {
            if (pWidget == NULL)
                return;

            // Visibility
            if (sVisibility.depends(port))
            {
                float value = sVisibility.evaluate();
                pWidget->set_visible(value >= 0.5f);
            }

            // Brightness
            if (sBright.depends(port))
            {
                float value = sBright.evaluate();
                pWidget->set_brightness(value);
            }
        }

        void CtlWidget::destroy()
        {
            sVisibility.destroy();
            sBright.destroy();

            if (pVisibilityID != NULL)
            {
                lsp_free(pVisibilityID);
                pVisibilityID = NULL;
            }
        }

        LSPWidget *CtlWidget::resolve(const char *uid)
        {
            const char *wuid = (pWidget != NULL) ? pWidget->unique_id() : NULL;
            if ((wuid != NULL) && (!strcmp(wuid, uid)))
                return pWidget;
            return NULL;
        }
    }
} /* namespace lsp */
