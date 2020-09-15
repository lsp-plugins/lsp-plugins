/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 23 авг. 2018 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef TEST_BYTEBUFFER_H_
#define TEST_BYTEBUFFER_H_

#include <common/types.h>

namespace test
{
    class ByteBuffer
    {
        private:
            uint8_t    *pBuffer;
            uint8_t    *pData;
            size_t      nLength;
            size_t      nAlign;
            bool        bAligned;

        private:
            void allocate(size_t samples, size_t align, bool aligned);

        public:
            explicit ByteBuffer(size_t samples, size_t align = DEFAULT_ALIGN, bool aligned = true);
            explicit ByteBuffer(const ByteBuffer &src);
            explicit ByteBuffer(const void *data, size_t samples, size_t align = DEFAULT_ALIGN, bool aligned = true);
            ~ByteBuffer();

        public:
            void randomize();
            void fill_zero();

            bool validate() const;
            bool equals(const ByteBuffer &src) const;
            bool equals(const void *src, size_t count) const;
            bool equals(const void *src, size_t offset, size_t count) const;

            void dump(const char *text) const;
            void dump(const char *text, size_t from, size_t count) const;

            inline bool aligned() const { return bAligned; }
            inline bool valid() const { return validate(); };
            inline bool corrupted() const { return !validate(); };

            inline size_t size() const { return nLength; }

        public:
            inline uint8_t &operator [] (size_t index) {
                return pBuffer[index];
            }

            inline uint8_t operator [] (size_t index) const {
                return pBuffer[index];
            }

            template <typename T>
                inline T *data(size_t off = 0)
                {
                    return reinterpret_cast<T *>(&pBuffer[off]);
                }

            inline uint8_t *data(size_t off = 0)
            {
                return &pBuffer[off];
            }

            inline operator uint8_t *()
            {
                return pBuffer;
            }

            inline operator void *()
            {
                return pBuffer;
            }

            inline operator const uint8_t *() const
            {
                return pBuffer;
            }

    };
}

#endif /* TEST_FLOATBUFFER_H_ */
