
#include <ui/ctl/ctl.h>

namespace lsp
{
    namespace ctl
    {
        const char *widget_tags[] =
        {
            "align",
            "axis",
            "basis",
            "body",
            "box",
            "button",
            "cell",
            "center",
            "cgroup",
            "combo",
            "dot",
            "edit",
            "fader",
            "fbuffer",
            "file",
            "frac",
            "graph",
            "grid",
            "group",
            "hbox",
            "hgrid",
            "hlink",
            "hsbar",
            "hsep",
            "indicator",
            "knob",
            "label",
            "led",
            "listbox",
            "marker",
            "mesh",
            "meter",
            "mstud",
            "param",
            "plugin",
            "port",
            "save",
            "sbar",
            "sep",
            "switch",
            "text",
            "ttap",
            "value",
            "vbox",
            "vgrid",
            "vsbar",
            "vsep",
            "window",
            NULL
        };

        const char *widget_attributes[] =
        {
            "width",
            "height",
            "size",
            "text",
            "rows",
            "cols",
            "color",
            "bg_color",
            "scale_color",
            "scale_hue_id",
            "scale_sat_id",
            "scale_light_id",
            "border_color",
            "text_color",
            "format",
            "id",
            "balance",
            "invert",
            "toggle",
            "border",
            "padding",
            "pad_left",
            "pad_top",
            "pad_right",
            "pad_bottom",
            "vpos",
            "hpos",
            "vscale",
            "hscale",
            "vspacing",
            "hspacing",
            "spacing",
            "angle",
            "aspect",
            "units",
            "valign",
            "halign",
            "resizable",
            "min",
            "max",
            "basis",
            "parallel",
            "value",
            "visible",
            "visibility_id",
            "visibility_key",
            "proportional",
            "expand",
            "fill",
            "key",
            "center",
            "logarithmic",
            "hue_id",
            "sat_id",
            "light_id",
            "coord",
            "head_id",
            "tail_id",
            "fadein_id",
            "fadeout_id",
            "length_id",
            "status_id",
            "mesh_id",
            "detailed",
            "transpose",
            "led",
            "reversive",
            "type",
            "font_size",
            "activity_id",
            "scroll_id",
            "vpos_id",
            "hpos_id",
            "smooth",
            "precision",
            "editable",
            "same_line",
            "offset",
            "visibility",
            "stereo",
            "id2",
            "color2",
            "hue2_id",
            "sat2_id",
            "light2_id",
            "activity2_id",
            "horizontal",
            "vertical",
            "activity",
            "activity2",
            "step",
            "tiny_step",
            "default",
            "radius",
            "url",
            "command_id",
            "progress_id",
            "path_id",
            "hfill",
            "vfill",
            "format_id",
            "bind",
            "opacity",
            "transparency",
            "mode",
            NULL
        };

        widget_ctl_t widget_ctl(const char *name)
        {
            size_t id = 0;
            for (const char **tag = widget_tags; *tag != NULL; ++tag, ++id)
                if (!strcmp(*tag, name))
                    return widget_ctl_t(id);
            return WC_UNKNOWN;
        }

        const char *widget_ctl(widget_ctl_t type)
        {
            return widget_tags[type];
        }

        widget_attribute_t widget_attribute(const char *name)
        {
            size_t id = 0;
            for (const char **tag = widget_attributes; *tag != NULL; ++tag, ++id)
                if (!strcmp(*tag, name))
                    return widget_attribute_t(id);
            return A_UNKNOWN;
        }
    }
}
