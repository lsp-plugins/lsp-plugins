/*
 * IWidgetCore.cpp
 *
 *  Created on: 12 дек. 2016 г.
 *      Author: sadko
 */

#include <ui/ui.h>

namespace lsp
{
    
    IWidgetCore::IWidgetCore()
    {
        nLeft       = 0;
        nTop        = 0;
        nWidth      = 0;
        nHeight     = 0;
        bVisible    = true;
    }
    
    IWidgetCore::~IWidgetCore()
    {
    }

    widget_t IWidgetCore::getClass() const
    {
        return W_UNKNOWN;
    }

    void IWidgetCore::render(ISurface *s)
    {
    }

    void IWidgetCore::realize(const ui_realize_t *r)
    {
        nLeft           = r->nLeft;
        nTop            = r->nTop;
        nWidth          = r->nWidth;
        nHeight         = r->nHeight;
    }

    void IWidgetCore::size_request(ui_size_request_t *r)
    {
        r->nMinWidth    = 0;
        r->nMinHeight   = 0;
        r->nMaxWidth    = -1;
        r->nMaxHeight   = -1;
    }

    void IWidgetCore::handle_event(const ui_event_t *e)
    {
        switch (e->nType)
        {
            case UIE_KEY_DOWN:
            {
                on_key_down(e);
                break;
            }

            case UIE_KEY_UP:
            {
                on_key_up(e);
                break;
            }

            case UIE_MOUSE_DOWN:
            {
                on_mouse_down(e);
                break;
            }

            case UIE_MOUSE_UP:
            {
                on_mouse_up(e);
                break;
            }

            case UIE_MOUSE_MOVE:
            {
                on_motion(e);
                break;
            }

            default:
                break;
        }
    }

    void IWidgetCore::on_mouse_down(const ui_event_t *e)
    {
    }

    void IWidgetCore::on_mouse_up(const ui_event_t *e)
    {
    }

    void IWidgetCore::on_key_down(const ui_event_t *e)
    {
    }

    void IWidgetCore::on_key_up(const ui_event_t *e)
    {
    }

    void IWidgetCore::on_motion(const ui_event_t *e)
    {
    }

} /* namespace lsp */
