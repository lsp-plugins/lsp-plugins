/*
 * FloatBuffer.h
 *
 *  Created on: 21 авг. 2018 г.
 *      Author: vsadovnikov
 */

#ifndef TEST_FLOATBUFFER_H_
#define TEST_FLOATBUFFER_H_

#include <common/types.h>
#include <test/helpers.h>

namespace test
{
    class FloatBuffer
    {
        private:
            float      *pBuffer;
            uint8_t    *pData;
            size_t      nLength;
            size_t      nAlign;
            bool        bAligned;
            mutable ssize_t  nLastDiff;

        private:
            void allocate(size_t samples, size_t align, bool aligned);

        public:
            explicit FloatBuffer(size_t samples, size_t align = DEFAULT_ALIGN, bool aligned = true);
            explicit FloatBuffer(const FloatBuffer &src);
            ~FloatBuffer();

        public:
            inline void randomize_positive()            { test::randomize_positive(pBuffer, nLength); };
            inline void randomize_negative()            { test::randomize_negative(pBuffer, nLength); };
            inline void randomize_sign()                { test::randomize_sign(pBuffer, nLength); };
            inline void randomize_0to1()                { test::randomize_0to1(pBuffer, nLength); };
            inline void randomize(float min, float max) { test::randomize(pBuffer, nLength, min, max); }
            void randomize()                            { test::randomize_positive(pBuffer, nLength); };
            void fill_zero();
            void copy(const FloatBuffer &buf);
            void copy(const float *buf, size_t count);
            void replace(const float *buf, size_t count);
            void vfill(size_t off, size_t n, ...);

            bool validate() const;
            inline bool valid() const { return validate(); };
            inline bool corrupted() const { return !validate(); };
            bool equals_relative(const FloatBuffer &src, float tolerance = 1e-5f) const;
            bool equals_absolute(const FloatBuffer &src, float tolerance = 1e-5f) const;
            bool equals_adaptive(const FloatBuffer &src, float tolerance = 1e-5f) const;
            void dump(const char *text) const;
            void dump(const char *text, size_t from, size_t count) const;
            void dump_hex(const char *text) const;
            void dump_hex(const char *text, size_t from, size_t count) const;
            inline bool aligned() const { return bAligned; }
            inline size_t size() const { return nLength; }
            inline ssize_t last_diff() const { return nLastDiff; }
            inline float get_diff() const { return (nLastDiff >= 0) ? pBuffer[nLastDiff] : 0.0f; }

        public:
            inline float &operator [] (size_t index) {
                return pBuffer[index];
            }

            inline float operator [] (size_t index) const {
                return pBuffer[index];
            }

            inline float get(size_t index) const {
                return pBuffer[index];
            }

            template <typename T>
                inline T *data()
                {
                    return reinterpret_cast<T *>(pBuffer);
                }

            inline float *data()
            {
                return pBuffer;
            }

            inline float *data(size_t offset)
            {
                return &pBuffer[offset];
            }

            inline operator float *()
            {
                return pBuffer;
            }

            inline operator const float *() const
            {
                return pBuffer;
            }

    };
}

#endif /* TEST_FLOATBUFFER_H_ */
