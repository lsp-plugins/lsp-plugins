/*
 * common.cpp
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */
#include <string.h>
#include <stddef.h>

#include <ui/common.h>

namespace lsp
{
    const char *widget_tags[] =
    {
        "plugin",
        "mstud",
        "button",
        "vbox",
        "hbox",
        "grid",
        "row",
        "cell",
        "label",
        "param",
        "value",
        "indicator",
        "led",
        "knob",
        "group",
        "align",
        "switch",
        "graph",
        "axis",
        "marker",
        "body",
        "mesh",
        "basis",
        "center",
        "combo",
        "text",
        "file",
        "hsep",
        "vsep",
        "port",
        "meter",
        "dot",
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
        NULL
    };

    const char *color_names[] =
    {
        "background",
        "hole",
        "glass",

        "red",
        "green",
        "blue",
        "yellow",
        "cyan",
        "magenta",
        "black",
        "white",

        "label_text",
        "button_face",

        "knob_cap",
        "knob_scale",

        "logo_face",
        "logo_text",

        "graph_axis",
        "graph_mesh",
        "graph_marker",
        "graph_text",
        "graph_line",
        "left_channel",
        "right_channel",
        "middle_channel",
        "side_channel",

        NULL
    };

    widget_t widget_type(const char *name)
    {
        size_t id = 0;
        for (const char **tag = widget_tags; *tag != NULL; ++tag, ++id)
            if (!strcmp(*tag, name))
                return widget_t(id);
        return W_UNKNOWN;
    }

    const char *widget_type(widget_t type)
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

    color_t color_id(const char *name)
    {
        size_t id = 0;
        for (const char **c = color_names; *c != NULL; ++c, ++id)
            if (!strcmp(*c, name))
                return color_t(id);
        return C_UNKNOWN;
    }

    const char *color_name(color_t color)
    {
        for (ssize_t id=0; id <= color; ++id)
        {
            if (color_names[id] == NULL)
                return NULL;
        }

        return color_names[color];
    }

}


