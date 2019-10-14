/*
 * bookmarks.h
 *
 *  Created on: 14 окт. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_BOOKMARKS_H_
#define CORE_FILES_BOOKMARKS_H_

#include <data/cvector.h>
#include <core/status.h>
#include <core/LSPString.h>
#include <core/io/Path.h>
#include <core/io/IInSequence.h>

namespace lsp
{
    namespace bookmarks
    {
        /**
         * Read GTK3 bookmarks
         * @param path location of the file in UTF-8 encoding
         * @param dst list to store bookmarks
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks_gtk3(cvector<LSPString> *dst, const char *path, const char *charset=NULL);

        /**
         * Read GTK3 bookmarks
         * @param path location of the file
         * @param dst list to store bookmarks
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks_gtk3(cvector<LSPString> *dst, const LSPString *path, const char *charset=NULL);

        /**
         * Read GTK3 bookmarks
         * @param path location of the file
         * @param dst list to store bookmarks
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks_gtk3(cvector<LSPString> *dst, const io::Path *path, const char *charset=NULL);

        /**
         * Read GTK3 bookmarks
         * @param path location of the file
         * @param dst list to store bookmarks
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks_gtk3(cvector<LSPString> *dst, io::IInSequence *in);

        /**
         * Destroy bookmarks
         * @param list list of bookmarks
         */
        void destroy_bookmarks(cvector<LSPString> *list);
    }
}

#endif /* CORE_FILES_BOOKMARKS_H_ */
