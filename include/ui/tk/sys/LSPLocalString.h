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

            protected:
                class Listener: public IStyleListener
                {
                    private:
                        LSPLocalString  *pString;

                    public:
                        inline Listener(LSPLocalString *ps) { pString = ps; }
                        virtual ~Listener();

                    public:
                        virtual void notify(ui_atom_t property);
                };

            protected:
                LSPWidget          *pWidget;    // Owner of this string
                size_t              nFlags;     // Different flags
                mutable ui_atom_t   nAtom;      // Atom for "lang" property
                LSPString           sText;      // Text used for rendering
                calc::Parameters    sParams;    // The paramet
                Listener            sListener;  // Style listener

            protected:
                status_t            fmt_internal(LSPString *out, IDictionary *dict, const LSPString *lang) const;
                virtual void        notify(ui_atom_t property);
                virtual void        sync();

            public:
                explicit LSPLocalString();
                explicit LSPLocalString(LSPWidget *widget);
                virtual ~LSPLocalString();

            public:
                /**
                 * Bind property to the widget
                 * @param widget widget to bind
                 * @return status of operation
                 */
                status_t bind(const char *property);
                status_t bind(const LSPString *property);
                status_t bind(ui_atom_t property);
                status_t bind();

                /**
                 * Unbind property from widget
                 * @param widget widget to unbind
                 * @return status of operation
                 */
                status_t unbind();

                /**
                 * Check wheter the string is localized
                 * @return true if the string is localized
                 */
                inline bool localized() const           { return nFlags & F_LOCALIZED;    }

                /**
                 * Get raw text
                 * @return raw text or NULL if string is localized
                 */
                inline const LSPString *raw() const { return (nFlags & F_LOCALIZED) ? NULL : &sText;        }

                /**
                 * Get localization key
                 * @return localization key or NULL if string is not localized
                 */
                inline const LSPString *key() const { return (nFlags & F_LOCALIZED) ? &sText : NULL;        }

                /**
                 * Get localization parameters
                 * @return localization parameters or NULL if string is not localized
                 */
                inline const calc::Parameters *params() const { return &sParams; }

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
                 * Set raw (non-localized) value
                 * @param value value to set
                 * @return status of operation
                 */
                status_t set_key(const LSPString *value);

                /**
                 * Set raw (non-localized) value
                 * @param value UTF-8 text to set
                 * @return status of operation
                 */
                status_t set_key(const char *value);

                /**
                 * Set localized value
                 * @param key the bundle key to use as a text source
                 * @param params additional optional parameters for message formatting
                 * @return status of operation
                 */
                status_t set(const LSPString *key, const calc::Parameters *params);
                inline status_t set(const LSPString *key) { return set(key, (calc::Parameters *)NULL); };

                /**
                 * Set localized value
                 * @param key the UTF8 bundle key to use as a text source
                 * @param params additional optional parameters for message formatting
                 * @return status of operation
                 */
                status_t set(const char *key, const calc::Parameters *params);
                inline status_t set(const char *key) { return set(key, (calc::Parameters *)NULL); };

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

                /**
                 * Output the formatted message to the string
                 * @param out output string
                 * @param dict dictionary that stores localization data
                 * @param lang the target language to use
                 * @return status of operation
                 */
                status_t format(LSPString *out, IDictionary *dict, const char *lang) const;

                /**
                 * Output the formatted message to the string
                 * @param out output string
                 * @param dict dictionary that stores localization data
                 * @param lang the target language to use (UTF-8 encoded)
                 * @return status of operation
                 */
                status_t format(LSPString *out, IDictionary *dict, const LSPString *lang) const;

                /**
                 * Format the message using dictionary and style from derived widget
                 * @param out output string
                 * @return status of operation
                 */
                status_t format(LSPString *out) const;
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_SYS_LSPLOCALSTRING_H_ */
