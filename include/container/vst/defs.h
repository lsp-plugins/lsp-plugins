/*
 * defs.h
 *
 *  Created on: 30 дек. 2015 г.
 *      Author: sadko
 */

#ifndef _CONTAINER_VST_DEFS_H_
#define _CONTAINER_VST_DEFS_H_

#if defined (__GNUC__) && ((__GNUC__ >= 4) || ((__GNUC__ == 3) && (__GNUC_MINOR__ >= 1)))
    #define VST_EXPORT  __attribute__ ((visibility ("default")))
#else
    #define VST_EXPORT
#endif

// Include common definitions
#include <container/const.h>
#include <metadata/metadata.h>

// Include VST 2.x SDK
#include <3rdparty/steinberg/vst2.h>

// This routine should be defined in the linked library
typedef AEffect * (* vst_create_instance_t) (VstInt32 uid, audioMasterCallback callback);

typedef const char * (* vst_get_version_t) ();

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

typedef struct vst_state_header
{
    VstInt32        nMagic1;            // LSP_VST_USER_MAGIC
    VstInt32        nSize;              // Size of contents, again...
    VstInt32        nVersion;           // Current format version
    VstInt32        nMagic2;            // LSP_VST_USER_MAGIC
} vst_state_header;

#pragma pack(pop)

#define VST_CREATE_INSTANCE_NAME        vst_create_instance
#define VST_CREATE_INSTANCE_STRNAME     "vst_create_instance"
#define VST_GET_VERSION_NAME            vst_get_lsp_build_version
#define VST_GET_VERSION_STRNAME         "vst_get_lsp_build_version"

#define LSP_VST_USER_MAGIC              CCONST('L', 'S', 'P', 'U')
#define VST_PROGRAM_HDR_SIZE            (sizeof(fxProgram) - 2 * sizeof(VstInt32))
#define VST_BANK_HDR_SKIP               (2*sizeof(VstInt32))
#define VST_BANK_HDR_SIZE               (sizeof(fxBank) - VST_BANK_HDR_SKIP)
#define VST_STATE_BUFFER_SIZE           (VST_BANK_HDR_SIZE + sizeof(vst_state))

enum
{
    LSP_VST_INT32   = 'i',
    LSP_VST_UINT32  = 'u',
    LSP_VST_INT64   = 'I',
    LSP_VST_UINT64  = 'U',
    LSP_VST_FLOAT32 = 'f',
    LSP_VST_FLOAT64 = 'F',
    LSP_VST_STRING  = 's',
    LSP_VST_BLOB    = 'B'
};

enum
{
    LSP_VST_PRIVATE = 1 << 0
};

#define VST_FX_VERSION_KVT_SUPPORT      2000
#define VST_FX_VERSION_JUCE_FIX         3000

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
