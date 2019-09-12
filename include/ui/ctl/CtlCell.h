/*
 * CtlCell.h
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLCELL_H_
#define UI_CTL_CTLCELL_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlCell: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                typedef struct param_t
                {
                    widget_attribute_t  attribute;
                    char                value[];
                } param_t;

            protected:
                CtlWidget          *pChild;
                cvector<param_t>    vParams;
                size_t              nRows;
                size_t              nCols;

            public:
                explicit CtlCell(CtlRegistry *src);
                virtual ~CtlCell();

            public:
                inline size_t   rows() const        { return nRows; }
                inline size_t   columns() const     { return nCols; }

            public:
                virtual LSPWidget *widget();

                virtual void set(widget_attribute_t att, const char *value);

                virtual status_t add(CtlWidget *child);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLCELL_H_ */
