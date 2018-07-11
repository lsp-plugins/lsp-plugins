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
        "visibility",
        "proportional",
        "expand",
        "fill",
        "key",
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

        "label_text",
        "button_face",

        "knob_cap",
        "knob_scale",

        "logo_face",
        "logo_text",

        "graph_axis",
        "graph_mesh",

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


