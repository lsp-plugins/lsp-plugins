/*
 * IRunnable.cpp
 *
 *  Created on: 5 мар. 2019 г.
 *      Author: sadko
 */

#include <core/ipc/IRunnable.h>

namespace lsp
{
    namespace ipc
    {
        IRunnable::IRunnable()
        {
        }
        
        IRunnable::~IRunnable()
        {
        }

        status_t IRunnable::run()
        {
            return STATUS_OK;
        }
    
    } /* namespace ipc */
} /* namespace lsp */
