/*
 * LSPGroup.cpp
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPGroup::metadata = { "LSPGroup", &LSPWidgetContainer::metadata };

        LSPGroup::LSPGroup(LSPDisplay *dpy):
            LSPWidgetContainer(dpy),
            sText(this),
            sColor(this),
            sFont(this)
        {
            nRadius     = 10;
            nBorder     = 0;
            pWidget     = NULL;
            bEmbed      = false;

            pClass      = &metadata;
        }

        LSPGroup::~LSPGroup()
        {
            do_destroy();
        }

        status_t LSPGroup::init()
        {
            sText.bind();

            status_t result = LSPWidgetContainer::init();
            if (result != STATUS_OK)
                return result;

            if (pDisplay != NULL)
            {
                LSPTheme *theme = pDisplay->theme();

                if (theme != NULL)
                {
                    sFont.init(theme->font());
                    sFont.set_size(12.0f);
                    init_color(C_BACKGROUND, sFont.color());
                }
            }

            init_color(C_LABEL_TEXT, &sColor);

            return STATUS_OK;
        }

        void LSPGroup::destroy()
        {
            do_destroy();
            LSPWidgetContainer::destroy();
        }

        LSPWidget *LSPGroup::find_widget(ssize_t x, ssize_t y)
        {
            if (pWidget == NULL)
                return NULL;
            return (pWidget->inside(x, y)) ? pWidget : NULL;
        }

        void LSPGroup::query_dimensions(dimensions_t *d)
        {
            size_t bw       = (bEmbed) ? 1 : ::round(nRadius * M_SQRT2 * 0.5) + 1;
            size_t dd       = bw + nBorder + 1;
            d->nGapLeft     = dd;
            d->nGapRight    = dd;
            d->nGapTop      = dd;
            d->nGapBottom   = dd;
            d->nMinWidth    = nBorder*2;
            d->nMinHeight   = nBorder*2;

            LSPString text;
            sText.format(&text);
            if (!text.is_empty())
            {
                // Create temporary surface
                ISurface *s = (pDisplay != NULL) ? pDisplay->create_surface(1, 1) : NULL;
                if (s == NULL)
                    return;

                font_parameters_t   fp;
                text_parameters_t   tp;

                sFont.get_parameters(s, &fp);
                sFont.get_text_parameters(s, &tp, &text);

                d->nMinWidth    += tp.Width + nRadius * 3;
                d->nMinHeight   += fp.Height + nRadius * 2;
                d->nGapTop      += fp.Height;

                // Destroy surface
                s->destroy();
                delete s;
            }
        }

        void LSPGroup::do_destroy()
        {
            if (pWidget != NULL)
            {
                unlink_widget(pWidget);
                pWidget  = NULL;
            }
        }

        void LSPGroup::set_radius(size_t value)
        {
            if (nRadius == value)
                return;
            nRadius = value;
            query_resize();
        }

        void LSPGroup::set_border(size_t value)
        {
            if (nBorder == value)
                return;
            nBorder = value;
            query_resize();
        }

        void LSPGroup::set_embed(bool embed)
        {
            if (bEmbed == embed)
                return;
            bEmbed = embed;
            query_resize();
        }

        void LSPGroup::render(ISurface *s, bool force)
        {
            if (nFlags & REDRAW_SURFACE)
                force = true;

            // Prepare palette
            Color bg_color(sBgColor);
            Color color(sColor);
            color.scale_lightness(brightness());

//            lsp_trace("Rendering this=%p, force=%d", this, int(force));
            // Draw child
            if (pWidget != NULL)
            {
                if ((force) || (pWidget->redraw_pending()))
                {
                    pWidget->render(s, force);
                    pWidget->commit_redraw();
                }
            }

            if (force)
            {
                // Get resource
                ssize_t cx  = sSize.nLeft + nBorder + 1;
                ssize_t cy  = sSize.nTop + nBorder + 1;
                ssize_t sx  = sSize.nWidth - (nBorder << 1) - 1;
                ssize_t sy  = sSize.nHeight - (nBorder << 1) - 1;
//                size_t bw   = round(nRadius * M_SQRT2 * 0.5f) + 1;

                // Draw background
                if (pWidget == NULL)
                    s->fill_rect(sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight, bg_color);
                else
                {
                    realize_t r;
                    pWidget->get_dimensions(&r);
//                    Color red(1.0f, 0.0f, 0.0f);
                    if ((bEmbed) && (nRadius > 1))
                        s->fill_round_frame(
                            sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight,
                            r.nLeft, r.nTop, r.nWidth, r.nHeight,
                            nRadius-1, SURFMASK_B_CORNER,
                            bg_color
                        );
                    else
                        s->fill_frame(
                                sSize.nLeft, sSize.nTop, sSize.nWidth, sSize.nHeight,
                                r.nLeft, r.nTop, r.nWidth, r.nHeight,
                                bg_color
                            );
                }

                // Draw frame
                bool aa = s->set_antialiasing(true);
                s->wire_round_rect(cx, cy, sx-1, sy-1, nRadius, 0x0e, 2.0f, color);

                // Draw text frame
                LSPString text;
                sText.format(&text);
                if (!text.is_empty())
                {
                    // Draw text border
                    font_parameters_t   fp;
                    text_parameters_t   tp;

                    sFont.get_parameters(s, &fp);
                    sFont.get_text_parameters(s, &tp, &text);

                    s->fill_round_rect(cx-1, cy-1, 4 + nRadius + tp.Width, fp.Height + 4, nRadius, 0x04, color);

                    // Show text
                    Color font(sFont.raw_color());
                    font.scale_lightness(brightness());
                    sFont.draw(s, cx + 4, cy + fp.Ascent + nBorder, font, &text);
                }

                s->set_antialiasing(aa);
            }
        }

        status_t LSPGroup::add(LSPWidget *widget)
        {
            if (pWidget != NULL)
                return STATUS_ALREADY_EXISTS;

            widget->set_parent(this);
            pWidget = widget;
            query_resize();
            return STATUS_OK;
        }

        status_t LSPGroup::remove(LSPWidget *widget)
        {
            if (pWidget != widget)
                return STATUS_NOT_FOUND;

            unlink_widget(pWidget);
            pWidget  = NULL;

            return STATUS_OK;
        }

        void LSPGroup::size_request(size_request_t *r)
        {
            if (pWidget != NULL)
                pWidget->size_request(r);

            if (r->nMinWidth < 0)
                r->nMinWidth    = 0;
            if (r->nMinHeight < 0)
                r->nMinHeight   = 0;

            if (pWidget != NULL)
            {
                r->nMinWidth   += pWidget->padding()->horizontal();
                r->nMinHeight  += pWidget->padding()->vertical();
            }

            dimensions_t d;
            query_dimensions(&d);

            if (r->nMinWidth >= 0)
            {
                size_t  n = r->nMinWidth + d.nGapLeft + d.nGapRight;
                if (n < d.nMinWidth)
                    r->nMinWidth    = d.nMinWidth;
                else
                    r->nMinWidth    = n;
            }
            if (r->nMinHeight >= 0)
            {
                size_t  n = r->nMinHeight + d.nGapTop + d.nGapBottom;
                if (n < d.nMinHeight)
                    r->nMinHeight   = d.nMinHeight;
                else
                    r->nMinHeight   = n;
            }

            // Align to 8-pixel grid
//            r->nMinWidth    = ((r->nMinWidth  + 7) >> 3) << 3;
//            r->nMinHeight   = ((r->nMinHeight + 7) >> 3) << 3;

            if ((r->nMaxWidth >= 0) && (r->nMaxWidth < r->nMinWidth))
                r->nMaxWidth    = r->nMinWidth;
            if ((r->nMaxHeight >= 0) && (r->nMaxHeight < r->nMinHeight))
                r->nMaxHeight   = r->nMinHeight;
        }

        void LSPGroup::realize(const realize_t *r)
        {
            LSPWidgetContainer::realize(r);
            if (pWidget == NULL)
                return;

            dimensions_t d;
            query_dimensions(&d);

            size_request_t sr;
            pWidget->size_request(&sr);

            realize_t rc;
            rc.nLeft    = r->nLeft   + d.nGapLeft  + pWidget->padding()->left();
            rc.nTop     = r->nTop    + d.nGapTop   + pWidget->padding()->top();
            rc.nWidth   = r->nWidth  - d.nGapLeft  - d.nGapRight   - pWidget->padding()->horizontal();
            rc.nHeight  = r->nHeight - d.nGapTop   - d.nGapBottom  - pWidget->padding()->vertical();

            if ((sr.nMaxWidth > 0) && (sr.nMaxWidth < rc.nWidth))
            {
                rc.nLeft   += (rc.nWidth - sr.nMaxWidth) >> 1;
                rc.nWidth   = sr.nMaxWidth;
            }

            if ((sr.nMaxHeight > 0) && (sr.nMaxHeight < rc.nHeight))
            {
                rc.nTop    += (rc.nHeight - sr.nMaxHeight) >> 1;
                rc.nHeight  = sr.nMaxHeight;
            }

            pWidget->realize(&rc);
        }

    } /* namespace tk */
} /* namespace lsp */
