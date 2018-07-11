/*
 * common.h
 *
 *  Created on: 23 окт. 2015 г.
 *      Author: sadko
 */

#ifndef _UI_COMMON_H_
#define _UI_COMMON_H_

#include <core/types.h>

#define UI_CONFIG_PORT_PREFIX               "ui:"
#define UI_MOUNT_STUD_PORT_ID               "mount_stud"
#define UI_LAST_VERSION_PORT_ID             "last_version"
#define UI_DLG_DEFAULT_PATH_ID              "dlg_default_path"
#define UI_DLG_SAMPLE_PATH_ID               "dlg_sample_path"
#define UI_DLG_IR_PATH_ID                   "dlg_ir_path"
#define UI_DLG_CONFIG_PATH_ID               "dlg_config_path"

#define MSTUD_PORT                  UI_CONFIG_PORT_PREFIX UI_MOUNT_STUD_PORT_ID
#define VERSION_PORT                UI_CONFIG_PORT_PREFIX UI_LAST_VERSION_PORT_ID
#define DEFAULT_PATH_PORT           UI_CONFIG_PORT_PREFIX UI_DLG_DEFAULT_PATH_ID
#define SAMPLE_PATH_PORT            UI_CONFIG_PORT_PREFIX UI_DLG_SAMPLE_PATH_ID
#define IR_PATH_PORT                UI_CONFIG_PORT_PREFIX UI_DLG_IR_PATH_ID
#define CONFIG_PATH_PORT            UI_CONFIG_PORT_PREFIX UI_DLG_CONFIG_PATH_ID

namespace lsp
{
    using namespace ::lsp::tk;


}

#endif /* _UI_COMMON_H_ */
