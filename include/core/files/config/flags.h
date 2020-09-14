/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 июл. 2019 г.
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

#ifndef CORE_FILES_CONFIG_FLAGS_H_
#define CORE_FILES_CONFIG_FLAGS_H_

namespace lsp
{
    namespace config
    {
        enum serialize_flags_t
        {
            SF_NONE         = 0,

            SF_TYPE_NATIVE  = 0x00,
            SF_TYPE_I32     = 0x01,
            SF_TYPE_U32     = 0x02,
            SF_TYPE_I64     = 0x03,
            SF_TYPE_U64     = 0x04,
            SF_TYPE_F32     = 0x05,
            SF_TYPE_F64     = 0x06,
            SF_TYPE_STR     = 0x07,
            SF_TYPE_BLOB    = 0x08,

            SF_TYPE_MASK    = 0x0f,

            SF_QUOTED       = 1 << 4,
        };
    }
}


#endif /* CORE_FILES_CONFIG_FLAGS_H_ */
