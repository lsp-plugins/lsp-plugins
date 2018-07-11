#include <gtk/gtk.h>

#include <lv2.h>
#include <core/plugin_metadata.h>
#include <core/plugins.h>
#include <lv2/lv2plug.in/ns/extensions/ui/ui.h>
#include <ui/ui.h>

// LV2UI extension routines
LV2_SYMBOL_EXPORT
const LV2_Descriptor *lv2_descriptor(uint32_t index);

LV2_SYMBOL_EXPORT
const LV2UI_Descriptor *lv2ui_descriptor(uint32_t index);

namespace gtk_test
{
    using namespace lsp;

    typedef struct lv2_controller_t
    {
    } lv2_controller_t;

    lv2_controller_t lv2_controller;

    void lv2_write_function(
        LV2UI_Controller controller,
        uint32_t         port_index,
        uint32_t         buffer_size,
        uint32_t         port_protocol,
        const void*      buffer)
    {
    }

    void lv2_window_destroy( GtkWidget *widget, gpointer data)
    {
        gtk_main_quit ();
    }

    int test(int argc, const char **argv)
    {
        size_t plugin_id = 0;
        const plugin_metadata_t *mdata = &phase_detector_metadata::metadata;

        const LV2_Descriptor *descr = lv2_descriptor(plugin_id);
        if (descr == NULL)
            return -1;

        const LV2UI_Descriptor *ui_descr = lv2ui_descriptor(plugin_id);
        if (ui_descr == NULL)
            return -1;

        gtk_init (&argc, const_cast<char ***>(&argv));

        GtkWidget *window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
        gtk_window_set_title(GTK_WINDOW(window), "Test plugin");
        gtk_window_set_default_size (GTK_WINDOW (window), 500, 300);
        gtk_container_set_border_width (GTK_CONTAINER (window), 0);

        g_signal_connect (G_OBJECT (window), "destroy",
                      G_CALLBACK (lv2_window_destroy), NULL);

        GtkWidget *widget = NULL;

        LV2UI_Handle handle = ui_descr->instantiate(
                ui_descr, descr->URI, "./res", lv2_write_function,
                &lv2_controller, reinterpret_cast<LV2UI_Widget *>(&widget), NULL
            );

        gtk_container_add(GTK_CONTAINER(window), widget);

        gtk_widget_show_all(window);

//        float value = 0;
        LV2UI_Peak_Data pd;
        pd.peak = 0;
        pd.period_size = 0;
        pd.period_start = 0;
        size_t port_id = 0;

        for (const port_t *port = mdata->ports; (port->id != NULL) && (port->name != NULL); ++port)
        {
            pd.peak     = port->start;
            switch (port->role)
            {
                case R_UI_SYNC:
                    break;
                case R_AUDIO:
                    break;
                case R_METER:
                    ui_descr->port_event(handle, port_id, sizeof(LV2UI_Peak_Data), 0, &pd);
                    break;
                case R_CONTROL:
                    ui_descr->port_event(handle, port_id, sizeof(float), 0, &pd.peak);
                    break;
                default:
                    break;
            }

            port_id ++;
        }

        gtk_main ();

        ui_descr->cleanup(handle);

        return 0;
    }
}
