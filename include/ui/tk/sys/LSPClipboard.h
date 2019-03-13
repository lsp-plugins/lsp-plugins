/*
 * LSPClipboard.h
 *
 *  Created on: 14 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_SYS_LSPCLIPBOARD_H_
#define UI_TK_SYS_LSPCLIPBOARD_H_

#include <core/io/IInStream.h>
#include <core/io/IOutStream.h>

namespace lsp
{
    namespace tk
    {
        /** This object should be always allocated using dynamic memory
         *
         */
        class LSPClipboard: public IClipboard
        {
            protected:
                enum cdata
                {
                    CHUNK_SIZE      = 0x10000,
                    CHUNK_EXT       = 0x10,
                    MAX_CSET_NAME   = 64
                };

            protected:
                uint8_t       **vChunks;            // The array of memory chunks
                size_t          nTotalChunks;       // The amount of memory chunks in array
                size_t          nUsedChunks;        // The amount of used memory chunks in array
                size_t          nLastChunkSize;     // The actual amount of data stored in last chunk
                ssize_t         nReferences;        // The number of references to clipboard data holder
                char           *sCType;             // Content type
                bool            bClosed;            // Closed flag

            protected:
                class LSPInputStream: public io::IInStream
                {
                    protected:
                        LSPClipboard       *pCB;
                        bool                bClosed;
                        size_t              nChunk;
                        size_t              nOffset;

                    public:
                        explicit LSPInputStream(LSPClipboard *cb);
                        virtual ~LSPInputStream();

                    public:
                        virtual wssize_t    avail();
                        virtual wssize_t    position();
                        virtual ssize_t     read(void *dst, size_t count);
                        virtual wssize_t    seek(wsize_t position);
                        virtual status_t    close();
                };

                class LSPOutputStream: public io::IOutStream
                {
                    protected:
                        LSPClipboard       *pCB;
                        bool                bClosed;
                        size_t              nChunk;
                        size_t              nOffset;

                    public:
                        explicit LSPOutputStream(LSPClipboard *cb);
                        virtual ~LSPOutputStream();

                    public:
                        virtual wssize_t    position();
                        virtual ssize_t     write(const void *buf, size_t count);
                        virtual wssize_t    seek(wsize_t position);
                        virtual status_t    close();
                };

            protected:
                void                destroy_data();

            public:
                explicit LSPClipboard();
                virtual ~LSPClipboard();

            public:
                /** Closes the clipboard data and forces the clipboard to destroy object if possible
                 *
                 * @return status of operation
                 */
                virtual status_t        close();

                /** Get last error code
                 *
                 * @return last error code
                 */
                inline status_t         error_code() const { return nError; };

                /** Get content type
                 *
                 * @return content type
                 */
                inline const char          *content_type() const { return sCType; };

                /** Get the size of clipboard in memory
                 *
                 * @return size of clipboard in memory
                 */
                size_t                      size() const;

                /** Get the capacity of clipboard in memory
                 *
                 * @return size of clipboard in memory
                 */
                size_t                      capacity() const;

                /** Get clipboard content for reading
                 *
                 * @param ctype content type
                 * @return pointer to the opened clipboard stream or NULL if content type or character set is not supported
                 */
                virtual io::IInStream   *read(const char *ctype);

                /** Write data to the clipboard
                 *
                 * @param ctype
                 * @param charset
                 * @return
                 */
                virtual io::IOutStream  *write(const char *ctype);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_SYS_LSPCLIPBOARD_H_ */
