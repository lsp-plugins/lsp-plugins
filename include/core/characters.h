/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 18 марта 2016 г.
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

#ifndef CORE_CHARACTERS_H_
#define CORE_CHARACTERS_H_

// Special character coding

// Special symbols
#define ASCII_A_UMLAUT_SMALL                "\x84"
#define ASCII_A_UMLAUT_LARGE                "\x8e"
#define ASCII_U_UMLAUT_SMALL                "\x81"
#define ASCII_U_UMLAUT_LARGE                "\x9a"
#define ASCII_O_UMLAUT_SMALL                "\x94"
#define ASCII_O_UMLAUT_LARGE                "\x99"
#define ASCII_ESZETT                        "\xe1"

#define ASCII_SHARP                         "#"
#define ASCII_FLAT                          "b"

#define UTF8_SHARP                          "#"
//#define UTF8_SHARP                          "\xe2\x99\xaf"
#define UTF8_FLAT                           "\xe2\x99\xad"

#endif /* CORE_CHARACTERS_H_ */
