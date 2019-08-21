/*
 * LSPTextDataSource.h
 *
 *  Created on: 21 авг. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_UTIL_LSPTEXTDATASOURCE_H_
#define UI_TK_UTIL_LSPTEXTDATASOURCE_H_

namespace lsp
{
    namespace tk
    {
        class LSPTextDataSource: public ws::IDataSource
        {
            protected:
                LSPString   sText;

            public:
                explicit LSPTextDataSource();
                virtual ~LSPTextDataSource();

            public:
                /**
                 * Set UTF-8 encoded text
                 * @param text UTF-8 encoded text
                 * @return status of operation
                 */
                status_t set_text(const char *text);

                /**
                 * Set text from the LSPString object
                 * @param text text
                 * @return status of operation
                 */
                status_t set_text(const LSPString *text);

                /**
                 * Set text from the LSPString object
                 * @param text text
                 * @param first the first character of the source string to use
                 * @return status of operation
                 */
                status_t set_text(const LSPString *text, ssize_t first);

                /**
                 * Set text from the LSPString object
                 * @param text text
                 * @param first the first character to start from
                 * @param last the last character to end
                 * @return status of operation
                 */
                status_t set_text(const LSPString *text, ssize_t first, ssize_t last);

            public:
                virtual io::IInStream   *open(const char *mime);
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_UTIL_LSPTEXTDATASOURCE_H_ */
