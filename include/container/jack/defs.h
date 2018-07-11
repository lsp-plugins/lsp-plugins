/*
 * defs.h
 *
 *  Created on: 19 мая 2016 г.
 *      Author: sadko
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
