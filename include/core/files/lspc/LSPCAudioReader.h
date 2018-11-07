/*
 * LSPCSampleReader.h
 *
 *  Created on: 6 нояб. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_LSPC_LSPCAUDIOREADER_H_
#define CORE_FILES_LSPC_LSPCAUDIOREADER_H_

#include <core/files/lspc/lspc.h>
#include <core/files/LSPCFile.h>

namespace lsp
{
    /**
     * This is helper class for reading audio content from LSPC files.
     */
    class LSPCAudioReader
    {
        private:
            enum flags_t
            {
                F_OPENED        = 1 << 0,
                F_CLOSE_READER  = 1 << 1,
                F_CLOSE_FILE    = 1 << 2,
                F_REV_BYTES     = 1 << 3,
                F_DROP_READER   = 1 << 4
            };

            typedef struct buffer_t
            {
                uint8_t                    *vData;      // Pointer to the data
                size_t                      nSize;      // Size of data stored in buffer
                size_t                      nOff;       // Offset to the beginning of non-read data
            } buffer_t;

            typedef void (*decode_func_t)(float *vp, const void *src, size_t ns);

        private:
            lspc_audio_parameters_t     sParams;
            LSPCFile                   *pFD;
            LSPCChunkReader            *pRD;
            size_t                      nFlags;
            size_t                      nBPS;           // Bytes per sample
            size_t                      nFrameSize;     // Size of frame
            size_t                      nBytesLeft;
            buffer_t                    sBuf;
            decode_func_t               pDecode;
            float                      *pFBuffer;       // frame buffer

        protected:
            static void     decode_u8(float *vp, const void *src, size_t ns);
            static void     decode_s8(float *vp, const void *src, size_t ns);
            static void     decode_u16(float *vp, const void *src, size_t ns);
            static void     decode_s16(float *vp, const void *src, size_t ns);
            static void     decode_u24le(float *vp, const void *src, size_t ns);
            static void     decode_u24be(float *vp, const void *src, size_t ns);
            static void     decode_s24le(float *vp, const void *src, size_t ns);
            static void     decode_s24be(float *vp, const void *src, size_t ns);
            static void     decode_u32(float *vp, const void *src, size_t ns);
            static void     decode_s32(float *vp, const void *src, size_t ns);
            static void     decode_f32(float *vp, const void *src, size_t ns);
            static void     decode_f64(float *vp, const void *src, size_t ns);

        protected:
            status_t    read_audio_header(LSPCChunkReader *rd);
            status_t    apply_params(const lspc_audio_parameters_t *p);
            status_t    fill_buffer();

        public:
            LSPCAudioReader();
            ~LSPCAudioReader();

        public:
            /**
             * Find first audio chunk and open it for reading
             * @param lspc opened LSPC file
             * @param auto_close automatically close reader on close()
             * @return status of operation
             */
            status_t open(LSPCFile *lspc, bool auto_close = false);

            /**
             * Find audio chunk by ID and read it
             * @param lspc opened LSPC file
             * @param uid unique chunk identifier
             * @param auto_close automatically close reader on close()
             * @return status of operation
             */
            status_t open(LSPCFile *lspc, uint32_t uid, bool auto_close = false);

            /**
             * Find first audio chunk by magic and open it as audio stream with specified parameters
             * @param lspc opened LSPC file
             * @param magic chunk magic identifier
             * @param params audio parameters
             * @param auto_close automatically close reader on close()
             * @return status of operation
             */
            status_t open_raw_magic(LSPCFile *lspc, const lspc_audio_parameters_t *params, uint32_t magic, bool auto_close = false);

            /**
             * Open audio chunk by UID as audio stream with specified parameters
             * @param lspc opened LSPC file
             * @param uid unique chunk identifier
             * @param params audio parameters
             * @param auto_close automatically close reader on close()
             * @return status of operation
             */
            status_t open_raw_uid(LSPCFile *lspc, const lspc_audio_parameters_t *params, uint32_t uid, bool auto_close = false);

            /**
             * Open audio chunk as sub-stream of audio stream with specified parameters
             * @param rd chunk reader
             * @param params audio parameers
             * @param auto_close automatically close reader on close()
             * @return status of operation
             */
            status_t open_raw(LSPCChunkReader *rd, const lspc_audio_parameters_t *params, bool auto_close = false);

            /**
             * Close audio reader
             * @return status of operation
             */
            status_t close();

            /**
             * Read sample data from chunk
             *
             * @param data array of pointers to store data, must match number of channels in the stream
             * @param frames number of frames to read
             * @return number of frames actually read or error code as a negative value (0 or STATUS_EOF if there is no data)
             */
            ssize_t read_samples(float **data, size_t frames);

            /**
             * Read frames from chunk
             * @param data buffer to store data
             * @param frames number of frames to read
             * @return actual number of frames read or error code as a negative value (0 or STATUS_EOF if there is no data)
             */
            ssize_t read_frames(float *data, size_t frames);

            /**
             * Skip number of frames
             * @param frames number of frames to skip
             * @return number of frames actually skipped
             */
            ssize_t skip_frames(size_t frames);

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

#endif /* CORE_FILES_LSPC_LSPCAUDIOREADER_H_ */
