/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 19 мая 2016 г.
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

#ifndef CONTAINER_JACK_DEFS_H_
#define CONTAINER_JACK_DEFS_H_

#define JACK_MAIN_FUNCTION          lsp_jack_main
#define JACK_MAIN_FUNCTION_NAME     "lsp_jack_main"
#define JACK_GET_VERSION            lsp_jack_build_version
#define JACK_GET_VERSION_NAME       "lsp_jack_build_version"

namespace lsp
{
    typedef int (* jack_main_function_t)(const char *plugin_id, int argc, const char **argv);

    typedef const char * (* jack_get_version_t)();
}

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

    extern int JACK_MAIN_FUNCTION(const char *plugin_id, int argc, const char **argv);

    extern const char *JACK_GET_VERSION();

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* CONTAINER_JACK_DEFS_H_ */
