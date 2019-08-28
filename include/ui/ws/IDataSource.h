/*
 * IDataSource.h
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_WS_IDATASOURCE_H_
#define UI_WS_IDATASOURCE_H_

#include <core/io/IInStream.h>

namespace lsp
{
    namespace ws
    {
        /**
         * Data source interface, implements some data binding which can be present in
         * different formats depending on the MIME type.
         *
         * This should ALWAYS be a dynamically-allocated object respectively to
         * the release() method which will perform delete of the object when number of
         * references reaches non-positive value.
         *
         * IDataSource should ALWAYS implement fetch() and sink() methods, not one of them.
         */
        class IDataSource
        {
            protected:
                ssize_t     nReferences;
                char      **vMimes;

            public:
                explicit IDataSource(const char *const *mimes);
                virtual ~IDataSource();

            public:
                /**
                 * Return NULL-terminated list of supported MIME types
                 * @return number of supported MIME types
                 */
                inline const char * const *mime_types() const { return vMimes; };

                /**
                 * Initiate transfer of the contents of the data source to the specified data fetch.
                 * Fetcher may be performed in asynchronous mode
                 *
                 * @param mime requested MIME type
                 * @param fetch pointer to store pointer to the fetching interface
                 * @return status of operation
                 */
                virtual io::IInStream   *open(const char *mime);

                /**
                 * Acquire data source for usage
                 * @return number of references
                 */
                size_t                  acquire();

                /**
                 * Get number of references to the data source
                 * @return number of references to the data source
                 */
                inline size_t           references() const { return nReferences; };

                /**
                 * Release data source
                 * @return number of references to the data source
                 */
                size_t                  release();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_WS_IDATASOURCE_H_ */
