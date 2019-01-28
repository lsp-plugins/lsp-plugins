/*
  Copyright 2017 Linux Studio Plugins Project <lsp.plugin@gmail.com>

  Permission to use, copy, modify, and/or distribute this software for any
  purpose with or without fee is hereby granted, provided that the above
  copyright notice and this permission notice appear in all copies.

  THIS SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
  WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
  MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
  ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
  WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
  ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
  OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
*/

#ifndef _3RDPARTY_STEINBERG_VST2MAIN_H_
#define _3RDPARTY_STEINBERG_VST2MAIN_H_

#include "vst2.h"

/**
 * This file should be included after implementation of
 * the VST Main function to generate additional stub functions
 * for compatibility with old hosts.
 */

#ifdef WIN32
    #include <windows.h>
#endif /* WIN32 */

//-------------------------------------------------------------------------------------------------------
// Main function shared object stub definition
//-------------------------------------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

    // Different routine names for different hosts
    #if (TARGET_API_MAC_CARBON && __ppc__)
        VST_EXPORT AEffect* main_macho (audioMasterCallback audioMaster) { return VSTPluginMain (audioMaster); }
    #elif WIN32
        VST_EXPORT AEffect* MAIN (audioMasterCallback audioMaster) { return VSTPluginMain (audioMaster); }
    #elif BEOS
        VST_EXPORT AEffect* main_plugin (audioMasterCallback audioMaster) { return VSTPluginMain (audioMaster); }
    #endif

    // Windows-specific code
    #ifdef WIN32
        void* hInstance;

        BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
        {
            hInstance = hInst;
            return 1;
        }
    #endif /* WIN32 */

#ifdef __cplusplus
} /* extern "C" */
#endif /* __cplusplus */

#endif /* _3RDPARTY_STEINBERG_VST2MAIN_H_ */
