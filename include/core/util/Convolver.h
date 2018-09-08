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

#if 1

#define CONVOLVER_RANK_FFT_SMALL    5                               /* for test purposes                        */
//#define CONVOLVER_RANK_FFT_SMALL    8                                 /* buffer of 256 samples (128 effective)    */
//#define CONVOLVER_RANK_FFT_SMALL    4                               /* buffer of 16 samples (8 effective)      */
#define CONVOLVER_RANK_MIN          (CONVOLVER_RANK_FFT_SMALL+1)    /* buffer of 512 samples (256 effective)    */
#define CONVOLVER_RANK_MAX          16                              /* buffer of 8192 samples (4096 effective)  */

namespace lsp
{
    class Convolver
    {
        private:
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
    };

} /* namespace lsp */


#else

//#define CONVOLVER_RANK_SMALLEST     4                               /* for test purposes                        */
#define CONVOLVER_RANK_SMALLEST     7                                 /* buffer of 128 samples (64 effective)     */
#define CONVOLVER_RANK_MIN          (CONVOLVER_RANK_SMALLEST+1)     /* buffer of 512 samples (256 effective)    */
#define CONVOLVER_RANK_MAX          16                              /* buffer of 65536 samples (32768 effective)  */
#define CONVOLVER_PLANS             (CONVOLVER_RANK_MAX - CONVOLVER_RANK_SMALLEST + 1)

namespace lsp
{
    class Convolver
    {
        protected:
#pragma pack(push, 1)
            typedef struct plan_t
            {
                size_t          nCount;         // Number of blocks
                float          *pConv;          // Convolution
                uint32_t        nCkSum;         // Checksum of convolution
            } plan_t;
#pragma pack(pop)

        protected:
            float      *vSmallConv;             // Small non-FFT convolution
            float      *vSmallFft;              // Small FFT convolution
            float      *vFrame;                 // Frame

            float      *vFftBuf;                // FFT buffer
            float      *vTmpBuf;                // Temporary buffer

            float      *vBufferHead;            // Convolution buffer head
            float      *pBufferPtr;             // Convolution buffer pointer
            float      *pBufferThresh;          // Convolution buffer threshold
            float      *pBufferEnd;             // Convolution buffer end

            float      *vTaskBuf;               // Task buffer
            float      *pTaskPtr;               // Task pointer
            size_t      nTaskDone;              // Number of items left

            size_t      nFrameSize;             // Current frame size
            size_t      nFrameMax;              // Maximum frame size
            size_t      nRank;
            ssize_t     nDump;                  // Dump
            float       fInitPhase;             // Initial phase

            uint8_t    *vData;
            plan_t      vPlans[CONVOLVER_PLANS];

        public:
            Convolver();
            ~Convolver();

        public:
            /** Initialize convolver
             *
             * @param data convolution data
             * @param count number of samples in convolution
             * @param rank convolution rank
             * @param phase initial convolver phase
             * @return true on success
             */
            bool init(const float *data, size_t count, size_t rank, float phase = 0);

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

#endif

#endif /* CORE_UTIL_CONVOLVER_H_ */
