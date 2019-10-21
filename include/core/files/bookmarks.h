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
#include <core/io/IOutSequence.h>

namespace lsp
{
    namespace bookmarks
    {
        enum bm_origin_t
        {
            BM_LSP      = 1 << 0,
            BM_GTK3     = 1 << 1,
            BM_QT5      = 1 << 2
        };

        typedef struct bookmark_t
        {
            LSPString   path;       // Path to the directory
            LSPString   name;       // Bookmark name
            size_t      origin;     // Bookmark origin: LSP, GTK3, QT
        } bookmark_t;

        /**
         * Read GTK3 bookmarks
         * @param dst list to store bookmarks
         * @param path location of the file in UTF-8 encoding
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks_gtk3(cvector<bookmark_t> *dst, const char *path, const char *charset=NULL);

        /**
         * Read GTK3 bookmarks
         * @param dst list to store bookmarks
         * @param path location of the file
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks_gtk3(cvector<bookmark_t> *dst, const LSPString *path, const char *charset=NULL);

        /**
         * Read GTK3 bookmarks
         * @param dst list to store bookmarks
         * @param path location of the file
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks_gtk3(cvector<bookmark_t> *dst, const io::Path *path, const char *charset=NULL);

        /**
         * Read GTK3 bookmarks
         * @param dst list to store bookmarks
         * @param in input character sequence
         * @return status of operation
         */
        status_t read_bookmarks_gtk3(cvector<bookmark_t> *dst, io::IInSequence *in);

        /**
         * Read LSP bookmarks in JSON5 format
         * @param dst list to store bookmarks
         * @param path location of the file in UTF-8 encoding
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks(cvector<bookmark_t> *dst, const char *path, const char *charset=NULL);

        /**
         * Read LSP bookmarks in JSON5 format
         * @param dst list to store bookmarks
         * @param path location of the file
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks(cvector<bookmark_t> *dst, const LSPString *path, const char *charset=NULL);

        /**
         * Read LSP bookmarks in JSON5 format
         * @param dst list to store bookmarks
         * @param path location of the file
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t read_bookmarks(cvector<bookmark_t> *dst, const io::Path *path, const char *charset=NULL);

        /**
         * Read LSP bookmarks in JSON5 format
         * @param dst list to store bookmarks
         * @param in input character sequence
         * @return status of operation
         */
        status_t read_bookmarks(cvector<bookmark_t> *dst, io::IInSequence *in);

        /**
         * Save LSP bookmarks in JSON5 format
         * @param src list to store bookmarks
         * @param path location of the file in UTF-8 encoding
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t save_bookmarks(const cvector<bookmark_t> *src, const char *path, const char *charset=NULL);

        /**
         * Save LSP bookmarks in JSON5 format
         * @param src list to store bookmarks
         * @param path location of the file
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t save_bookmarks(const cvector<bookmark_t> *src, const LSPString *path, const char *charset=NULL);

        /**
         * Save LSP bookmarks in JSON5 format
         * @param src list to store bookmarks
         * @param path location of the file
         * @param charset file encoding character set, NULL for default locale
         * @return status of operation
         */
        status_t save_bookmarks(const cvector<bookmark_t> *src, const io::Path *path, const char *charset=NULL);

        /**
         * Save LSP bookmarks in JSON5 format
         * @param src list to store bookmarks
         * @param out output character sequence
         * @return status of operation
         */
        status_t save_bookmarks(const cvector<bookmark_t> *src, io::IOutSequence *out);

        /**
         * Destroy bookmarks
         * @param list list of bookmarks
         */
        void destroy_bookmarks(cvector<bookmark_t> *list);
    }
}

#endif /* CORE_FILES_BOOKMARKS_H_ */
