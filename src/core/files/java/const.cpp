/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 31 авг. 2019 г.
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

#include <core/types.h>
#include <core/files/java/const.h>
#include <core/files/java/Object.h>

namespace lsp
{
    namespace java
    {
        size_t size_of(ftype_t type)
        {
            switch (type)
            {
                case JFT_BYTE:      return sizeof(uint8_t);
                case JFT_CHAR:      return sizeof(lsp_utf16_t);
                case JFT_DOUBLE:    return sizeof(double_t);
                case JFT_FLOAT:     return sizeof(float_t);
                case JFT_INTEGER:   return sizeof(uint32_t);
                case JFT_LONG:      return sizeof(uint64_t);
                case JFT_SHORT:     return sizeof(uint16_t);
                case JFT_BOOL:      return sizeof(uint8_t);
                case JFT_ARRAY:     return sizeof(Object *);
                case JFT_OBJECT:    return sizeof(Object *);
                default:            break;
            }
            return 0;
        }

        size_t aligned_offset(size_t offset, ftype_t type)
        {
            // Currently we can not consider offsets being power of 2, use divides
            size_t size = size_of(type);
            size_t tail = offset % size;
            return (tail) ? offset - tail + size : offset;
        }

        bool is_primitive(ftype_t type)
        {
            switch (type)
            {
                case JFT_BYTE:
                case JFT_CHAR:
                case JFT_DOUBLE:
                case JFT_FLOAT:
                case JFT_INTEGER:
                case JFT_LONG:
                case JFT_SHORT:
                case JFT_BOOL:
                    return true;
                default:
                    break;
            }
            return false;
        }

        bool is_reference(ftype_t type)
        {
            switch (type)
            {
                case JFT_ARRAY:
                case JFT_OBJECT:
                    return true;
                default:
                    break;
            }
            return false;
        }
    }
}


