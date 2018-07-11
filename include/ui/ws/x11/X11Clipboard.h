/*
 * X11Clipboard.h
 *
 *  Created on: 15 нояб. 2017 г.
 *      Author: sadko
 */

#ifndef INCLUDE_UI_WS_X11_X11CLIPBOARD_H_
#define INCLUDE_UI_WS_X11_X11CLIPBOARD_H_

namespace lsp
{
    namespace ws
    {
        namespace x11
        {
            
            class X11Clipboard: public IClipboard
            {
                protected:
                    enum cdata
                    {
                        CHUNK_SIZE      = 0x10000
                    };

                    typedef struct chunk_t
                    {
                        size_t          size;           // Size of chunk
                        size_t          avail;          // Available data in chunk
                        chunk_t        *next;           // Pointer to next chunk
                        uint8_t         data[];         // Data
                    } chunk_t;

                protected:
                    wsize_t         nAvail;
                    chunk_t        *pFirst;
                    chunk_t        *pLast;
                    char           *sType;

                protected:
                    class InputStream: public io::IInputStream
                    {
                        protected:
                            X11Clipboard       *pCB;
                            bool                bClosed;
                            chunk_t            *pCurr;
                            size_t              nOffset;
                            wsize_t             nPosition;

                        public:
                            explicit InputStream(X11Clipboard *cb);
                            virtual ~InputStream();

                        public:
                            virtual wssize_t    avail();
                            virtual wssize_t    position();
                            virtual ssize_t     read(void *dst, size_t count);
                            virtual wssize_t    seek(wsize_t position);
                            virtual status_t    close();
                    };

                public:
                    X11Clipboard();
                    virtual ~X11Clipboard();

                public:
                    virtual status_t            close();

                    virtual io::IInputStream   *read(const char *ctype);

                    ssize_t                     append(const void *buf, size_t count);
            };
        
        } /* namespace x11 */
    } /* namespace ws */
} /* namespace lsp */

#endif /* INCLUDE_UI_WS_X11_X11CLIPBOARD_H_ */
