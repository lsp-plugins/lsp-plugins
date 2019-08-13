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

    ipc::IExecutor *IWrapper::get_executor()
    {
        return NULL;
    }

    void IWrapper::query_display_draw()
    {
    }

    const position_t *IWrapper::position()
    {
        return NULL;
    }

    ICanvas *IWrapper::create_canvas(ICanvas *&cv, size_t width, size_t height)
    {
        return NULL;
    }

    KVTStorage *IWrapper::kvt_lock()
    {
        return NULL;
    }

    KVTStorage *IWrapper::kvt_trylock()
    {
        return NULL;
    }

    bool IWrapper::kvt_release()
    {
        return false;
    }

} /* namespace lsp */
