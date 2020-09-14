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

#ifndef CORE_IPC_IEXECUTOR_H_
#define CORE_IPC_IEXECUTOR_H_

#include <core/types.h>
#include <core/ipc/ITask.h>
#include <dsp/dsp.h>

namespace lsp
{
    namespace ipc
    {
        class IExecutor
        {
            protected:
                static inline void change_task_state(ITask *task, ITask::task_state_t state)
                {
                    task->nState    = state;
                }

                static inline void link_task(ITask *tail, ITask *link)
                {
                    tail->pNext     = link;
                    link->pNext     = NULL;
                }

                static inline ITask *next_task(ITask *task)
                {
                    ITask *next     = task->pNext;
                    task->pNext     = NULL;
                    return next;
                }

                static inline void run_task(ITask *task)
                {
                    // Enable DSP context for executor service
                    dsp::context_t ctx;
                    dsp::start(&ctx);

                    task->nState    = ITask::TS_RUNNING;
                    task->nCode     = 0;
                    task->nCode     = task->run();
                    task->nState    = ITask::TS_COMPLETED;

                    dsp::finish(&ctx);
                }

            private:
                IExecutor &operator = (const IExecutor &src);       // Deny copying

            public:
                explicit IExecutor();
                virtual ~IExecutor();

            public:
                /** Submit task for execution
                 *
                 * @param task task to execute
                 * @return true if task was submitted
                 */
                virtual bool submit(ITask *task);

                /** Shutdown executor service
                 * The method must return only when all tasks
                 * have been completed or terminated
                 *
                 */
                virtual void shutdown();
        };

    } /* namespace ipc */
} /* namespace lsp */

#endif /* CORE_IPC_IEXECUTOR_H_ */
