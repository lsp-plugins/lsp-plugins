/*
 * LSPLocalString.h
 *
 *  Created on: 2 мар. 2020 г.
 *      Author: sadko
 */

#ifndef UI_TK_SYS_LSPLOCALSTRING_H_
#define UI_TK_SYS_LSPLOCALSTRING_H_

#include <core/calc/Parameters.h>
#include <core/i18n/IDictionary.h>

namespace lsp
{
    namespace tk
    {
        class LSPWidget;

        class LSPLocalString
        {
            private:
                enum flags_t
                {
                    F_LOCALIZED     = 1 << 0,
//                    F_DIRTY         = 1 << 1
                };

            private:
                LSPWidget          *pOwner;     // Owner of this string
                size_t              nFlags;     // Different flags
                LSPString           sText;      // Text used for rendering
                calc::Parameters    sParams;    // The paramet

            public:
                explicit LSPLocalString();
                explicit LSPLocalString(LSPWidget *owner);
                virtual ~LSPLocalString();

            public:
                /**
                 * Check wheter the string is localized
                 * @return true if the string is localized
                 */
                inline bool localized() const           { return nFlags & F_LOCALIZED;    }

                /**
                 * Get raw text
                 * @return raw text or NULL if string is localized
                 */
                inline const LSPString *get_raw() const { return (nFlags & F_LOCALIZED) ? NULL : &sText;        }

                /**
                 * Get localization key
                 * @return localization key or NULL if string is not localized
                 */
                inline const LSPString *get_key() const { return (nFlags & F_LOCALIZED) ? &sText : NULL;        }

                /**
                 * Get localization parameters
                 * @return localization parameters or NULL if string is not localized
                 */
                inline const calc::Parameters *get_params() const { return (nFlags & F_LOCALIZED) ? &sParams : NULL; }

            public:
                /**
                 * Set raw (non-localized) value
                 * @param value value to set
                 * @return status of operation
                 */
                status_t set_raw(const LSPString *value);

                /**
                 * Set raw (non-localized) value
                 * @param value UTF-8 text to set
                 * @return status of operation
                 */
                status_t set_raw(const char *value);

                /**
                 * Set localized value
                 * @param key the bundle key to use as a text source
                 * @param params additional optional parameters for message formatting
                 * @return status of operation
                 */
                status_t set(const LSPString *key, const calc::Parameters *params = NULL);

                /**
                 * Set localized value
                 * @param key the UTF8 bundle key to use as a text source
                 * @param params additional optional parameters for message formatting
                 * @return status of operation
                 */
                status_t set(const char *key, const calc::Parameters *params = NULL);

                /**
                 * Make a copy of data from the source local string to this local string
                 * @param value source string value
                 * @return status of operation
                 */
                status_t set(const LSPLocalString *value);

                /**
                 * Clear the localized string
                 */
                void clear();
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_SYS_LSPLOCALSTRING_H_ */
