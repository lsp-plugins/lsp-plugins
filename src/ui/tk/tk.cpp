/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 19 июн. 2017 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        static const char *color_names[] =
        {
            "bg",
            "bg2",
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
            "button_text",

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

            "hlink_text",
            "hlink_hover",

            "status_ok",
            "status_warn",
            "status_error",

            "invalid_input",

            NULL
        };

        color_t color_id(const char *name)
        {
            size_t id = 0;
            for (const char **c = color_names; *c != NULL; ++c, ++id)
                if (!::strcmp(*c, name))
                    return color_t(id);
            return C_UNKNOWN;
        }

        const char *color_name(color_t color)
        {
            if ((color < 0) || (color >= ssize_t(sizeof(color_names)/sizeof(const char *))))
                return NULL;

            return color_names[color];
        }
    }
}


