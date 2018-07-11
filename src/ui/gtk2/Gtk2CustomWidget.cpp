/*
 * Gtk2CustomWidget.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <core/debug.h>

namespace lsp
{
    #define GTK2_CUSTOM(obj)            (GTK_CHECK_CAST((obj), custom_type, Gtk2Custom))
    #define GTK2_CUSTOM_CLASS(klass)    (GTK_CHECK_CLASS_CAST((klass), custom_type, Gtk2CustomClass))
    #define GTK2_IS_CUSTOM(obj)         (GTK_CHECK_TYPE((obj), custom_type))
    #define GTK2_CUSTOM_GETCLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), custom_type, Gtk2CustomClass))

    #define CHECK_GTK2_CUSTOM(widget)                   \
        g_return_val_if_fail(widget != NULL, FALSE);    \
        g_return_val_if_fail(GTK2_IS_CUSTOM(widget), FALSE);

    static GType custom_type = 0;

    const GTypeInfo Gtk2CustomWidget::type_info =
    {
        sizeof(Gtk2CustomClass), // Size of class descriptor
        NULL, // Base init
        NULL, // Base finalize
        reinterpret_cast<GClassInitFunc>(class_init), // Class initialize
        NULL, // Class finalize
        NULL, // Class data
        sizeof(Gtk2Custom), // Size of instance
        0,
        reinterpret_cast<GInstanceInitFunc>(init)
    };

    void Gtk2CustomWidget::class_init(Gtk2CustomClass *klass)
    {
        GtkWidgetClass *wc          = GTK_WIDGET_CLASS(klass);
        GtkObjectClass *oc          = GTK_OBJECT_CLASS(klass);

        wc->realize                 = realize;
        wc->size_request            = size_request;
        wc->size_allocate           = size_allocate;
        wc->expose_event            = expose;

        wc->button_press_event      = button_press;
        wc->button_release_event    = button_release;
        wc->motion_notify_event     = motion_notify;
        wc->scroll_event            = scroll;
//        wc->enter_notify_event      = enter_notify;

        klass->destroy              = oc->destroy;
        oc->destroy                 = destroy;
    }

    void Gtk2CustomWidget::destroy(GtkObject *object)
    {
        g_return_if_fail(object != NULL);
        g_return_if_fail(GTK2_IS_CUSTOM(object));

        // Call destroy if possible
        Gtk2Custom *_this = GTK2_CUSTOM(object);
        if (_this->pImpl != NULL)
        {
            _this->pImpl->destroy();
            _this->pImpl->pWidget   = NULL;
            _this->pImpl            = NULL;
        }

        // Get widget class
        Gtk2CustomClass *klass = GTK2_CUSTOM_GETCLASS(object);
        if (klass->destroy != NULL)
            klass->destroy(object);
    }

    void Gtk2CustomWidget::init(GtkWidget *widget)
    {
    }

    gboolean Gtk2CustomWidget::expose(GtkWidget *widget, GdkEventExpose *event)
    {
//        lsp_trace("expose x=%d y=%d, w=%d, h=%d",
//            int(event->area.x), int(event->area.y),
//            int(event->area.width), int(event->area.height));

        CHECK_GTK2_CUSTOM(widget);
        g_return_val_if_fail(event != NULL, FALSE);

        Gtk2Custom *_this = GTK2_CUSTOM(widget);
        if (_this->pImpl != NULL)
        {
            _this->pImpl->allowRedraw();
            _this->pImpl->render();
        }

        return FALSE;
    }

    gboolean Gtk2CustomWidget::button_press(GtkWidget *widget, GdkEventButton *event)
    {
        g_return_val_if_fail(widget != NULL, FALSE);
        g_return_val_if_fail(GTK2_IS_CUSTOM(widget), FALSE);

        Gtk2Custom *_this = GTK2_CUSTOM(widget);

        if (_this->pImpl != NULL)
        {
            switch (event->type)
            {
                case GDK_2BUTTON_PRESS:
                    _this->pImpl->button_double_press(size_t(event->x), size_t(event->y), size_t(event->state), size_t(event->button));
                    break;
                case GDK_3BUTTON_PRESS:
                    _this->pImpl->button_triple_press(size_t(event->x), size_t(event->y), size_t(event->state), size_t(event->button));
                    break;
                default:
                    _this->pImpl->button_press(size_t(event->x), size_t(event->y), size_t(event->state), size_t(event->button));
                    break;
            }
        }

        return TRUE;
    }

    gboolean Gtk2CustomWidget::enter_notify(GtkWidget *widget, GdkEventCrossing *event)
    {
//        g_return_val_if_fail(widget != NULL, FALSE);
//        g_return_val_if_fail(GTK2_IS_CUSTOM(widget), FALSE);
//
//        Gtk2Custom *_this = GTK2_CUSTOM(widget);
//        if (_this->pImpl != NULL)
//            _this->pImpl->motion(ssize_t(event->x), ssize_t(event->y), size_t(event->state));

        return TRUE;
    }

    gboolean Gtk2CustomWidget::motion_notify(GtkWidget *widget, GdkEventMotion *event)
    {
        g_return_val_if_fail(widget != NULL, FALSE);
        g_return_val_if_fail(GTK2_IS_CUSTOM(widget), FALSE);

        Gtk2Custom *_this = GTK2_CUSTOM(widget);
        if (_this->pImpl != NULL)
            _this->pImpl->motion(ssize_t(event->x), ssize_t(event->y), size_t(event->state));

        return TRUE;
    }

    gboolean Gtk2CustomWidget::button_release(GtkWidget *widget, GdkEventButton *event)
    {
        g_return_val_if_fail(widget != NULL, FALSE);
        g_return_val_if_fail(GTK2_IS_CUSTOM(widget), FALSE);

        Gtk2Custom *_this = GTK2_CUSTOM(widget);
        if (_this->pImpl != NULL)
            _this->pImpl->button_release(ssize_t(event->x), ssize_t(event->y), size_t(event->state), size_t(event->button));

        return TRUE;
    }

    gboolean Gtk2CustomWidget::scroll(GtkWidget *widget, GdkEventScroll *event)
    {
        g_return_val_if_fail(widget != NULL, FALSE);
        g_return_val_if_fail(GTK2_IS_CUSTOM(widget), FALSE);

        Gtk2Custom *_this = GTK2_CUSTOM(widget);
        if (_this->pImpl != NULL)
            _this->pImpl->scroll(ssize_t(event->x), ssize_t(event->y), size_t(event->state), size_t(event->direction));

        return TRUE;
    }

    void Gtk2CustomWidget::realize(GtkWidget *widget)
    {
        g_return_if_fail(widget != NULL);
        g_return_if_fail(GTK2_IS_CUSTOM(widget));

        GTK_WIDGET_SET_FLAGS(widget, GTK_REALIZED);

        Gtk2Custom *_this   = GTK2_CUSTOM(widget);
        if (_this->pImpl == NULL)
            return;

        GdkWindowAttr attr;
        attr.window_type        = GDK_WINDOW_CHILD;
        attr.x                  = widget->allocation.x;
        attr.y                  = widget->allocation.y;
        attr.width              = widget->allocation.width;
        attr.height             = widget->allocation.height;

        _this->pImpl->locate(
            attr.x, attr.y, attr.width, attr.height
        );

        attr.wclass             = GDK_INPUT_OUTPUT;
        attr.window_type        = GDK_WINDOW_CHILD;
        attr.visual             = gtk_widget_get_visual (widget);
        attr.colormap           = gtk_widget_get_colormap (widget);
        attr.event_mask         = gtk_widget_get_events(widget) |
                                    GDK_EXPOSURE_MASK |
                                    GDK_BUTTON_PRESS_MASK |
                                    GDK_BUTTON_RELEASE_MASK |
                                    GDK_BUTTON_MOTION_MASK |
                                    GDK_POINTER_MOTION_MASK |
                                    GDK_SCROLL_MASK;
        guint mask              = GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL | GDK_WA_COLORMAP;

        widget->window          = gdk_window_new(
            gtk_widget_get_parent_window (widget),
            & attr, mask
        );

        widget->style = gtk_style_attach(widget->style, widget->window);

        gdk_window_set_user_data(widget->window, widget);
        gtk_widget_set_app_paintable(widget, FALSE);

        gtk_style_set_background(widget->style, widget->window, GTK_STATE_ACTIVE);

//        lsp_trace("realize w=%d h=%d", int(attr.width), int(attr.height));
    }

    void Gtk2CustomWidget::size_request(GtkWidget *widget, GtkRequisition *requisition)
    {
        g_return_if_fail(widget != NULL);
        g_return_if_fail(GTK2_IS_CUSTOM(widget));
        g_return_if_fail(requisition != NULL);

        Gtk2Custom *_this   = GTK2_CUSTOM(widget);

//        lsp_trace("size_request w=%d h=%d", int(requisition->width), int(requisition->height));
        size_t w = requisition->width;
        size_t h = requisition->height;

        if (_this->pImpl != NULL)
            _this->pImpl->resize(w, h);

        requisition->width  = w;
        requisition->height = h;
//        lsp_trace("size_request actual w=%d h=%d", int(requisition->width), int(requisition->height));
    }

    void Gtk2CustomWidget::size_allocate(GtkWidget *widget, GtkAllocation *allocation)
    {
        g_return_if_fail(widget != NULL);
        g_return_if_fail(GTK2_IS_CUSTOM(widget));
        g_return_if_fail(allocation != NULL);

        Gtk2Custom *_this   = GTK2_CUSTOM(widget);

//        lsp_trace("size_allocate x=%d y=%d w=%d h=%d", int(allocation->x), int(allocation->y), int(allocation->width), int(allocation->height));
        if (_this->pImpl != NULL)
            _this->pImpl->locate(allocation->x, allocation->y, allocation->width, allocation->height);

        widget->allocation          = *allocation;

        if (GTK_WIDGET_REALIZED(widget))
        {
            gdk_window_move_resize(
                   widget->window,
                   allocation->x, allocation->y,
                   allocation->width, allocation->height
            );

            if (_this->pImpl != NULL)
                _this->pImpl->markRedraw();
        }
    }

    Gtk2CustomWidget::Gtk2CustomWidget(plugin_ui *ui, widget_t w_class, GType basic_class): Gtk2Widget(ui, w_class)
    {
        // Register custom type
        if (!custom_type)
        {
            // Register type
            for (size_t i = 0; ; i++)
            {
                char buf[128];
                sprintf(buf, "Gtk2CustomWidget-v%d", int(i));
                gchar *name = g_strdup(buf);

                if (!g_type_from_name(name))
                {
                    custom_type = g_type_register_static(basic_class, name, &type_info,(GTypeFlags)0);
                    lsp_trace("registred type %s as %llx", buf, (long long)(custom_type));

                    g_free(name);
                    break;
                }
                g_free(name);
            }
        }

        // Create widget and bind implementation
        GtkWidget *widget   = GTK_WIDGET(gtk_type_new(custom_type));
        g_return_if_fail(GTK2_IS_CUSTOM(widget));

        Gtk2Custom *_this   = GTK2_CUSTOM(widget);
        _this->pImpl        = this;

        // Store widget
        pWidget             = widget;
        nLeft               = 0;
        nTop                = 0;
        nWidth              = 0;
        nHeight             = 0;
        bDirty              = true;
        pSurface            = NULL;
    }

    Gtk2CustomWidget::~Gtk2CustomWidget()
    {
        // Get widget and disconnect
        if (pWidget != NULL)
        {
            Gtk2Custom *_this   = GTK2_CUSTOM(pWidget);
            if (_this != NULL)
                _this->pImpl        = NULL;

            pWidget             = NULL;
        }

        if (pSurface != NULL)
        {
            cairo_surface_destroy(pSurface);
            pSurface        = NULL;
        }
    }

    void Gtk2CustomWidget::draw(cairo_t *cr)
    {
    }

    cairo_surface_t *Gtk2CustomWidget::get_surface()
    {
        // Check surface
        if (pSurface != NULL)
        {
            size_t width    = cairo_image_surface_get_width(pSurface);
            size_t height   = cairo_image_surface_get_height(pSurface);

            if ((nWidth != width) || (nHeight != height))
            {
                cairo_surface_destroy(pSurface);
                pSurface    = NULL;
            }
        }

        // Create new surface if needed
        if (pSurface == NULL)
        {
            pSurface    = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, nWidth, nHeight);
            if (pSurface == NULL)
                return NULL;
            bDirty      = true;
        }

        if (bDirty)
        {
            // Create cairo context
            cairo_t *cr = cairo_create(pSurface);
            if (cr == NULL)
                return NULL;

            // Call underlying function for rendering
            draw(cr);

            // Destroy cairo context
            cairo_destroy(cr);

            // Mark as drawn
            bDirty = false;
        }

        return pSurface;
    }

    void Gtk2CustomWidget::render()
    {
        // Get surface
        cairo_surface_t *s  = get_surface();
        if (s == NULL)
            return;

        // Get cairo resource of the window
        cairo_t *cr = gdk_cairo_create(pWidget->window);
        cairo_set_source_surface (cr, s, 0, 0);
        cairo_paint(cr);

        // Release resource
        cairo_destroy(cr);
    }

    void Gtk2CustomWidget::markRedraw()
    {
        bDirty      = true;
        Gtk2Widget::markRedraw();
    }

    void Gtk2CustomWidget::resize(size_t &w, size_t &h)
    {
    }

    void Gtk2CustomWidget::locate(ssize_t x, ssize_t y, size_t w, size_t h)
    {
        nLeft       = x;
        nTop        = y;
        nWidth      = w;
        nHeight     = h;
    }

    void Gtk2CustomWidget::button_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
    }

    void Gtk2CustomWidget::button_double_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        button_press(x, y, state, button);
    }

    void Gtk2CustomWidget::button_triple_press(ssize_t x, ssize_t y, size_t state, size_t button)
    {
        button_press(x, y, state, button);
    }

    void Gtk2CustomWidget::button_release(ssize_t x, ssize_t y, size_t state, size_t button)
    {
    }

    void Gtk2CustomWidget::motion(ssize_t x, ssize_t y, size_t state)
    {
    }

    void Gtk2CustomWidget::scroll(ssize_t x, ssize_t y, size_t state, size_t direction)
    {
    }

    void Gtk2CustomWidget::destroy()
    {
    }

} /* namespace lsp */
