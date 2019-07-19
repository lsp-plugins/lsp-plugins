/*
 * config.h
 *
 *  Created on: 11 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_CONFIG_H_
#define CORE_FILES_CONFIG_H_

#include <stdio.h>

#include <core/types.h>
#include <core/status.h>

#include <core/files/config/IConfigHandler.h>
#include <core/files/config/IConfigSource.h>

#include <core/io/File.h>
#include <core/io/Path.h>
#include <core/io/IInSequence.h>
#include <core/io/IInStream.h>
#include <core/io/IOutSequence.h>
#include <core/io/IOutStream.h>

namespace lsp
{
    namespace config
    {
        /**
         * Load configuration file
         * @param path location of the file
         * @param h configuration parameter handler
         * @return status of operation
         */
        status_t load(const char *path, IConfigHandler *h);

        /**
         * Load configuration file
         * @param path location of the file
         * @param h configuration parameter handler
         * @return status of operation
         */
        status_t load(const LSPString *path, IConfigHandler *h);

        /**
         * Load configuration file
         * @param path location of the file
         * @param h configuration parameter handler
         * @return status of operation
         */
        status_t load(const io::Path *path, IConfigHandler *h);

        /**
         * Load configuration from file
         * @param fd file handle
         * @param h configuration parameter handler
         * @return status of operation
         */
        status_t load(FILE *fd, IConfigHandler *h);

        /**
         * Load configuration from character input stream
         * @param character input stream pointer
         * @param h configuration parameter handler
         * @return status of operation
         */
        status_t load(io::IInSequence *is, IConfigHandler *h);

        /**
         * Load configuration from character input stream
         * @param input stream pointer
         * @param h configuration parameter handler
         * @return status of operation
         */
        status_t load(io::IInStream *is, IConfigHandler *h);

        /**
         * Load configuration from file
         * @param input stream pointer
         * @param h configuration parameter handler
         * @return status of operation
         */
        status_t load(io::File *fd, IConfigHandler *h);

        /**
         * Deserialize configuration from string instance
         * @param cfg configuration string
         * @param h configuration parameter handler
         * @return status of operation
         */
        status_t deserialize(const LSPString *cfg, IConfigHandler *h);

        /**
         * Save configuration file
         * @param path location of the file
         * @param s configuration parameter source
         * @param comments add comments
         * @return status of operation
         */
        status_t save(const char *path, IConfigSource *s, bool comments = true);

        /**
         * Save configuration file
         * @param fd file handle
         * @param s configuration parameter source
         * @param comments add comments
         * @return status of operation
         */
        status_t save(FILE *fd, IConfigSource *s, bool comments = true);

        /**
         * Save configuration file
         * @param fd file handle
         * @param s configuration parameter source
         * @param comments add comments
         * @return status of operation
         */
        status_t save(io::File *fd, IConfigSource *s, bool comments = true);

        /**
         * Save configuration file
         * @param os output stream
         * @param s configuration parameter source
         * @param comments add comments
         * @return status of operation
         */
        status_t save(io::IOutStream *os, IConfigSource *s, bool comments = true);

        /**
         * Save configuration to output stream
         * @param os output stream
         * @param s configuration parameter source
         * @param comments add comments
         * @return status of operation
         */
        status_t save(io::IOutSequence *os, IConfigSource *s, bool comments = true);

        /**
         * Serialize configuration to string instance
         * @param cfg string to store configuration
         * @param s configuration parameter source
         * @param comments add comments
         * @return status of operation
         */
        status_t serialize(LSPString *cfg, IConfigSource *h, bool comments = true);
    };
    
} /* namespace lsp */

#endif /* CORE_FILES_CONFIG_H_ */
