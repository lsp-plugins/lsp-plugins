/*
 * LSPArea3D.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>
#include <dsp/endian.h>
#include <ui/tk/helpers/draw.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPArea3D::metadata = { "LSPArea3D", &LSPWidget::metadata };
        
        LSPArea3D::LSPArea3D(LSPDisplay *dpy):
            LSPWidget(dpy),
            sColor(this),
            sBgColor(this),
            sIPadding(this)
        {
            pClass          = &metadata;

            pBackend        = NULL;
            pGlass          = NULL;
            nBorder         = 12;
            nRadius         = 4;

            nMinWidth       = 1;
            nMinHeight      = 1;

            sIPadding.set(1, 1, 1, 1);
//            pBackendWnd     = NULL;

            dsp::init_matrix3d_identity(&sWorld);
            dsp::init_matrix3d_identity(&sProjection);
            dsp::init_matrix3d_identity(&sView);
        }
        
        LSPArea3D::~LSPArea3D()
        {
            do_destroy();
        }

        status_t LSPArea3D::init()
        {
            status_t res = LSPWidget::init();
            if (res != STATUS_OK)
                return res;

            if (pDisplay != NULL)
            {
                LSPTheme *theme = pDisplay->theme();
                if (theme != NULL)
                {
                    theme->get_color(C_GLASS, &sColor);
                    theme->get_color(C_BACKGROUND, &sBgColor);
                }
            }

            ui_handler_id_t id = 0;

            id = sSlots.add(LSPSLOT_DRAW3D, slot_draw3d, self());

            return (id >= 0) ? STATUS_OK : -id;
        }

        void LSPArea3D::destroy()
        {
            do_destroy();
            LSPWidget::destroy();
        }

        void LSPArea3D::do_destroy()
        {
            if (pBackend != NULL)
            {
                pBackend->destroy();
                delete pBackend;
            }
            if (pGlass != NULL)
            {
                pGlass->destroy();
                delete pGlass;
                pGlass = NULL;
            }
//            if (pBackendWnd != NULL)
//            {
//                pBackendWnd->destroy();
//                delete pBackendWnd;
//            }
            pBackend = NULL;
        }

        IR3DBackend *LSPArea3D::backend()
        {
            if (pBackend != NULL)
                return pBackend;

            // Obtain the necessary information
            IDisplay *dpy = pDisplay->display();
            if (dpy == NULL)
                return NULL;
            LSPWidget *toplevel = this->toplevel();
            if (toplevel == NULL)
                return NULL;
            LSPWindow *wnd = toplevel->cast<LSPWindow>();
            if (wnd == NULL)
                return NULL;
            INativeWindow *native = wnd->native();
            if (native == NULL)
                return NULL;

            // Try to create backend
            IR3DBackend *r3d    = dpy->create3DBackend(native);
            if (r3d == NULL)
                return NULL;

//            INativeWindow *nwnd     = NULL;
//
//            // There is also native window handle present?
//            if (r3d->handle() != NULL)
//            {
//                // Create native window
//                nwnd = dpy->wrapWindow(r3d->handle());
//                if (wnd == NULL)
//                {
//                    r3d->destroy();
//                    return NULL;
//                }
//
//                // Initialize native window
//                if (nwnd->init() != STATUS_OK)
//                {
//                    nwnd->destroy();
//                    return NULL;
//                }
//
//                // Set-up event handler
//                nwnd->set_handler(this);
//            }

            // Resize backend
            if (visible())
            {
                ssize_t wLeft = left();
                ssize_t wTop = top();
                ssize_t wWidth = width();
                ssize_t wHeight = height();

                if (wWidth <= 0)
                    wWidth  = 1;
                if (wHeight <= 0)
                    wHeight = 1;

                r3d->locate(wLeft, wTop, wWidth, wHeight);
//                r3d->show();
            }

            // Store backend pointer and return
            pBackend        = r3d;
//            pBackendWnd     = nwnd;

            return pBackend;
        }

        bool LSPArea3D::hide()
        {
            if (!LSPWidget::hide())
                return false;

            // Hide backend if it is present
//            if (pBackend != NULL)
//                pBackend->hide();

            return true;
        }

        bool LSPArea3D::show()
        {
            if (!LSPWidget::show())
                return false;

            // Obtain backend and show it
            IR3DBackend *r3d    = backend();
            if (r3d != NULL)
            {
                ssize_t wLeft = left();
                ssize_t wTop = top();
                ssize_t wWidth = width();
                ssize_t wHeight = height();

                if (wWidth <= 0)
                    wWidth  = 1;
                if (wHeight <= 0)
                    wHeight = 1;

                r3d->locate(wLeft, wTop, wWidth, wHeight);
//                r3d->show();
            }

            return true;
        }

        void LSPArea3D::realize(const realize_t *r)
        {
            // Realize the widget
            LSPWidget::realize(r);

            // Resize backend
            IR3DBackend *r3d    = pBackend;
            if ((r3d != NULL) && (r3d->valid()))
                r3d->locate(r->nLeft, r->nTop, r->nWidth, r->nHeight);
        }

        void LSPArea3D::size_request(size_request_t *r)
        {
            LSPWidget::size_request(r);

            ssize_t minw    = nMinWidth + sIPadding.horizontal() + (nBorder << 1);
            ssize_t minh    = nMinHeight + sIPadding.vertical() + (nBorder << 1);

            if (r->nMinWidth < minw)
                r->nMinWidth    = minw;
            if (r->nMinHeight < minh)
                r->nMinHeight    = minh;

            if ((r->nMaxWidth >= 0) && (r->nMaxWidth < minw))
                r->nMaxWidth    = minw;
            if ((r->nMaxHeight >= 0) && (r->nMaxHeight < minh))
                r->nMaxHeight   = minh;
        }

        void LSPArea3D::set_min_width(size_t value)
        {
            if (nMinWidth == value)
                return;
            nMinWidth = value;
            query_resize();
        }

        void LSPArea3D::set_min_height(size_t value)
        {
            if (nMinHeight == value)
                return;
            nMinHeight = value;
            query_resize();
        }

        void LSPArea3D::set_border(size_t value)
        {
            if (nBorder == value)
                return;
            nBorder = value;
            query_resize();
        }

        void LSPArea3D::set_radius(size_t value)
        {
            if (nRadius == value)
                return;
            nRadius = value;
            query_resize();
        }

        void LSPArea3D::draw(ISurface *s)
        {
            // Draw background part
            ssize_t pr = (nBorder + 1) >> 1;
            s->fill_frame(0, 0, sSize.nWidth, sSize.nHeight,
                    pr, pr, sSize.nWidth - 2*pr, sSize.nHeight - 2*pr,
                    sBgColor);
            s->fill_round_rect(0, 0, sSize.nWidth, sSize.nHeight, nBorder, SURFMASK_ALL_CORNER, sColor);

            // Estimate the size of the graph
            size_t bs   = nBorder * M_SQRT2 * 0.5;
            ssize_t gw  = sSize.nWidth  - (bs << 1);
            ssize_t gh  = sSize.nHeight - (bs << 1);

            // Obtain a 3D backend and draw it if it is valid
            IR3DBackend *r3d    = backend();
            if ((r3d != NULL) && (r3d->valid()))
            {
                // Update backend color
                color3d_t c;
                c.r     = sColor.red();
                c.g     = sColor.green();
                c.b     = sColor.blue();
                c.a     = 1.0f;
                pBackend->set_bg_color(&c);

                // Update matrices
                pBackend->set_matrix(R3D_MATRIX_PROJECTION, &sProjection);
                pBackend->set_matrix(R3D_MATRIX_VIEW, &sView);
                pBackend->set_matrix(R3D_MATRIX_WORLD, &sWorld);

                // Perform a draw call
                void *buf       = s->start_direct();
                // Estimate the right memory offset
                size_t stride   = s->stride();
                uint8_t *dst    = reinterpret_cast<uint8_t *>(buf) + stride * bs + sizeof(uint32_t) * bs;

                r3d->locate(0, 0, gw, gh);
                r3d->begin_draw();
                    sSlots.execute(LSPSLOT_DRAW3D, this, r3d);
                    r3d->sync();
                    r3d->read_pixels(dst, stride, R3D_PIXEL_RGBA);

                    for (ssize_t i=0; i<gh; ++i)
                    {
                        dsp::abgr32_to_bgra32(dst, dst, gw);
                        dst    += stride;
                    }
                r3d->end_draw();

                s->end_direct();
            }

            // Draw glass
            ISurface *cv = create_border_glass(s, &pGlass, sSize.nWidth, sSize.nHeight, nRadius, nBorder, SURFMASK_ALL_CORNER, sColor);
            if (cv != NULL)
                s->draw(cv, 0, 0);
        }

        status_t LSPArea3D::slot_draw3d(LSPWidget *sender, void *ptr, void *data)
        {
            if ((ptr == NULL) || (data == NULL))
                return STATUS_BAD_ARGUMENTS;

            LSPArea3D *_this   = widget_ptrcast<LSPArea3D>(ptr);
            return (_this != NULL) ? _this->on_draw3d(static_cast<IR3DBackend *>(data)) : STATUS_BAD_ARGUMENTS;
        }

        status_t LSPArea3D::on_draw3d(IR3DBackend *r3d)
        {
            return STATUS_OK;
        }
    
    } /* namespace tk */
} /* namespace lsp */
