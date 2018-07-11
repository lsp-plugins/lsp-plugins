/*
 * defs.h
 *
 *  Created on: 30 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_VST_DEFS_H_
#define _CONTAINER_VST_DEFS_H_

// Define __cdecl modifier
#ifdef __GNUC__
    #ifndef __cdecl
        #if defined(__i386__)
            #define __cdecl __attribute__((__cdecl__))
        #elif defined(__x86_64__)
            #define __cdecl
        #endif /* __cdecl */
    #endif /* __cdecl */
#endif /* __GNUC__ */

// Include VST 2.x SDK
#include <pluginterfaces/vst2.x/aeffect.h>
#include <pluginterfaces/vst2.x/aeffectx.h>
#include <pluginterfaces/vst2.x/vstfxstore.h>

// This routine should be defined in the linked library
typedef AEffect * (vst_create_instance_t) (const char *uid, audioMasterCallback callback);

#define VST_CREATE_INSTANCE_NAME        vst_create_instance
#define VST_CREATE_INSTANCE_STRNAME     #VST_CREATE_INSTANCE_NAME

#endif /* _CONTAINER_VST_DEFS_H_ */
