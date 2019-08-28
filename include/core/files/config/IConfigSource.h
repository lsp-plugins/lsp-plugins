/*
 * IConfigSource.h
 *
 *  Created on: 11 июн. 2018 г.
 *      Author: sadko
 */

#ifndef CORE_FILES_CONFIG_ICONFIGSOURCE_H_
#define CORE_FILES_CONFIG_ICONFIGSOURCE_H_

#include <core/types.h>
#include <core/status.h>
#include <core/buffer.h>
#include <core/LSPString.h>
#include <core/files/config/flags.h>

namespace lsp
{
    namespace config
    {
        class IConfigSource
        {
            private:
                buffer_t sBuf;

            private:
                IConfigSource & operator = (const IConfigSource &);

            public:
                explicit IConfigSource();
                virtual ~IConfigSource();

            public:
                /**
                 * Get header comment
                 * @param comment pointer to store header comment
                 * @return status of operation, STATUS_NOT_FOUND or STATUS_NO_DATA if no comment
                 */
                virtual status_t get_head_comment(LSPString *comment);

                /**
                 * Get header comment for serialization as C-style string
                 * @param comment pointer to store header comment
                 * @return status of operation, STATUS_NOT_FOUND or STATUS_NO_DATA if no comment
                 */
                virtual status_t get_head_comment(const char **comment);

                /**
                 * Return next parameter for serialization
                 * @param name pointer to store string with name of parameter
                 * @param value pointer to store string with value of parameter
                 * @param comment pointer to store string with comment to parameter
                 * @param flags serialization flags
                 * @return STATUS_OK on success, STATUS_NOT_FOUND, STATUS_NO_DATA or STATUS_EOF when there is no more parameter
                 */
                virtual status_t get_parameter(LSPString *name, LSPString *value, LSPString *comment, int *flags);

                /**
                 * Return next parameter for serialization
                 * @param name pointer to store string with name of parameter
                 * @param value pointer to store string with value of parameter
                 * @param flags serialization flags
                 * @return STATUS_OK on success, STATUS_NOT_FOUND, STATUS_NO_DATA or STATUS_EOF when there is no more parameter
                 */
                virtual status_t get_parameter(LSPString *name, LSPString *value, int *flags);

                /**
                 * Return next parameter for serialization as C-style strings.
                 * Configuration source instance is responsive for managing memory for
                 * returned pointers.
                 *
                 * @param name pointer to store UTF-8 string with name of parameter
                 * @param value pointer to store UTF-8 string with value of parameter
                 * @param comment pointer to store UTF-8 string with comment to parameter
                 * @param flags serialization flags
                 * @return STATUS_OK on success, STATUS_NOT_FOUND, STATUS_NO_DATA or STATUS_EOF when there is no more parameter
                 */
                virtual status_t get_parameter(const char **name, const char **value, const char **comment, int *flags);

                /**
                 * Return next parameter for serialization as C-style strings.
                 * Configuration source instance is responsive for managing memory for
                 * returned pointers.
                 *
                 * @param name pointer to store UTF-8 string with name of parameter
                 * @param value pointer to store UTF-8 string with value of parameter
                 * @param flags serialization flags
                 * @return STATUS_OK on success, STATUS_NOT_FOUND, STATUS_NO_DATA or STATUS_EOF when there is no more parameter
                 */
                virtual status_t get_parameter(const char **name, const char **value, int *flags);
        };
    
    } /* namespace config */
} /* namespace lsp */

#endif /* CORE_FILES_CONFIG_ICONFIGSOURCE_H_ */
