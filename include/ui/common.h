/*
 * common.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_COMMON_H_
#define _UI_COMMON_H_

#include <core/types.h>

// UI configuration ports
#define UI_CONFIG_PORT_PREFIX               "ui:"
#define UI_MOUNT_STUD_PORT_ID               "mount_stud"
#define UI_LAST_VERSION_PORT_ID             "last_version"
#define UI_DLG_DEFAULT_PATH_ID              "dlg_default_path"
#define UI_DLG_SAMPLE_PATH_ID               "dlg_sample_path"
#define UI_DLG_IR_PATH_ID                   "dlg_ir_path"
#define UI_DLG_CONFIG_PATH_ID               "dlg_config_path"
#define UI_DLG_MODEL3D_PATH_ID              "dlg_model3d_path"
#define UI_DLG_REW_PATH_ID                  "dlg_rew_path"
#define UI_R3D_BACKEND_PORT_ID              "r3d_backend"
#define UI_LANGUAGE_PORT_ID                 "language"

#define MSTUD_PORT                          UI_CONFIG_PORT_PREFIX UI_MOUNT_STUD_PORT_ID
#define VERSION_PORT                        UI_CONFIG_PORT_PREFIX UI_LAST_VERSION_PORT_ID
#define DEFAULT_PATH_PORT                   UI_CONFIG_PORT_PREFIX UI_DLG_DEFAULT_PATH_ID
#define SAMPLE_PATH_PORT                    UI_CONFIG_PORT_PREFIX UI_DLG_SAMPLE_PATH_ID
#define IR_PATH_PORT                        UI_CONFIG_PORT_PREFIX UI_DLG_IR_PATH_ID
#define CONFIG_PATH_PORT                    UI_CONFIG_PORT_PREFIX UI_DLG_CONFIG_PATH_ID
#define MODEL3D_PATH_PORT                   UI_CONFIG_PORT_PREFIX UI_DLG_MODEL3D_PATH_ID
#define R3D_BACKEND_PORT                    UI_CONFIG_PORT_PREFIX UI_R3D_BACKEND_PORT_ID
#define LANGUAGE_PORT                       UI_CONFIG_PORT_PREFIX UI_LANGUAGE_PORT_ID

// Special widget identifiers
#define WUID_MAIN_MENU                      "main_menu"
#define WUID_EXPORT_MENU                    "export_menu"
#define WUID_IMPORT_MENU                    "import_menu"
#define WUID_LANGUAGE_MENU                  "language_menu"

// Special ports for handling current time
#define TIME_PORT_PREFIX                    "time:"
#define TIME_SAMPLE_RATE_PORT               "sr"
#define TIME_SPEED_PORT                     "speed"
#define TIME_FRAME_PORT                     "frame"
#define TIME_NUMERATOR_PORT                 "num"
#define TIME_DENOMINATOR_PORT               "denom"
#define TIME_BEATS_PER_MINUTE_PORT          "bpm"
#define TIME_TICK_PORT                      "tick"
#define TIME_TICKS_PER_BEAT_PORT            "tpb"

#define SAMPLE_RATE_PORT                    TIME_PORT_PREFIX TIME_SAMPLE_RATE_PORT
#define SPEED_PORT                          TIME_PORT_PREFIX TIME_SPEED_PORT
#define FRAME_PORT                          TIME_PORT_PREFIX TIME_FRAME_PORT
#define NUMERATOR_PORT                      TIME_PORT_PREFIX TIME_NUMERATOR_PORT
#define DENOMINATOR_PORT                    TIME_PORT_PREFIX TIME_DENOMINATOR_PORT
#define BEATS_PER_MINUTE_PORT               TIME_PORT_PREFIX TIME_BEATS_PER_MINUTE_PORT
#define TICK_PORT                           TIME_PORT_PREFIX TIME_TICK_PORT
#define TICKS_PER_BEAT_PORT                 TIME_PORT_PREFIX TIME_TICKS_PER_BEAT_PORT

namespace lsp
{
    using namespace ::lsp::tk;


}

#endif /* _UI_COMMON_H_ */
