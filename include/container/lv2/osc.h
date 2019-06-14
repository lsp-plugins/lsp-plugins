/*
 * osc.h
 *
 *  Created on: 13 июн. 2019 г.
 *      Author: sadko
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
