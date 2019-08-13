/*
 * CtlPortAlias.h
 *
 *  Created on: 23 окт. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLPORTALIAS_H_
#define UI_CTL_CTLPORTALIAS_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlPortAlias: public CtlWidget
        {
            private:
                char       *sID;
                char       *sAlias;

            public:
                explicit CtlPortAlias(CtlRegistry *src);
                virtual ~CtlPortAlias();

            public:
                virtual void set(widget_attribute_t att, const char *value);

            public:
                inline const char *id() const       { return sID; }
                inline const char *alias() const    { return sAlias; }
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLPORTALIAS_H_ */
