/*
 * IWrapper.h
 *
 *  Created on: 27 янв. 2016 г.
 *      Author: sadko
 */

#ifndef CORE_IWRAPPER_H_
#define CORE_IWRAPPER_H_

#include <core/ITask.h>
#include <core/IExecutor.h>
#include <core/ICanvas.h>

namespace lsp
{
    class IWrapper
    {
        public:
            IWrapper();
            virtual ~IWrapper();

        public:
            /** Get executor service
             *
             * @return executor service
             */
            virtual IExecutor *get_executor();

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
    };

} /* namespace lsp */

#endif /* CORE_IWRAPPER_H_ */
