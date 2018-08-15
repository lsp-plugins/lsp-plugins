/*
 * ShiftBuffer.h
 *
 *  Created on: 29 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_SHIFTBUFFER_H_
#define CORE_UTIL_SHIFTBUFFER_H_

#include <dsp/dsp.h>

namespace lsp
{
    /** Shift buffer processor
     *    This class implements shift buffer.
     *    New data is added to buffer at the tail position with append() methods
     *    Old data is removed from buffer from the head position with shift() methods
     *
     */
    class ShiftBuffer
    {
        private:
            float      *pData;
            size_t      nCapacity;
            size_t      nHead;
            size_t      nTail;

        public:
            ShiftBuffer();
            ~ShiftBuffer();

        public:
            /** Init buffer, all previously stored data will be lost
             *
             * @param size the requested size of buffer, in terms of optimization may be allocated a bit more data
             * @param gap number of zero samples initially stored in buffer, can not be greater than size
             * @return status of operation
             */
            bool init(size_t size, size_t gap = 0);

            /** Destroy buffer
             *
             */
            void destroy();

            /** Resize buffer, if not initialized is similar to init()
             *
             * @param size the requested size of buffer, in terms of optimization may be allocated a bit more data
             * @param gap number of samples initially stored in buffer, if gap > previous size() then additional samples will be zeroed
             * @return status of operation
             */
            bool resize(size_t size, size_t gap = 0);

            /** Add data to the end of buffer
             *
             * @param data amount of data to push, if NULL then buffer is filled with zeros
             * @param count number of samples
             * @return number of samples appended
             */
            size_t append(const float *data, size_t count);

            /** Append the single sample
             *
             * @param data sample to append
             * @return number of samples appended
             */
            size_t append(float data);

            /** Remove data from the beginning of the buffer
             *
             * @param data pointer to store the samples removed from buffer, may be NULL for skipping
             * @param count number of samples to remove
             * @return number of samples removed
             */
            size_t shift(float *data, size_t count);

            /** Remove data from the beginning of the buffer
             *
             * @param count number of samples to remove
             * @return number of samples removed
             */
            size_t shift(size_t count);

            /** Remove one sample from the beginning of the buffer
             *
             * @return removed sampler or 0 if the buffer is empty
             */
            float shift();

            /** Return the number of items in the buffer
             *
             * @return number of items in the buffer
             */
            inline size_t size() const { return nTail - nHead;  };

            /** Get maximum size of the buffer
             *
             * @return maximum size of the buffer
             */
            inline size_t capacity() const { return nCapacity; };

            /** Clear buffer
             *
             */
            inline void clear() { nHead = nTail = 0; }

            /** Get the data pointer at the head of buffer
             *
             * @return data pointer at the head of buffer
             */
            inline float *head()
            {
                return (pData != NULL) ? &pData[nHead] : NULL;
            }

            /** Get the data pointer at the tail of buffer
             *
             * @return data pointer at the tail of buffer
             */
            inline float *tail()
            {
                return (pData != NULL) ? &pData[nTail] : NULL;
            }

            /** Get the data pointer at the head of buffer
             * @param offset offset from the head
             * @return data pointer at the head of buffer
             */
            inline float *head(size_t offset)
            {
                if (pData == NULL)
                    return NULL;
                offset += nHead;
                return (offset >= nTail) ? NULL : &pData[offset];
            }

            /** Get the data pointer at the tail of buffer
             * @param offset offset from the tail
             * @return data pointer at the tail of buffer
             */
            inline float *tail(size_t offset)
            {
                if (pData == NULL)
                    return NULL;
                ssize_t off = nTail - offset;
                return (off < ssize_t(nHead)) ? NULL : &pData[off];
            }

            /** Get sample from tail
             *
             * @param offset offset
             * @return sample
             */
            inline float last(size_t offset)
            {
                if (pData == NULL)
                    return 0.0f;
                ssize_t off = nTail - offset;
                return (off < ssize_t(nHead)) ? 0.0f : pData[off];
            }

            /** Get the first sample in the buffer
             *
             * @return the first sample in the buffer or 0 if empty
             */
            inline float first() const
            {
                return ((pData != NULL) && (nTail > nHead)) ? pData[nHead] : 0.0f;
            }

            /** Get the last sample in the buffer
             *
             * @return the last sample in the buffer or 0 if empty
             */
            inline float last() const
            {
                return ((pData != NULL) && (nTail > nHead)) ? pData[nTail-1] : 0.0f;
            }

            /** Get the first sample in the buffer
             *
             * @param offset offset from the head
             *
             * @return the first sample in the buffer or 0 if empty
             */
            inline float first(size_t offset) const
            {
                if (pData == NULL)
                    return 0.0f;
                offset += nHead;
                return (offset >= nTail) ? 0.0f : pData[offset];
            }

            /** Get the last sample in the buffer
             * @param offset offset from the tail
             *
             * @return the last sample in the buffer or 0 if empty
             */
            inline float last(size_t offset) const
            {
                if (pData == NULL)
                    return 0.0f;
                ssize_t off     = nTail - offset;
                return (off >= ssize_t(nHead)) ? pData[off] : 0.0f;
            }

            /** Fill buffer with specific value
             *
             * @param value value used to fill
             */
            inline void fill(float value)
            {
                if (nHead < nTail)
                    dsp::fill(&pData[nHead], value, nTail - nHead);
            }

            /** Copy data from the specified ShiftBuffer
             *
             * @param src buffer to copy data from
             */
            void copy(const ShiftBuffer *src);
    };

} /* namespace lsp */

#endif /* CORE_UTIL_SHIFTBUFFER_H_ */
