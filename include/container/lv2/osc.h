/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 13 июн. 2019 г.
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

#ifndef CONTAINER_LV2_OSC_H_
#define CONTAINER_LV2_OSC_H_

// The URI definitions originally taken from OpenMusicKontrollers
#define LV2_OSC_URI                     "http://open-music-kontrollers.ch/lv2/osc"
#define LV2_OSC_PREFIX                  LV2_OSC_URI "#"
#define LV2_RAW_OSC_URI                 "http://opensoundcontrol.org/spec-1_0"

#define LV2_OSC__Bundle                 LV2_OSC_PREFIX "Bundle"             /* atom object type     */
#define LV2_OSC__bundleTimetag          LV2_OSC_PREFIX "bundleTimetag"      /* atom object property */
#define LV2_OSC__bundleItems            LV2_OSC_PREFIX "bundleItems"

#define LV2_OSC__Message                LV2_OSC_PREFIX "Message"            /* atom object type     */
#define LV2_OSC__messagePath            LV2_OSC_PREFIX "messagePath"        /* atom object property */
#define LV2_OSC__messageArguments       LV2_OSC_PREFIX "messageArguments"   /* atom object property */

#define LV2_OSC__Timetag                LV2_OSC_PREFIX "Timetag"            /* atom object type     */
#define LV2_OSC__timetagIntegral        LV2_OSC_PREFIX "timetagIntegral"    /* atom object property */
#define LV2_OSC__timetagFraction        LV2_OSC_PREFIX "timetagFraction"    /* atom object property */

#define LV2_OSC__Nil                    LV2_OSC_PREFIX "Nil"                /* atom literal type    */
#define LV2_OSC__Impulse                LV2_OSC_PREFIX "Impulse"            /* atom literal type    */
#define LV2_OSC__Char                   LV2_OSC_PREFIX "Char"               /* atom literal type    */
#define LV2_OSC__RGBA                   LV2_OSC_PREFIX "RGBA"               /* atom literal type    */

#define LV2_OSC__RawPacket              LV2_RAW_OSC_URI "Packet"            /* raw OSC packet data  */

#endif /* CONTAINER_LV2_OSC_H_ */
