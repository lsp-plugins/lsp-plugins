/*
 * Convolver.h
 *
 *  Created on: 29 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_CONVOLVER_H_
#define CORE_CONVOLVER_H_

#include <core/types.h>
#include <core/ShiftBuffer.h>

//#define CONVOLVER_RANK_FFT_SMALL    4                               /* for test purposes                        */
#define CONVOLVER_RANK_FFT_SMALL    8                                 /* buffer of 256 samples (128 effective)    */
//#define CONVOLVER_RANK_FFT_SMALL    4                               /* buffer of 16 samples (8 effective)      */
#define CONVOLVER_RANK_MIN          (CONVOLVER_RANK_FFT_SMALL+1)    /* buffer of 512 samples (256 effective)    */
#define CONVOLVER_RANK_MAX          16                              /* buffer of 8192 samples (4096 effective)  */

namespace lsp
{
    class Convolver
    {
        private:
//            size_t      nSmallParts;        // Number of small convolution parts
//            size_t      nLargeParts;        // Number of large convolution parts
//            size_t      nHistParts;         // Number of history parts
//            size_t      nFrameSize;         // Current number of samples in the frame
//            size_t      nFrameCapacity;     // Maximum frame capacity in samples
//            size_t      nSmallFFT;          // Small FFT size
//            size_t      nLargeFFT;          // Large FFT size
//
//            float      *vConvRe;            // Buffer to store real parts of each convolution part
//            float      *vConvIm;            // Buffer to store imaginary parts of each convolution part
//            float      *vBuffer;            // Buffer to store the tail of convolution
//            float      *vTmpBuf;            // Temporary buffer
//            float      *vFrame;             // Frame for collecting samples
//            float      *vZeroFrame;         // Zero frame
//            float      *vFirstPart;         // First part in non-FFT presentation
//
//            float      *pBufHead;           // Buffer head
//            float      *pBufThresh;         // Buffer threshold
//
            size_t      nFrameSize;             // Current frame size
            size_t      nFrameMax;              // Maximum frame size

            float      *vFrameRe;               // Real part of frame
            float      *vFrameIm;               // Imaginary part of frame
            float      *vFftRe;                 // FFT temporary buffer (real)
            float      *vFftIm;                 // FFT temporary buffer (imaginary)
            float      *vTempRe;                // Temporary buffer (real)
            float      *vTempIm;                // Temporary buffer (imaginary)
            float      *vConvRe;                // Convolution (real)
            float      *vConvIm;                // Convolution (imaginary)
            float      *vBufferHead;            // Buffer Head
            float      *vBufferTail;            // Buffer Tail
            float      *vBufferPtr;             // Current pointer
            float      *vBufferEnd;             // Buffer End
            float      *vConvFirst;             // First part of convolution in non-FFT mode
            float      *vTaskRe;                // Task for post-processing (real)
            float      *vTaskIm;                // Task for post-processing (imaginary)

            size_t      nRank;                  // FFT rank for convolution
            size_t      nSteps;                 // Number of raising steps
            size_t      nBlocks;                // Number of blocks
            size_t      nBlocksDone;            // Number of blocks done
            float      *pConvRe;                // Task convolution (re)
            float      *pConvIm;                // Task convolution (im)
            float      *pTargetPtr;             // Target pointer for task
            uint8_t    *vData;

        public:
            Convolver();
            ~Convolver();

        public:
            /** Initialize convolver
             *
             * @param data convolution data
             * @param count number of samples in convolution
             * @param rank convolution rank
             * @return true on success
             */
            bool init(const float *data, size_t count, size_t rank);

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
    };

} /* namespace lsp */

#endif /* CORE_CONVOLVER_H_ */
