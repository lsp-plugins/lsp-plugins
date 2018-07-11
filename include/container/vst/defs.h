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

#if defined (__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
    #define VST_EXPORT  __attribute__ ((visibility ("default")))
#else
    #define VST_EXPORT
#endif

// Include common definitions
#include <container/const.h>
#include <metadata/metadata.h>

// Include VST 2.x SDK
#include <pluginterfaces/vst2.x/aeffect.h>
#include <pluginterfaces/vst2.x/aeffectx.h>
#include <pluginterfaces/vst2.x/vstfxstore.h>

// This routine should be defined in the linked library
typedef AEffect * (* vst_create_instance_t) (const char *bundle_path, VstInt32 uid, audioMasterCallback callback);

#pragma pack(push, 1)
typedef struct vst_state
{
    uint32_t        nItems;             // Number of elements
    uint8_t         vData[];            // Binary data
} vst_state;

typedef struct vst_state_buffer
{
    size_t          nDataSize;          // Size of variable part in bytes
    fxBank          sHeader;            // Program header
    vst_state       sState;             // VST state
} vst_state_buffer;

#pragma pack(pop)

#define VST_CREATE_INSTANCE_NAME        vst_create_instance
#define VST_CREATE_INSTANCE_STRNAME     "vst_create_instance"

#define LSP_VST_USER_MAGIC              CCONST('L', 'S', 'P', 'U')
#define VST_IDENTIFY_MAGIC              CCONST('N', 'v', 'E', 'f')
#define VST_CHUNK_MAGIC                 CCONST('C', 'c', 'n', 'K')
#define VST_REGULAR_CHUNK_MAGIC         CCONST('F', 'x', 'C', 'k')
#define VST_REGULAR_BANK_MAGIC          CCONST('F', 'x', 'C', 'k')
#define VST_OPAQUE_CHUNK_MAGIC          CCONST('F', 'P', 'C', 'h')
#define VST_OPAQUE_BANK_MAGIC           CCONST('F', 'B', 'C', 'h')

#define VST_PROGRAM_HDR_SIZE            (sizeof(fxProgram) - 2 * sizeof(VstInt32))
#define VST_BANK_HDR_SIZE               (sizeof(fxBank) - 2 * sizeof(VstInt32))
#define VST_STATE_BUFFER_SIZE           (VST_BANK_HDR_SIZE + sizeof(vst_state))

namespace lsp
{
    typedef unsigned long               vst_serial_t;

    inline VstInt32 vst_cconst(const char *vst_id)
    {
        if (vst_id == NULL)
        {
            lsp_error("Not defined cconst");
            return 0;
        }
        if (strlen(vst_id) != 4)
        {
            lsp_error("Invalid cconst: %s", vst_id);
            return 0;
        }
        return CCONST(vst_id[0], vst_id[1], vst_id[2], vst_id[3]);
    }

    inline VstInt32 vst_version(uint32_t lsp_version)
    {
        size_t major = LSP_VERSION_MAJOR(lsp_version);
        size_t minor = LSP_VERSION_MINOR(lsp_version);
        size_t micro = LSP_VERSION_MICRO(lsp_version);

        // Limit version elemnts for VST
        if (minor >= 10)
            minor   = 9;
        if (micro >= 100)
            micro = 99;

        // The VST versioning is too dumb, make micro version extended
        return (major * 1000) + (minor * 100) + micro;
    }
}

#endif /* _CONTAINER_VST_DEFS_H_ */
