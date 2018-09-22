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

#include <core/io/Reader.h>
#include <core/io/Writer.h>
#include <core/io/IInputStream.h>

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
        status_t load(io::Reader *is, IConfigHandler *h);

        /**
         * Load configuration from character input stream
         * @param input stream pointer
         * @param h configuration parameter handler
         * @return status of operation
         */
        status_t load(io::IInputStream *is, IConfigHandler *h);

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
         * Save configuration to output stream
         * @param os character output stream
         * @param s configuration parameter source
         * @param comments add comments
         * @return status of operation
         */
        status_t save(io::Writer *os, IConfigSource *s, bool comments = true);

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
