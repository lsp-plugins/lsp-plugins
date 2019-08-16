/*
 * IDataSource.h
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_WS_IDATASOURCE_H_
#define UI_WS_IDATASOURCE_H_

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

            public:
                explicit IDataSource();
                virtual ~IDataSource();

            public:
                /**
                 * Return number of supported MIME types for export
                 * @return number of supported MIME types
                 */
                virtual size_t      mime_types();

                /**
                 * Return the corresponding MIME type by the index
                 * @param id MIME type by the index
                 * @return MIME type or NULL
                 */
                virtual const char *mime_type(size_t id);

                /**
                 * Initiate transfer of the contents of the data source to the specified data sink.
                 * Sink writes may be performed in asynchronous mode, so returning from the
                 * sink() does not guarantee that the target sink will contain actual data.
                 * The sink should consider that transfer is complete only when commit() or abort()
                 * methods are called.
                 *
                 * @param mime requested MIME type
                 * @param sink data sink
                 * @return status of operation
                 */
                virtual status_t    sink(const char *mime, IDataSink *sink);

//                /**
//                 * Initiate transfer of the contents of the data source to the specified data fetch.
//                 * Fetcher may be performed in asynchronous mode
//                 *
//                 * @param mime requested MIME type
//                 * @param fetch pointer to store pointer to the fetching interface
//                 * @return status of operation
//                 */
//                virtual status_t    fetch(const char *mime, IDataFetch **fetch);

                /**
                 * Abort all currently pending sink and fetch operations
                 * @return status of operation
                 */
                virtual status_t    abort();

                /**
                 * Acquire data source for usage
                 * @return number of references
                 */
                size_t              acquire();

                /**
                 * Get number of references to the data source
                 * @return number of references to the data source
                 */
                inline size_t       references() const { return nReferences; };

                /**
                 * Release data source
                 * @return number of references to the data source
                 */
                size_t              release();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_WS_IDATASOURCE_H_ */
