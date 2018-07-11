/*
 * IWrapper.cpp
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#include <core/types.h>
#include <core/IWrapper.h>

namespace lsp
{
    IWrapper::IWrapper()
    {
    }

    IWrapper::~IWrapper()
    {
    }

    IExecutor *IWrapper::get_executor()
    {
        return NULL;
    }
} /* namespace lsp */
