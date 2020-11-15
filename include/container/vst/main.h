/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 31 дек. 2015 г.
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
