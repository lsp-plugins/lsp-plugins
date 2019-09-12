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
        CtlWidget::CtlWidget(CtlRegistry *src, LSPWidget *widget)
        {
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

        CtlWidget::~CtlWidget()
        {
            destroy();
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
        }

        void CtlWidget::notify(CtlPort *port)
        {
            if (sVisibility.valid())
            {
                float value = sVisibility.evaluate();
                if (pWidget != NULL)
                {
//                    lsp_trace("set visible widget %s ptr=%p to %s",
//                            pWidget->get_class()->name,
//                            pWidget,
//                            (value >= 0.5f) ? "true" : "false");
                    pWidget->set_visible(value >= 0.5f);
                }
            }
        }

        void CtlWidget::destroy()
        {
            sVisibility.destroy();

            if (pVisibilityID != NULL)
            {
                lsp_free(pVisibilityID);
                pVisibilityID = NULL;
            }
        }

        LSPWidget *CtlWidget::resolve(const char *uid)
        {
            const char *wuid = pWidget->unique_id();
            if ((wuid != NULL) && (!strcmp(wuid, uid)))
                return pWidget;
            return NULL;
        }
    }
} /* namespace lsp */
