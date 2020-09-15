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

#ifndef CONTAINER_LV2_EXECUTOR_H_
#define CONTAINER_LV2_EXECUTOR_H_

#include <core/ipc/IExecutor.h>

namespace lsp
{
    class LV2Executor: public ipc::IExecutor
    {
        private:
            static const uint32_t magic = (uint32_t('L') << 24) | (uint32_t('V') << 16) | (uint32_t('2') << 8) | (uint32_t('E') << 0);

            LV2_Worker_Schedule *sched;

            typedef struct task_descriptor_t
            {
                uint32_t        magic;
                ipc::ITask     *task;
            } task_descriptor_t;

        public:
            LV2Executor(LV2_Worker_Schedule *schedule)
            {
                sched       = schedule;
            }

            ~LV2Executor()
            {
                sched       = NULL;
            }

        public:
            virtual bool submit(ipc::ITask *task)
            {
                // Check state of task
                if (!task->idle())
                    return false;

                // Try to submit task
                task_descriptor_t descr = { magic, task };
                change_task_state(task, ipc::ITask::TS_SUBMITTED);
                if (sched->schedule_work(sched->handle, sizeof(task_descriptor_t), &descr) == LV2_WORKER_SUCCESS)
                    return true;

                // Failed to submit task, return status back
                change_task_state(task, ipc::ITask::TS_IDLE);
                return false;
            }

            inline void run_job(
                LV2_Worker_Respond_Handle   handle,
                LV2_Worker_Respond_Function respond,
                uint32_t                    size,
                const void*                 data
            )
            {
                // Validate structure
                if (size != sizeof(task_descriptor_t))
                    return;
                const task_descriptor_t *descr = reinterpret_cast<const task_descriptor_t *>(data);
                if (descr->magic != magic)
                    return;

                // Run task
                run_task(descr->task);
            }
    };
}


#endif /* CONTAINER_LV2_EXECUTOR_H_ */
