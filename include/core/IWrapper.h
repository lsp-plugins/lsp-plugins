/*
 * IWrapper.h
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_IWRAPPER_H_
#define CORE_IWRAPPER_H_

#include <core/ipc/ITask.h>
#include <core/ipc/IExecutor.h>
#include <core/ICanvas.h>
#include <core/port_data.h>
#include <core/KVTStorage.h>

namespace lsp
{
    class IWrapper
    {
        public:
            explicit IWrapper();
            virtual ~IWrapper();

        public:
            /** Get executor service
             *
             * @return executor service
             */
            virtual ipc::IExecutor *get_executor();

            /** Query for inline display drawing
             *
             */
            virtual void query_display_draw();

            /** Get current time position
             *
             * @return current time position
             */
            virtual const position_t *position();

            /**
             * Create/resize canvas
             * @param cv valid pointer to ICanvas object or NULL if there's none
             * @param width the requested width of canvas
             * @param height the requested height of canvas
             * @return the pointer to valid canvas object or NULL on error
             */
            virtual ICanvas *create_canvas(ICanvas *&cv, size_t width, size_t height);

            /**
             * Lock KVT storage and return pointer to the storage,
             * this is non-RT-safe operation
             * @return pointer to KVT storage or NULL if KVT is not supported
             */
            virtual KVTStorage *kvt_lock();

            /**
             * Try to lock KVT storage and return pointer to the storage on success
             * @return pointer to KVT storage or NULL
             */
            virtual KVTStorage *kvt_trylock();

            /**
             * Release the KVT storage
             * @return true on success
             */
            virtual bool kvt_release();
    };

} /* namespace lsp */

#endif /* CORE_IWRAPPER_H_ */
