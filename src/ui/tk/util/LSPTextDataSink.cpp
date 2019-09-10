/*
 * LSPTextDataSink.cpp
 *
 *  Created on: 22 авг. 2019 г.
 *      Author: sadko
 */

#include <ui/tk/tk.h>

namespace lsp
{
    namespace tk
    {
        static const char * const mimes[] =
        {
            "UTF8_STRING",
            "text/plain;charset=utf-8",
            "text/plain;charset=UTF-16LE",
            "text/plain;charset=UTF-16BE",
            "text/plain;charset=US-ASCII",
            "text/plain",
            NULL
        };
        
        LSPTextDataSink::LSPTextDataSink()
        {
            nMimeType   = -1;
        }
        
        LSPTextDataSink::~LSPTextDataSink()
        {
            nMimeType   = -1;
            sOut.drop();
        }
    
        ssize_t LSPTextDataSink::open(const char * const *mime_types)
        {
            // Iterate each MIME type in preferred order
            ssize_t i = 0;
            for (const char * const *p = mimes; *p != NULL; ++p, ++i)
            {
                // Does the list provide corresponding MIME type?
                size_t j = 0;
                for (const char *const *m = mime_types; *m != NULL; ++m, ++j)
                {
                    if (!::strcasecmp(*p, *m))
                    {
                        nMimeType = i;
                        return j;
                    }
                }
            }
            return -STATUS_UNSUPPORTED_FORMAT;
        }

       status_t LSPTextDataSink::write(const void *buf, size_t count)
       {
           ssize_t written = sOut.write(buf, count);
           return (written < 0) ? -written : STATUS_OK;
       }

       status_t LSPTextDataSink::close(status_t code)
       {
           // Analyze found MIME type
           LSPString text;
           bool success = false;

           switch (nMimeType)
           {
               case 0: // UTF8_STRING
               case 1: // text/plain;charset=utf-8
                   success  = text.set_utf8(reinterpret_cast<const char *>(sOut.data()), sOut.size());
                   break;
               case 2: // text/plain;charset=UTF-16LE
                   success  = __IF_LEBE(
                           text.set_utf16(reinterpret_cast<const lsp_utf16_t *>(sOut.data()), sOut.size()/2),
                           text.set_native(reinterpret_cast<const char *>(sOut.data()), sOut.size(), "UTF16-LE")
                       );
                   break;
               case 3: // text/plain;charset=UTF-16BE
                   success  = __IF_LEBE(
                           text.set_native(reinterpret_cast<const char *>(sOut.data()), sOut.size(), "UTF16-BE"),
                           text.set_utf16(reinterpret_cast<const lsp_utf16_t *>(sOut.data()), sOut.size()/2)
                       );
                   break;
               case 4:
                   success  = text.set_ascii(reinterpret_cast<const char *>(sOut.data()), sOut.size());
                   break;
               case 5:
                   success  = text.set_native(reinterpret_cast<const char *>(sOut.data()), sOut.size());
                   break;
               default:
                   break;
           }

           if (!success)
               code = STATUS_NO_MEM;

           status_t res = on_complete(code, &text);
           sOut.drop();
           return res;
       }

       status_t LSPTextDataSink::on_complete(status_t code, const LSPString *data)
       {
           return STATUS_OK;
       }
    } /* namespace tk */
} /* namespace lsp */
