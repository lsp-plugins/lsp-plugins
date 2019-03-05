/*
 * executor.h
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
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
