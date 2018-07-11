/*
 * IExecutor.cpp
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/IExecutor.h>

namespace lsp
{
    IExecutor::IExecutor()
    {
    }

    IExecutor::~IExecutor()
    {
    }

    bool IExecutor::submit(ITask *task)
    {
        return false;
    }

    void IExecutor::shutdown()
    {
    }

} /* namespace lsp */
