/*
 * ITask.cpp
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/ITask.h>

namespace lsp
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

    int ITask::run()
    {
        return 0;
    }

} /* namespace lsp */
