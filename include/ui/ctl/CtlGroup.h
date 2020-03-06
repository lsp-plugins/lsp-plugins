/*
 * CtlGroup.h
 *
 *  Created on: 17 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLGROUP_H_
#define UI_CTL_CTLGROUP_H_

namespace lsp
{
    namespace ctl
    {
        class CtlGroup: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlColor        sColor;
                CtlColor        sTextColor;
                CtlExpression   sEmbed;

            protected:
                void do_destroy();

            public:
                explicit CtlGroup(CtlRegistry *src, LSPGroup *widget);
                virtual ~CtlGroup();

            public:
                virtual void init();

                virtual void destroy();

                virtual void set(const char *name, const char *value);

                virtual void set(widget_attribute_t att, const char *value);

                virtual status_t add(CtlWidget *child);

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLGROUP_H_ */
