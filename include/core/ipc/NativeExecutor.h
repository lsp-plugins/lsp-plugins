/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 27 янв. 2016 г.
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

#ifndef CORE_NATIVEEXECUTOR_H_
#define CORE_NATIVEEXECUTOR_H_

#include <dsp/atomic.h>
#include <core/ipc/Thread.h>
#include <core/ipc/IExecutor.h>
#include <core/ipc/ITask.h>

namespace lsp
{
    namespace ipc
    {
        class NativeExecutor: public IExecutor
        {
            private:
                Thread              hThread;
                ITask              *pHead;
                ITask              *pTail;
                atomic_t            nLock;

                static status_t     execute(void *params);
                void    run();

            private:
                NativeExecutor &operator = (const NativeExecutor &src); // Deny copying

            public:
                explicit NativeExecutor();
                virtual ~NativeExecutor();

            public:
                status_t start();

                virtual bool submit(ITask *task);

                virtual void shutdown();
        };
    }
}

#endif /* CORE_NATIVEEXECUTOR_H_ */
