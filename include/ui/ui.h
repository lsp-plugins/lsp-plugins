/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 20 окт. 2015 г.
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

#ifndef _UI_UI_H_
#define _UI_UI_H_

#define LSP_UI_SIDE

#include <core/types.h>
#include <core/status.h>

#include <stddef.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Windowing system
#include <ui/ws/ws.h>

// Toolkit system
#include <ui/tk/tk.h>

// Control system
#include <ui/ctl/ctl.h>

namespace lsp
{
    class ui_builder;

    using namespace ::lsp::ws;
    using namespace ::lsp::tk;
    using namespace ::lsp::ctl;
}

// Misc data structures
#include <data/cvector.h>

// Core
#include <core/types.h>
#include <core/debug.h>

// Metadata
#include <metadata/metadata.h>

// Common definitions
#include <ui/common.h>
#include <ui/IUIWrapper.h>

// Common plugin UI
#include <ui/plugin_ui.h>
#include <ui/ui_builder.h>

// Particular plugin UI
#include <ui/plugins/test_plugin_ui.h>
#include <ui/plugins/room_builder_ui.h>
#include <ui/plugins/para_equalizer_ui.h>
#include <ui/plugins/multisampler_ui.h>

#endif /* _UI_UI_H_ */
