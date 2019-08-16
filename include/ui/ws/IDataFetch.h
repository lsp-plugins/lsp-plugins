/*
 * IDataFetch.h
 *
 *  Created on: 16 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_WS_IDATAFETCH_H_
#define UI_WS_IDATAFETCH_H_

namespace lsp
{
    namespace ws
    {
        /**
         * Data fetch interface for fetching data from DataSource
         */
        class IDataFetch
        {
            public:
                explicit IDataFetch(const char *mime);
                virtual ~IDataFetch();

            public:
                /**
                 * Read amount of bytes from the fetch
                 * @param buf buffer to store data
                 * @param count number of bytes to read from fetch
                 * @return number of bytes read or negative error code,
                 * including STATUS_EOF as mark of end of transfer
                 */
                virtual ssize_t read(void *buf, size_t count);

                /**
                 * This method is called when the IDataSource aborts fetch
                 * @return status of operation
                 */
                virtual status_t abort();

                /**
                 * Commit: the data has been successfully committed
                 */
                virtual status_t close();

        };
    
    } /* namespace ws */
} /* namespace lsp */

#endif /* UI_WS_IDATAFETCH_H_ */
