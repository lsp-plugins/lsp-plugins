/*
 * lspc.h
 *
 *  Created on: 14 янв. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_LSPC_LSPC_H_
#define CORE_FILES_LSPC_LSPC_H_

#include <core/types.h>

namespace lsp
{
    /** @note All data is stored in big-endian format!
     * Common file structure:
     *
     *      1. Header
     *      2. Chunk
     *      3. Chunk
     *      4. Chunk
     *      ...
     *      N. Chunk
     */

#pragma pack(push, 1)
    typedef struct lspc_root_header_t
    {
        uint32_t        magic;          // Magic number, should be 'LSPC'
        uint16_t        version;        // Header version
        uint16_t        size;           // Size of header
        uint32_t        reserved[4];    // Some reserved data
    } lspc_root_header_t;

    typedef struct lspc_chunk_header_t
    {
        uint32_t        magic;          // Chunk type, should be identical for each chunk
        uint32_t        uid;            // Unique chunk identifier within file
        uint32_t        flags;          // Chunk flags
        uint32_t        size;           // The size of chunk data after header
    } lspc_chunk_header_t;

    typedef struct lspc_header_t
    {
        uint32_t        size;           // Size of header
        uint16_t        version;        // Version of header
    } lspc_chunk_common_header_t;

    typedef struct lspc_chunk_raw_header_t
    {
        lspc_header_t   common;         // Common header data
        uint8_t         data[];         // header contents
    } lspc_chunk_raw_header_t;

    typedef struct lspc_chunk_audio_header_t // Magic number: 'LCAH'
    {
        lspc_header_t   common;         // Common header data
        uint8_t         channels;       // Number of channels
        uint8_t         sample_format;  // Sample format
        uint32_t        sample_rate;    // Sample rate
        uint32_t        codec;          // Codec used
        uint64_t        frames;         // Overall number of frames in file
        int64_t 		offset; 		// Offset with which to load the frames (since header v.1, deprecated since header v.2)
        uint32_t        reserved[4];    // Some reserved data
    } lspc_chunk_audio_header_t;

    typedef struct lspc_chunk_audio_profile_t // Magic number: 'LCAP'
    {
        lspc_header_t   common;         // Common header data
        uint16_t        pad;            // Padding (reserved)
        uint32_t        chunk_id;       // Chunk identifier related to the audio profile
        uint32_t        chirp_order;    // Chirp order
        float           alpha;          // The chirp parameter alpha, a float value
        double          beta;           // The chirp parameter beta, a double value
        double          gamma;          // The chirp parameter gamma, a double value
        double          delta;          // The chirp parameter delta, a double value
        double          initial_freq;   // The chirp initial frequency
        double          final_freq;     // The chirp final frequency
        int64_t         skip;           // Frame to skip for linear response loading (since header v.2)
        uint32_t        reserved[6];    // Some reserved data for future use
    } lspc_chunk_audio_profile_t;

#pragma pack(pop)

// Different chunk types
#define LSPC_ROOT_MAGIC             0x4C535043
#define LSPC_CHUNK_AUDIO            0x41554449
#define LSPC_CHUNK_PROFILE          0x50524F46

// Chunk flags
#define LSPC_CHUNK_FLAG_LAST        (1 << 0)

// Different kinds of sample format
#define LSPC_SAMPLE_FMT_U8LE        0x00
#define LSPC_SAMPLE_FMT_U8BE        0x01
#define LSPC_SAMPLE_FMT_S8LE        0x02
#define LSPC_SAMPLE_FMT_S8BE        0x03
#define LSPC_SAMPLE_FMT_U16LE       0x04
#define LSPC_SAMPLE_FMT_U16BE       0x05
#define LSPC_SAMPLE_FMT_S16LE       0x06
#define LSPC_SAMPLE_FMT_S16BE       0x07
#define LSPC_SAMPLE_FMT_U24LE       0x08
#define LSPC_SAMPLE_FMT_U24BE       0x09
#define LSPC_SAMPLE_FMT_S24LE       0x0a
#define LSPC_SAMPLE_FMT_S24BE       0x0b
#define LSPC_SAMPLE_FMT_U32LE       0x0c
#define LSPC_SAMPLE_FMT_U32BE       0x0d
#define LSPC_SAMPLE_FMT_S32LE       0x0e
#define LSPC_SAMPLE_FMT_S32BE       0x0f
#define LSPC_SAMPLE_FMT_F32LE       0x10
#define LSPC_SAMPLE_FMT_F32BE       0x11
#define LSPC_SAMPLE_FMT_F64LE       0x12
#define LSPC_SAMPLE_FMT_F64BE       0x13

typedef struct lspc_audio_parameters_t
{
    size_t          channels;       // Number of channels
    size_t          sample_format;  // Sample format
    size_t          sample_rate;    // Sample rate
    size_t          codec;          // Codec used
    wsize_t         frames;         // Overall number of frames in file
} lspc_audio_parameters_t;

#define LSPC_SAMPLE_FMT_IS_LE(x)    (!(x & 1))
#define LSPC_SAMPLE_FMT_IS_BE(x)    (x & 1)
#ifdef ARCH_LE /* Little-endian architecture */
    #define LSPC_SAMPLE_FMT_NEED_REVERSE(x)     LSPC_SAMPLE_FMT_IS_BE(x)
#else /* Big-endian architecture */
    #define  LSPC_SAMPLE_FMT_NEED_REVERSE(x)     LSPC_SAMPLE_FMT_IS_LE(x)
#endif /* ARCH_LE */

// Different codec types
#define LSPC_CODEC_PCM              0

} /* lsp */

#endif /* CORE_FILES_LSPC_LSPC_H_ */
