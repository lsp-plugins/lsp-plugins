/*
 * Dir.h
 *
 *  Created on: 18 мар. 2019 г.
 *      Author: sadko
 */

#ifndef CORE_IO_DIR_H_
#define CORE_IO_DIR_H_

#include <core/status.h>
#include <core/LSPString.h>
#include <core/io/Path.h>

#ifdef PLATFORM_WINDOWS
    #include <fileapi.h>
#else
    #include <dirent.h>
#endif /* PLATFORM_WINDOWS */

namespace lsp
{
    namespace io
    {
        class Dir
        {
            protected:
                status_t        nErrorCode;
                size_t          nPosition;
                Path            sPath;
#ifdef PLATFORM_WINDOWS
                HANDLE          hDir;           // Directory handle
                WIN32_FIND_DATAW sData;         // Last data read
                status_t        nPending;       // Pending error code
#else
                DIR            *hDir;           // Directory handle
#endif /* PLATFORM_WINDOWS */

            private:
                Dir &operator = (const Dir &);

            protected:
                inline status_t set_error(status_t error) { return nErrorCode = error; }

            public:
                explicit Dir();
                virtual ~Dir();

            public:
                /**
                 * Open directory
                 * @param path path to directory in UTF-8 encoding
                 * @return status of operation
                 */
                status_t    open(const char *path);

                /**
                 * Open directory
                 * @param path path to directory
                 * @return status of operation
                 */
                status_t    open(const LSPString *path);

                /**
                 * Open directory
                 * @param path path to directory
                 * @return status of operation
                 */
                status_t    open(const Path *path);

                /**
                 * Seek directory to it's beginning
                 * @return status of operation
                 */
                status_t    rewind();

                /**
                 * Read the record from directory
                 * @param path pointer to string to store new record
                 * @param full retrieve full path instead of relative
                 * @return status of operation
                 */
                status_t    read(LSPString *path, bool full = false);

                /**
                 * Read the record from directory
                 * @param path pointer to string to store new record
                 * @param full retrieve full path instead of relative
                 * @return status of operation
                 */
                status_t    read(Path *path, bool full = false);

                /**
                 * Close current directory
                 * @return status of operation
                 */
                status_t    close();

                /**
                 * Return last error code
                 * @return last error code
                 */
                inline status_t last_error() const  { return nErrorCode; };

                /**
                 * Return true if last read operations reached end of file
                 * @return true if last read operations reached end of file
                 */
                inline bool eof() const { return nErrorCode == STATUS_EOF; };

                /**
                 * Return current position of directory in records
                 * @return current position of directory in records
                 */
                inline size_t position() const { return nPosition; }

                /**
                 * Get path associated with directory
                 * @param dst pointer to path to store value
                 * @return status of operation
                 */
                inline status_t get_path(Path *dst) const { return (dst != NULL) ? dst->set(&sPath) : STATUS_BAD_ARGUMENTS; }

                /**
                 * Get path associated with directory
                 * @param dst pointer to path to store value
                 * @return status of operation
                 */
                inline status_t get_path(LSPString *dst) const { return (dst != NULL) ? dst->set(sPath.as_string()) : STATUS_BAD_ARGUMENTS; }

                /**
                 * Get path associated with directory
                 * @return pointer to UTF-8 string or NULL
                 */
                inline const char *get_path() const { return sPath.as_string()->get_utf8(); }
        };
    
    } /* namespace io */
} /* namespace lsp */

#endif /* CORE_IO_DIR_H_ */
