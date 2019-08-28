/*
 * InMemoryStream.h
 *
 *  Created on: 21 авг. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_IO_INMEMORYSTREAM_H_
#define CORE_IO_INMEMORYSTREAM_H_

#include <core/io/IInStream.h>

namespace lsp
{
    namespace io
    {
        
        class InMemoryStream: public IInStream
        {
            public:
                enum drop_t
                {
                    DROP_NONE,
                    DROP_FREE,
                    DROP_DELETE,
                    DROP_ARR_DELETE
                };

            protected:
                uint8_t    *pData;
                size_t      nOffset;
                size_t      nSize;
                drop_t      enDrop;

            public:
                /**
                 * Create memory input stream
                 * @param data associated buffer
                 * @param size size of buffer
                 * @param drop the method how to drop data on close()
                 */
                explicit InMemoryStream();
                explicit InMemoryStream(void *data, size_t size, drop_t drop = DROP_NONE);
                virtual ~InMemoryStream();

            public:
                /**
                 * Wrap the memory buffer, drop previous buffer using specified mechanism
                 * @param data associated buffer
                 * @param size size of buffer
                 * @param drop the method how to drop data on close()
                 */
                void wrap(void *data, size_t size, drop_t drop = DROP_NONE);

                /**
                 * Get the memory contents
                 * @return memory contents
                 */
                inline const uint8_t *data() const { return pData; }

                /**
                 * Drop data using specified drop mechanism
                 * @return true if data has been dropped
                 */
                bool drop(drop_t drop);

                /**
                 * Drop data using default mechanism specified in constructor
                 * @return true if data has been dropped
                 */
                inline bool drop() { return drop(enDrop); };

            public:

                virtual wssize_t    avail();

                virtual wssize_t    position();

                virtual ssize_t     read(void *dst, size_t count);

                virtual wssize_t    seek(wsize_t position);

                virtual status_t    close();
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IO_INMEMORYSTREAM_H_ */
