/*
 * Gtk2Window.cpp
 *
 *  Created on: 20 окт. 2015 г.
 *      Author: sadko
 */

#include <ui/gtk2/ui.h>
#include <container/const.h>

#define MSTUD_PORT      UI_CONFIG_PORT_PREFIX UI_MOUNT_STUD_PORT_ID
#define VERSION_PORT    UI_CONFIG_PORT_PREFIX UI_LAST_VERSION_PORT_ID

namespace lsp
{
    Gtk2Window::Gtk2Window(plugin_ui *ui) : Gtk2Container(ui, W_PLUGIN)
    {
        pWidget     = NULL;
        sBgColor.set(pUI->theme(), C_BACKGROUND);

        for (size_t i=C_FIRST; i < C_TOTAL; ++i)
            pWidgets[i]     = NULL;

        bBody           = false;
        nWidth          = -1;
        nHeight         = -1;
        bResizable      = true;
        bMapped         = false;
        bRedrawing      = false;
        hFunction       = 0;
        hMapHandler     = 0;
        hUnmapHandler   = 0;
        pToplevel       = NULL;
        pVersion        = NULL;
    }

    Gtk2Window::~Gtk2Window()
    {
        // Remove timer event
        if ((hFunction > 0) && (bMapped))
        {
            lsp_trace("Remove function %x", int(hFunction));
            g_source_remove(hFunction);
        }

        hFunction   = 0;
        bMapped     = false;

        // Unlink widget
        if (pWidget != NULL)
            pWidget     = NULL;

        // Remove map/unmap handler
        if (pToplevel != NULL)
        {
            if (hMapHandler > 0)
            {
                lsp_trace("Remove map handler %x", int(hMapHandler));
                if (g_signal_handler_is_connected (pToplevel, hMapHandler))
                    g_signal_handler_disconnect(pToplevel, hMapHandler);
                hMapHandler     = 0;
            }
            if (hUnmapHandler > 0)
            {
                lsp_trace("Remove unmap handler %x", int(hUnmapHandler));
                if (g_signal_handler_is_connected (pToplevel, hUnmapHandler))
                    g_signal_handler_disconnect(pToplevel, hUnmapHandler);
                hUnmapHandler   = 0;
            }

            pToplevel   = NULL;
        }

        // Forget widgets
        for (size_t i=C_FIRST; i < C_TOTAL; ++i)
            pWidgets[i]     = NULL;
    }

    void Gtk2Window::set(widget_attribute_t att, const char *value)
    {
        switch (att)
        {
            case A_BG_COLOR:
                sBgColor.set(pUI->theme(), value);
                break;
            case A_WIDTH:
                PARSE_INT(value, nWidth = __);
                break;
            case A_HEIGHT:
                PARSE_INT(value, nHeight = __);
                break;
            case A_RESIZABLE:
                PARSE_BOOL(value, bResizable = __);
                break;
            default:
                Gtk2Container::set(att, value);
                break;
        }
    }

    void Gtk2Window::begin()
    {
        Gtk2Container::begin();

        char buf[32];
        sBgColor.format_rgb(buf, sizeof(buf), 2);

        pWidgets[C_ROOT]    =   pUI->createWidget(W_BODY);
        pWidgets[C_ROOT]    ->  begin();
        pWidgets[C_ROOT]    ->  set(A_BG_COLOR, buf);

        // Generate common look
        pWidgets[C_BOX]     =   pUI->createWidget(W_HBOX);
        pWidgets[C_BOX]     ->  set(A_SIZE, "3");
        pWidgets[C_BOX]     ->  set(A_BORDER, "0");
        pWidgets[C_BOX]     ->  begin();

        // Left part with logo
        pWidgets[C_LEFT]    =   pUI->createWidget(W_MSTUD);
        pWidgets[C_LEFT]    ->  set(A_TEXT, LSP_ACRONYM);
        pWidgets[C_LEFT]    ->  set(A_ANGLE, "0");
        pWidgets[C_LEFT]    ->  set(A_VISIBILITY_ID, MSTUD_PORT);
        pWidgets[C_LEFT]    ->  set(A_VISIBILITY_KEY, "1");
        pWidgets[C_LEFT]    ->  begin();
        pWidgets[C_LEFT]    ->  end();
        pWidgets[C_BOX]     ->  add(pWidgets[C_LEFT]);

        // Middle part with plugin contents
        pWidgets[C_MIDDLE]  =   pUI->createWidget(W_VBOX);
        pWidgets[C_MIDDLE]  ->  set(A_BORDER, "2");
        pWidgets[C_MIDDLE]  ->  begin();

        bBody               =   true;

        snprintf(buf, sizeof(buf), "%s  %s", LSP_ACRONYM, pUI->metadata()->acronym);

        // Add menu
        pWidgets[C_MENU]    =   pUI->createWidget(W_MSTUD);
        pWidgets[C_MENU]    ->  set(A_TEXT, buf);
        pWidgets[C_MENU]    ->  set(A_ANGLE, "2");
        pWidgets[C_MENU]    ->  set(A_VISIBILITY_ID, MSTUD_PORT);
        pWidgets[C_MENU]    ->  set(A_VISIBILITY_KEY, "0");
        pWidgets[C_MENU]    ->  begin();
        pWidgets[C_MENU]    ->  end();
        pWidgets[C_MIDDLE]  ->  add(pWidgets[C_MENU]);

        // Bind version port
        BIND_PORT(pUI, pVersion, VERSION_PORT);
    }

    void Gtk2Window::add(IWidget *widget)
    {
        if (bBody)
            pWidgets[C_MIDDLE]     ->  add(widget);
        else
        {
            Gtk2Widget *g_widget = Gtk2Widget::cast(widget);
            if (g_widget != NULL)
                pWidget = g_widget->widget();
        }
    }

    void Gtk2Window::end()
    {
        bBody               =   false;
        const plugin_metadata_t *mdata = pUI->metadata();

        pWidgets[C_MIDDLE]  ->  end();
        pWidgets[C_BOX]     ->  add(pWidgets[C_MIDDLE]);

        // Right part with plugin name
        pWidgets[C_RIGHT]   =   pUI->createWidget(W_MSTUD);
        pWidgets[C_RIGHT]   ->  set(A_TEXT, mdata->acronym);
        pWidgets[C_RIGHT]   ->  set(A_ANGLE, "1");
        pWidgets[C_RIGHT]   ->  set(A_VISIBILITY_ID, MSTUD_PORT);
        pWidgets[C_RIGHT]   ->  set(A_VISIBILITY_KEY, "1");
        pWidgets[C_RIGHT]   ->  begin();
        pWidgets[C_RIGHT]   ->  end();
        pWidgets[C_BOX]     ->  add(pWidgets[C_RIGHT]);

        // Add body to this widget
        pWidgets[C_BOX]     ->  end();

        pWidgets[C_ROOT]    ->  add(pWidgets[C_BOX]);
        pWidgets[C_ROOT]    ->  end();

        this                ->  add(pWidgets[C_FIRST]);

        g_signal_connect (G_OBJECT (pWidget), "parent-set", G_CALLBACK (gtk_window_set_parent), this);

        Gtk2Container::end();
    }

    void Gtk2Window::set_parent(GtkWidget *parent)
    {
        // Change size
        GtkWidget *toplevel = gtk_widget_get_toplevel(pWidget);
        lsp_trace("toplevel = %p", toplevel);

        if ((toplevel != NULL) && (gtk_widget_is_toplevel (toplevel)) && (GTK_IS_WINDOW(toplevel)))
        {
            pToplevel   = toplevel;

            // Due to having artifacts with some cairo elements on expose() we have to periodically redraw the entire window
            hMapHandler     = g_signal_connect(pToplevel, "map", G_CALLBACK(map_window), gpointer(this));
            lsp_trace("Added map handler %x", int(hMapHandler));
            hUnmapHandler   = g_signal_connect(pToplevel, "unmap", G_CALLBACK(unmap_window), gpointer(this));
            lsp_trace("Added unmap handler %x", int(hUnmapHandler));
        }
        else
        {
            hFunction           = 0;
            hMapHandler         = 0;
            hUnmapHandler       = 0;
            pToplevel           = NULL;
        }
    }

    void Gtk2Window::gtk_window_set_parent(GtkWidget *widget, GtkObject *prev, gpointer p_this)
    {
        Gtk2Window *_this = reinterpret_cast<Gtk2Window *>(p_this);
        lsp_trace("_this=%p, _this->pWidget=%p, _this->bMapped=%d, _this->Function=%x, p_this=%p",
                _this, _this->pWidget, _this->bMapped ? 1 : 0, int(_this->hFunction), reinterpret_cast<const void *>(p_this));
        if (_this != NULL)
            _this->set_parent(widget);
    }

    gboolean Gtk2Window::redraw_window(gpointer ptr)
    {
        Gtk2Window *_this = reinterpret_cast<Gtk2Window *>(ptr);
//        lsp_trace("_this=%p, _this->pWidget=%p, _this->bMapped=%d, _this->Function=%x",
//                _this, _this->pWidget, _this->bMapped ? 1 : 0, int(_this->hFunction));
        if (_this == NULL)
            return TRUE;
        if (!_this->bMapped)
            return TRUE;
        if (_this->pWidget == NULL)
            return TRUE;
        if (_this->pUI == NULL)
            return TRUE;
        if (_this->bRedrawing)
            return TRUE;

        // Call for redraw
        _this->bRedrawing = true;
        _this->pUI->redraw();
        _this->bRedrawing = false;

        return TRUE;
    }

    void Gtk2Window::show()
    {
        // Check
        if (pToplevel == NULL)
            return;

        // Request size of widget
        GtkRequisition rq;
        gtk_window_get_size(GTK_WINDOW(pToplevel), &rq.width, &rq.height);

        if (pWidget != NULL)
            gtk_widget_size_request(pWidget, &rq);
        lsp_trace("window width=%d, height=%d", int(rq.width), int(rq.height));
        if (nWidth >= 0)
            rq.width    = nWidth;
        if (nHeight >= 0)
            rq.height   = nHeight;

        // Change width and height of the window
        lsp_trace("resize width=%d, height=%d", int(rq.width), int(rq.height));
        gtk_window_resize(GTK_WINDOW(pToplevel), rq.width, rq.height);

        // Make window resizable if needed
        lsp_trace("set_resizable");
        gtk_window_set_resizable(GTK_WINDOW(pToplevel), (bResizable) ? TRUE : FALSE);

        // Since Ardour doesn't make plugin's window always above it's window, force it to be above
        lsp_trace("set_keep_above");
        gtk_window_set_keep_above(GTK_WINDOW(pToplevel), TRUE);

        // Add periodically redraw
        lsp_trace("add timeout");
        hFunction    = g_timeout_add (1000 / MESH_REFRESH_RATE, redraw_window, this);
        bMapped      = true;

        // Handle show
        IUIWrapper *wrapper = pUI->getWrapper();
        if (wrapper != NULL)
            wrapper->ui_activated();

        // Show notification
        show_notification();

        lsp_trace("show finished");
    }

    void Gtk2Window::show_notification()
    {
        lsp_trace("Checking version");
        if (pVersion == NULL)
            return;
        const char *v = pVersion->getBuffer<char>();
        if (strcmp(LSP_MAIN_VERSION, v) == 0)
            return;

        // Main version and version differ, need to show dialog
        GtkWidget *dialog = gtk_dialog_new_with_buttons(
                "Update notification",
                NULL,
                GTK_DIALOG_MODAL,
                "OK",
                GTK_RESPONSE_NONE,
                NULL
            );

        GtkWidget *body = gtk_dialog_get_content_area(GTK_DIALOG(dialog));

        GtkWidget *vbox = gtk_vbox_new(FALSE, 8);
        gtk_container_set_border_width(GTK_CONTAINER(vbox), 16);

            GtkWidget *align = gtk_alignment_new(0, 0.5, 0.0, 0.0);
            GtkWidget *text = gtk_label_new("Greetings!");
            gtk_label_set_markup(GTK_LABEL(text), "<span size=\"16000\">Greetings!</span>");
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            gtk_container_add(GTK_CONTAINER(vbox), gtk_label_new(""));

            align = gtk_alignment_new(0, 0.5, 0.0, 0.0);
            text = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(text), "<b>You've just updated plugins to version " LSP_MAIN_VERSION "!</b>");
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(0, 0.5, 0.0, 0.0);
            text = gtk_label_new("The " LSP_FULL_NAME " is non-commercial project and needs financial support for the further development.");
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(0, 0.5, 0.0, 0.0);
            text = gtk_label_new("You may help all plugins become open source by visiting the following link and submitting donations to the project:");
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(0, 0.5, 0.0, 0.0);
            text = gtk_link_button_new(LSP_DONATION_URI);
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(0, 0.5, 0.0, 0.0);
            text = gtk_label_new("You can find more information about policy of publishing source code by visiting the following link:");
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(0, 0.5, 0.0, 0.0);
            text = gtk_link_button_new(LSP_DOWNLOAD_URI);
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(0, 0.5, 0.0, 0.0);
            text = gtk_label_new("Remember that subscription and regular small donations will give more benefits to the project than one-time donations.");
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(0, 0.5, 0.0, 0.0);
            text = gtk_label_new("To not to be very annoying, this dialog will be shown only after each version update of plugins.");
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
            text = gtk_label_new("Thanks in advance");
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
            text = gtk_label_new(LSP_FULL_NAME);
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

            align = gtk_alignment_new(1.0, 0.5, 0.0, 0.0);
            text = gtk_link_button_new(LSP_BASE_URI);
            gtk_container_add(GTK_CONTAINER(align), text);
            gtk_box_pack_start(GTK_BOX(vbox), align, TRUE, TRUE, 0);

        gtk_container_add(GTK_CONTAINER(body), vbox);
        gtk_widget_show_all(vbox);

        gtk_window_set_keep_above(GTK_WINDOW(dialog), TRUE);

        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);

        // Store new version
        pVersion->write(LSP_MAIN_VERSION, strlen(LSP_MAIN_VERSION));
    }

    void Gtk2Window::map_window(GtkWidget *widget, gpointer ptr)
    {
        Gtk2Window *_this   = reinterpret_cast<Gtk2Window *>(ptr);
        lsp_trace("_this=%p, _this->pWidget=%p, _this->bMapped=%d, _this->Function=%x",
                _this, _this->pWidget, _this->bMapped ? 1 : 0, int(_this->hFunction));
        if ((_this != NULL) && (!_this->bMapped))
            _this->show();
        lsp_trace("map_window finished");
    }

    void Gtk2Window::unmap_window(GtkWidget *widget, gpointer ptr)
    {
        Gtk2Window *_this   = reinterpret_cast<Gtk2Window *>(ptr);
        lsp_trace("_this=%p, _this->pWidget=%p, _this->bMapped=%d, _this->Function=%x",
                _this, _this->pWidget, _this->bMapped ? 1 : 0, int(_this->hFunction));
        if ((_this != NULL) && (_this->bMapped))
        {
            // Mark unmapped
            if (_this->hFunction)
                g_source_remove(_this->hFunction);
            _this->bMapped      = false;
            _this->hFunction    = 0;

            // Handle hide
            IUIWrapper *wrapper = _this->pUI->getWrapper();
            if (wrapper != NULL)
                wrapper->ui_deactivated();
        }
        lsp_trace("unmap_window finished");
    }

} /* namespace lsp */
