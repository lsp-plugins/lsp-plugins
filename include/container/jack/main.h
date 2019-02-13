/*
 * main.h
 *
 *  Created on: 31 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_JACK_MAIN_H_
#define _CONTAINER_JACK_MAIN_H_

// Do not use tracefile because this file does not use jack-core
#ifdef LSP_TRACEFILE
    #undef LSP_TRACEFILE
#endif /* LSP_TRACEFILE */

#define JACK_MAIN_IMPL

#include <core/types.h>
#include <core/status.h>
#include <core/debug.h>
#include <container/jack/defs.h>
#include <metadata/metadata.h>

#if defined(PLATFORM_WINDOWS)
    #include <container/jack/main/winnt.h>
#else
    #include <container/jack/main/posix.h>
#endif /* PLATFORM_WINDOWS */

#undef JACK_MAIN_IMPL

#endif /* _CONTAINER_JACK_MAIN_H_ */
