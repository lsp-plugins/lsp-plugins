/*
 * Convolver.h
 *
 *  Created on: 29 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_UTIL_CONVOLVER_H_
#define CORE_UTIL_CONVOLVER_H_

#include <core/types.h>
#include <core/util/ShiftBuffer.h>


//#define CONVOLVER_RANK_FFT_SMALL    5                               /* for test purposes                        */
#define CONVOLVER_RANK_FFT_SMALL    8                                 /* buffer of 256 samples (128 effective)    */
//#define CONVOLVER_RANK_FFT_SMALL    4                               /* buffer of 16 samples (8 effective)      */
#define CONVOLVER_RANK_MIN          (CONVOLVER_RANK_FFT_SMALL+1)    /* buffer of 512 samples (256 effective)    */
#define CONVOLVER_RANK_MAX          16                              /* buffer of 8192 samples (4096 effective)  */

namespace lsp
{
    class Convolver
    {
        private:
            Convolver & operator = (const Convolver &);

        private:
            size_t      nConvSize;              // Size of convolution
            size_t      nFrameSize;             // Current frame size
            size_t      nFrameMax;              // Maximum frame size

            float      *vFrame;                 // Input signal frame
            float      *vTempBuf;               // Temporary buffer (real + imaginary)
            float      *vConv;                  // Convolution (real + imaginary)
            float      *vBufferHead;            // Buffer Head
            float      *vBufferTail;            // Buffer Tail
            float      *vBufferPtr;             // Current pointer
            float      *vBufferEnd;             // Buffer End
            float      *vConvFirst;             // First part of convolution in non-FFT mode
            float      *vTask;                  // Task for post-processing (real + imaginary)

            size_t      nRank;                  // FFT rank for convolution
            size_t      nSteps;                 // Number of raising steps
            size_t      nBlocks;                // Number of blocks
            size_t      nBlocksDone;            // Number of blocks done
            size_t      nDirectSize;            // Direct convolution size
            float      *pConv;                  // Task convolution (real + imaginary)
            float      *pTargetPtr;             // Target pointer for task
            uint8_t    *vData;

        public:
            explicit Convolver();
            ~Convolver();

        public:
            /** Initialize convolver
             *
             * @param data convolution data
             * @param count number of samples in convolution
             * @param rank convolution rank
             * @return true on success
             */
            bool init(const float *data, size_t count, size_t rank, float phase);

            /** Destroy convolver
             *
             */
            void destroy();

            /** Process samples
             *
             * @param dst destination buffer
             * @param src source buffer
             * @param count number of samples to process
             */
            void process(float *dst, const float *src, size_t count);

            /** Get the actual convolution size in samples
             *
             * @return actual convolution size in samples
             */
            inline size_t data_size()       { return nConvSize; }
    };

} /* namespace lsp */

#endif /* CORE_UTIL_CONVOLVER_H_ */
