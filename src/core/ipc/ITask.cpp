/*
 * ITask.cpp
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/ipc/ITask.h>

namespace lsp
{
    namespace ipc
    {
        ITask::ITask()
        {
            nState  = TS_IDLE;
            nCode   = 0;
            pNext   = NULL;
        }

        ITask::~ITask()
        {
        }

        status_t ITask::run()
        {
            return 0;
        }
    }

} /* namespace lsp */
