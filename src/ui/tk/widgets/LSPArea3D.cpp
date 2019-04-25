/*
 * LSPArea3D.cpp
 *
 *  Created on: 25 апр. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        const w_class_t LSPArea3D::metadata = { "LSPArea3D", &LSPWidget::metadata };
        
        LSPArea3D::LSPArea3D(LSPDisplay *dpy):
            LSPWidget(dpy),
            sColor(this),
            sBgColor(this)
        {
            pBackend        = NULL;
            pBackendWnd     = NULL;

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
            if (pBackendWnd != NULL)
            {
                pBackendWnd->destroy();
                delete pBackendWnd;
            }
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

            INativeWindow *nwnd     = NULL;

            // There is also native window handle present?
            if (r3d->handle() != NULL)
            {
                // Create native window
                nwnd = dpy->wrapWindow(r3d->handle());
                if (wnd == NULL)
                {
                    r3d->destroy();
                    return NULL;
                }

                // Initialize native window
                if (nwnd->init() != STATUS_OK)
                {
                    nwnd->destroy();
                    return NULL;
                }

                // Set-up event handler
                nwnd->set_handler(this);
            }

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
                r3d->show();
            }

            // Store backend pointer and return
            pBackend        = r3d;
            pBackendWnd     = nwnd;

            return pBackend;
        }

        bool LSPArea3D::hide()
        {
            if (!LSPWidget::hide())
                return false;

            // Hide backend if it is present
            if (pBackend != NULL)
                pBackend->hide();

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
                r3d->show();
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

            if (r->nMinWidth < 1)
                r->nMinWidth    = 1;
            if (r->nMinHeight < 1)
                r->nMinHeight   = 1;
        }

        void LSPArea3D::render(ISurface *s, bool force)
        {
            // Call the parent widget for draw
            LSPWidget::render(s, force);

            // Obtain a 3D backend and draw it if it is valid
            IR3DBackend *r3d    = backend();
            if ((r3d != NULL) && (r3d->valid()))
            {
                // Update backend color
                color3d_t c;
                c.r     = 0.0f; //sColor.red();
                c.g     = 0.0f; //sColor.green();
                c.b     = 0.0f; //sColor.blue();
                c.a     = 0.0f;
                pBackend->set_bg_color(&c);

                // Update matrices
                pBackend->set_matrix(R3D_MATRIX_PROJECTION, &sProjection);
                pBackend->set_matrix(R3D_MATRIX_VIEW, &sView);
                pBackend->set_matrix(R3D_MATRIX_WORLD, &sWorld);

                // Perform a draw call
                r3d->begin_draw();
                    sSlots.execute(LSPSLOT_DRAW3D, this, r3d);
                r3d->end_draw();
            }
        }

        void LSPArea3D::draw(ISurface *s)
        {
            s->clear(sBgColor);
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
