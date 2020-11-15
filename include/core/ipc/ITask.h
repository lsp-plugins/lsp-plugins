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

#ifndef CORE_IPC_ITASK_H_
#define CORE_IPC_ITASK_H_

#include <core/status.h>
#include <core/ipc/IRunnable.h>

namespace lsp
{
    namespace ipc
    {
        class ITask: public IRunnable
        {
            public:
                enum task_state_t
                {
                    TS_IDLE,
                    TS_SUBMITTED,
                    TS_RUNNING,
                    TS_COMPLETED
                };

            protected:
                // Task linking
                ITask      *pNext;
                int         nCode;

                // Task state
                volatile task_state_t    nState;

                // Executor service
                friend class IExecutor;

                static inline bool successful(int code)     { return code == STATUS_OK; };

            public:
                ITask();
                virtual ~ITask();

            public:
                virtual status_t run();

            public:
                /** Check that task status is idle
                 *
                 * @return true if task status is idle
                 */
                inline bool idle() const        { return nState == TS_IDLE;         };

                /** Check that task status is submitted
                 *
                 * @return true if task status is submitted
                 */
                inline bool submitted() const   { return nState == TS_SUBMITTED;    };

                /** Check that task status is running
                 *
                 * @return true if task status is running
                 */
                inline bool running() const     { return nState == TS_RUNNING;      };

                /** Check that task status is completed
                 *
                 * @return true if task status is completed
                 */
                inline bool completed() const   { return nState == TS_COMPLETED;    };

                /** Check that execution was successful
                 *
                 * @return true if execution was successful;
                 */
                inline bool successful() const  { return successful(nCode);         };

                /** Get last execution code
                 *
                 * @return last execution code
                 */
                inline int code() const         { return nCode;                     };

                /** Get current state of task
                 *
                 * @return current task state
                 */
                inline task_state_t state() const {return nState;                   };

                /** Reset task state
                 *
                 * @return task state
                 */
                inline bool reset()
                {
                    if (nState != TS_COMPLETED)
                        return false;
                    nState      = TS_IDLE;
                    return true;
                }
        };

    } /* namespace ipc */

} /* namespace lsp */

#endif /* CORE_IPC_ITASK_H_ */
