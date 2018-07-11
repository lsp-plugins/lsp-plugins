/*
 * IClipboard.cpp
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#include <ui/ws/ws.h>
#include <locale.h>

namespace lsp
{
    namespace ws
    {
        
        IClipboard::IClipboard()
        {
            nError          = STATUS_OK;
            nReferences     = 1;
        }
        
        IClipboard::~IClipboard()
        {
            nError          = STATUS_OK;
            nReferences     = 0;
        }

        status_t IClipboard::acquire()
        {
            ++nReferences;
            return STATUS_OK;
        }

        status_t IClipboard::close()
        {
            if ((--nReferences) == 0)
                delete this;
            return STATUS_OK;
        }

        IInputStream *IClipboard::read(const char *ctype)
        {
            nError      = STATUS_NOT_IMPLEMENTED;
            return NULL;
        }

        size_t IClipboard::targets()
        {
            return 0;
        }

        const char *IClipboard::target(size_t i)
        {
            return NULL;
        }
    
    } /* namespace ws */
} /* namespace lsp */
