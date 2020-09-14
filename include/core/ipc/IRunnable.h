/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 5 мар. 2019 г.
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

#ifndef INCLUDE_CORE_IPC_IRUNNABLE_H_
#define INCLUDE_CORE_IPC_IRUNNABLE_H_

#include <core/status.h>

namespace lsp
{
    namespace ipc
    {
        /**
         * Runnable interface
         */
        class IRunnable
        {
            private:
                IRunnable & operator = (const IRunnable &src);      // Deny copying

            public:
                explicit IRunnable();
                virtual ~IRunnable();

            public:
                /**
                 * The main method of the runnable interface
                 * @return status of operation
                 */
                virtual status_t run();
        };
    
    } /* namespace ipc */
} /* namespace lsp */

#endif /* INCLUDE_CORE_IPC_IRUNNABLE_H_ */
