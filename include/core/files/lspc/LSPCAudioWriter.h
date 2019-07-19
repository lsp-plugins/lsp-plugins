/*
 * LSPCAudioWriter.h
 *
 *  Created on: 6 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_LSPC_LSPCAUDIOWRITER_H_
#define CORE_FILES_LSPC_LSPCAUDIOWRITER_H_

#include <core/files/lspc/lspc.h>
#include <core/files/LSPCFile.h>

namespace lsp
{
    /**
     * This is helper class for writing audio content to LSPC files.
     */
    class LSPCAudioWriter
    {
        private:
            LSPCAudioWriter & operator = (const LSPCAudioWriter &);

        private:
            enum flags_t
            {
                F_OPENED            = 1 << 0,
                F_CLOSE_WRITER      = 1 << 1,
                F_CLOSE_FILE        = 1 << 2,
                F_REV_BYTES         = 1 << 3,
                F_DROP_WRITER       = 1 << 4,
                F_INTEGER_SAMPLE    = 1 << 5,
                F_DROP_FILE         = 1 << 6
            };

            typedef void (*encode_func_t)(void *dst, const float *src, size_t ns);

        protected:
            lspc_audio_parameters_t     sParams;
            LSPCFile                   *pFD;
            LSPCChunkWriter            *pWD;
            size_t                      nFlags;
            size_t                      nBPS;           // Bytes per sample
            size_t                      nFrameChannels; // Size of frame in channels
            encode_func_t               pEncode;
            float                      *pBuffer;
            uint8_t                    *pFBuffer;       // frame buffer

        protected:
            static void     encode_u8(void *vp, const float *src, size_t ns);
            static void     encode_s8(void *vp, const float *src, size_t ns);
            static void     encode_u16(void *vp, const float *src, size_t ns);
            static void     encode_s16(void *vp, const float *src, size_t ns);
            static void     encode_u24le(void *vp, const float *src, size_t ns);
            static void     encode_u24be(void *vp, const float *src, size_t ns);
            static void     encode_s24le(void *vp, const float *src, size_t ns);
            static void     encode_s24be(void *vp, const float *src, size_t ns);
            static void     encode_u32(void *vp, const float *src, size_t ns);
            static void     encode_s32(void *vp, const float *src, size_t ns);
            static void     encode_f32(void *vp, const float *src, size_t ns);
            static void     encode_f64(void *vp, const float *src, size_t ns);

        protected:
            status_t parse_parameters(const lspc_audio_parameters_t *p);
            status_t free_resources();
            status_t write_header(LSPCChunkWriter *wr);

        public:
            explicit LSPCAudioWriter();
            ~LSPCAudioWriter();

        public:
            /**
             * Create new LSPC file and open for writing, write header to LSPC file
             * @param path UTF-8 path to the LSPC file
             * @param params audio parameters
             * @return status of operation
             */
            status_t create(const char *path, const lspc_audio_parameters_t *params);

            /**
             * Create new LSPC file and open for writing, write header to LSPC file
             * @param path path to the LSPC file
             * @param params audio parameters
             * @return status of operation
             */
            status_t create(const LSPString *path, const lspc_audio_parameters_t *params);

            /**
             * Create new LSPC file and open for writing, write header to LSPC file
             * @param path path to the LSPC file
             * @param params audio parameters
             * @return status of operation
             */
            status_t create(const io::Path *path, const lspc_audio_parameters_t *params);

            /**
             * Create chunk in LSPC file with magic=LSPC_CHUNK_AUDIO and write header
             * @param lspc LSPC file
             * @param params audio stream parameters
             * @param auto_close automatically close file on close()
             * @return status of operation
             */
            status_t open(LSPCFile *lspc, const lspc_audio_parameters_t *params, bool auto_close = false);

            /**
             * Create chunk in LSPC file with magic=LSPC_CHUNK_AUDIO, do not write header
             * @param lspc LSPC file
             * @param params audio stream parameters
             * @param auto_close automatically close file on close()
             * @return status of operation
             */
            status_t open_raw(LSPCFile *lspc, const lspc_audio_parameters_t *params, bool auto_close = false);

            /**
             * Create chunk in LSPC file with specified magic and write header
             * @param lspc LSPC file
             * @param magic specified chunk magic
             * @param params audio stream parameters
             * @param auto_close automatically close file on close()
             * @return status of operation
             */
            status_t open(LSPCFile *lspc, uint32_t magic, const lspc_audio_parameters_t *params, bool auto_close = false);

            /**
             * Create chunk in LSPC file with specified magic, do not write header
             * @param lspc LSPC file
             * @param magic specified chunk magic
             * @param params audio stream parameters
             * @param auto_close automatically close file on close()
             * @return status of operation
             */
            status_t open_raw(LSPCFile *lspc, uint32_t magic, const lspc_audio_parameters_t *params, bool auto_close = false);

            /**
             * Write header to already opened chunk
             * @param wr chunk writer
             * @param params audio stream parameters
             * @param auto_close automatically close chunk writer on close()
             * @return status of operation
             */
            status_t open(LSPCChunkWriter *wr, const lspc_audio_parameters_t *params, bool auto_close = false);

            /**
             * Open chunk writer as a raw data stream. Does not emit audio header into stream contents.
             * @param wr chunk writer
             * @param params audio stream parameters
             * @param auto_close automatically close chunk writer on close()
             * @return status of operation
             */
            status_t open_raw(LSPCChunkWriter *wr, const lspc_audio_parameters_t *params, bool auto_close = false);

            /**
             * Close audio writer
             * @return status of operation
             */
            status_t close();

            /**
             * Write sample data to chunk
             *
             * @param data array of pointers to store data, must match number of channels in the stream
             * @param frames number of frames to write
             * @return status of operation
             */
            status_t write_samples(const float **data, size_t frames);

            /**
             * Write frames to chunk
             * @param data buffer to store data
             * @param frames number of frames to write
             * @return status of operation
             */
            status_t write_frames(const float *data, size_t frames);

            /**
             * Obtain current audio parameters of the stream
             * @param dst pointer to store audio parameters
             * @return status of operation
             */
            status_t get_parameters(lspc_audio_parameters_t *dst) const;

            /**
             * Get current chunk identifier
             * @return current chunk identifier
             */
            uint32_t unique_id() const;

            /**
             * Get current chunk magic
             * @return current chunk magic
             */
            uint32_t magic() const;
    };

} /* namespace lsp */

#endif /* CORE_FILES_LSPC_LSPCAUDIOWRITER_H_ */
