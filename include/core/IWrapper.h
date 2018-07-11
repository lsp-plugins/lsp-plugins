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
    };

} /* namespace lsp */

#endif /* CORE_IWRAPPER_H_ */
