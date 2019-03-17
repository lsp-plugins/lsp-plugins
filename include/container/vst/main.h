/*
 * main.h
 *
 *  Created on: 31 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_VST_MAIN_H_
#define _CONTAINER_VST_MAIN_H_

#define VST_MAIN_IMPL

// Do not use tracefile because this file does not use jack-core
#ifdef LSP_TRACEFILE
    #undef LSP_TRACEFILE
#endif /* LSP_TRACEFILE */

#include <core/debug.h>
#include <core/types.h>
#include <container/vst/defs.h>
#include <container/common/libpath.h>

#if defined(PLATFORM_WINDOWS)
    #include <container/vst/main/winnt.h>
#else
    #include <container/vst/main/posix.h>
#endif /* PLATFORM_WINDOWS */

// This should be included to generate other VST stuff
#include <3rdparty/steinberg/vst2main.h>

#undef VST_MAIN_IMPL

#endif /* _CONTAINER_VST_MAIN_H_ */
