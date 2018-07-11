/*
 * LSPCell.h
 *
 *  Created on: 21 июн. 2017 г.
 *      Author: sadko
 */

#ifndef UI_TK_LSPCELL_H_
#define UI_TK_LSPCELL_H_

namespace lsp
{
    namespace tk
    {
        class LSPCell: public LSPWidgetProxy
        {
            public:
                static const w_class_t    metadata;

            private:
                size_t      nRowSpan;
                size_t      nColSpan;

            public:
                explicit LSPCell(LSPDisplay *dpy);
                virtual ~LSPCell();

            public:
                /** Get cell's rowspan
                 *
                 * @return cell's rowspan
                 */
                inline size_t rowspan() { return nRowSpan; }

                /** Set cell's colspan
                 *
                 * @return cell's colspan
                 */
                inline size_t colspan() { return nColSpan; }

            public:
                /** Set rowspan
                 *
                 * @param value value for the rowspan
                 */
                inline void set_rowspan(size_t value) { nRowSpan = value; }

                /** Set colspan
                 *
                 * @param value value for the colspan
                 */
                inline void set_colspan(size_t value) { nColSpan = value; }
        };
    
    } /* namespace tk */
} /* namespace lsp */

#endif /* UI_TK_LSPCELL_H_ */
