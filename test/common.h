/*
 * common.h
 *
 *  Created on: 07 апр. 2016 г.
 *      Author: sadko
 */

#ifndef TEST_COMMON_H_
#define TEST_COMMON_H_

#include <core/types.h>
#include <stdio.h>
#include <stdlib.h>

namespace test
{
    using namespace lsp;

    class FBuffer
    {
        private:
            float      *pBuffer;
            uint8_t    *pData;
            size_t      nLength;

            void build(size_t samples, bool aligned = true)
            {
                size_t alloc    = sizeof(uint32_t)*2 + samples*sizeof(float) + DEFAULT_ALIGN;
                nLength         = samples;
                pData           = new uint8_t[alloc];
                if (aligned)
                    pBuffer         = reinterpret_cast<float *>(ALIGN_PTR(&pData[sizeof(uint32_t)], DEFAULT_ALIGN));
                else
                {
                    pBuffer         = reinterpret_cast<float *>(&pData[sizeof(uint32_t)]);
                    if (IS_PTR_ALIGNED(pBuffer))
                        pBuffer++;
                }

                // Mark the head and the tail of the buffer with signatures
                uint32_t key    = uint32_t(ptrdiff_t(this));
                uint32_t *ptr   = reinterpret_cast<uint32_t *>(&pBuffer[-1]);
                *ptr            = 0x55aa5a5a ^ key;
                ptr             = reinterpret_cast<uint32_t *>(&pBuffer[nLength]);
                *ptr            = 0xaa55a5a5 ^ key;
            }

        public:
            FBuffer(size_t samples, bool aligned = true)
            {
                build(samples, aligned);

                // Initialize buffer with random data
                randomize();
            }

            FBuffer(const FBuffer &src)
            {
                build(src.nLength, IS_PTR_ALIGNED(src.pBuffer));
                for (size_t i=0; i<nLength; ++i)
                    pBuffer[i]      = src.pBuffer[i];
            }

            ~FBuffer()
            {
                if (pData != NULL)
                {
                    delete  [] pData;
                    pData = NULL;
                }
                pBuffer = NULL;
            }

            void randomize_positive()
            {
                for (size_t i=0; i<nLength; ++i)
                    pBuffer[i] = (float(rand())/RAND_MAX) + 0.001f;
            }

            void randomize_negative()
            {
                for (size_t i=0; i<nLength; ++i)
                    pBuffer[i] = - ((float(rand())/RAND_MAX) + 0.001f);
            }

            void randomize_sign()
            {
                for (size_t i=0; i<nLength; ++i)
                {
                    float tmp = (float(rand())/RAND_MAX) + 0.001f;
                    pBuffer[i] = (rand() >= (RAND_MAX >> 1)) ? tmp : -tmp;
                }
            }

            void randomize()
            {
                randomize_positive();
            }

            inline float *data()
            {
                return pBuffer;
            }

            inline operator float *()
            {
                return pBuffer;
            }

            inline operator const float *() const
            {
                return pBuffer;
            }

            inline bool validate() const
            {
                uint32_t key            = uint32_t(ptrdiff_t(this));
                const uint32_t *ptr     = reinterpret_cast<const uint32_t *>(&pBuffer[-1]);
                if (*ptr != (0x55aa5a5a ^ key))
                    return false;
                ptr                     = reinterpret_cast<const uint32_t *>(&pBuffer[nLength]);
                return (*ptr == (0xaa55a5a5 ^ key));
            }

            inline bool compare(const FBuffer &src, float tolerance = 1e-6f) const
            {
                if (src.nLength != nLength)
                    return false;
                if (!(validate() && src.validate()))
                    return false;
                const float *a = pBuffer, *b = src.pBuffer;
                for (size_t i=0; i<nLength; ++i)
                {
                    if (fabs(1.0f - b[i]/a[i]) >= tolerance)
                        return false;
                }
                return true;
            }

            void dump(const char *text)
            {
                printf("%s: ", text);
                for (size_t i=0; i<nLength; ++i)
                    printf("%.3f ", pBuffer[i]);
                printf("\n");
            }

    };

    void dump_data(const float *data, size_t count)
    {
        for (size_t i=0; i<count; ++i)
        {
            if (i > 0)
                printf(", ");
            printf("%.3f", data[i]);
        }
        printf("\n");
    }
}

#endif /* TEST_COMMON_H_ */
