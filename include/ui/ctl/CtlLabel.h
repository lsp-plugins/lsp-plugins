/*
 * CtlLabel.h
 *
 *  Created on: 7 июл. 2017 г.
 *      Author: sadko
 */

#ifndef INCLUDE_UI_CTL_CTLLABEL_H_
#define INCLUDE_UI_CTL_CTLLABEL_H_

namespace lsp
{
    namespace ctl
    {
        enum ctl_label_type_t
        {
            CTL_LABEL_TEXT,
            CTL_LABEL_VALUE,
            CTL_LABEL_PARAM,
            CTL_STATUS_CODE
        };


        class CtlLabel: public CtlWidget
        {
            protected:
                CtlColor            sColor;
                CtlColor            sBgColor;
                CtlPort            *pPort;
                ctl_label_type_t    enType;
                float               fValue;
                bool                bDetailed;
                bool                bSameLine;
                ssize_t             nUnits;
                ssize_t             nPrecision;

            protected:
                void commit_value();

            public:
                explicit CtlLabel(CtlRegistry *src, LSPLabel *widget, ctl_label_type_t type);
                virtual ~CtlLabel();

            public:
                /** Begin initialization of controller
                 *
                 */
                virtual void init();

                /** Set attribute to widget
                 *
                 * @param att attribute identifier
                 * @param value attribute value
                 */
                virtual void set(widget_attribute_t att, const char *value);

                /** Notify controller about one of port bindings has changed
                 *
                 * @param port port triggered change
                 */
                virtual void notify(CtlPort *port);

                /** Complete initialization
                 *
                 */
                virtual void end();
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* INCLUDE_UI_CTL_CTLLABEL_H_ */
