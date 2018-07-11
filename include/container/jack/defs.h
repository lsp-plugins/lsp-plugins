/*
 * defs.h
 *
 *  Created on: 19 мая 2016 г.
 *      Author: sadko
 */

#ifndef CONTAINER_JACK_DEFS_H_
#define CONTAINER_JACK_DEFS_H_

#define JACK_MAIN_FUNCTION lsp_jack_main
#define JACK_MAIN_FUNCTION_NAME "lsp_jack_main"

namespace lsp
{
    typedef int (* jack_main_function_t)(const char *plugin_id, int argc, const char **argv);
}

extern "C"
{
    extern int JACK_MAIN_FUNCTION(const char *plugin_id, int argc, const char **argv);
}

#endif /* CONTAINER_JACK_DEFS_H_ */
