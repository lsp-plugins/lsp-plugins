/*
 * CtlMesh.h
 *
 *  Created on: 27 июл. 2017 г.
 *      Author: sadko
 */

#ifndef UI_CTL_CTLMESH_H_
#define UI_CTL_CTLMESH_H_

namespace lsp
{
    namespace ctl
    {
        
        class CtlMesh: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort        *pPort;
                float           fTransparency;
                CtlColor        sColor;

            public:
                explicit CtlMesh(CtlRegistry *src, LSPMesh *mesh);
                virtual ~CtlMesh();

            public:
                /** Begin initialization of controller
                 *
                 */
                virtual void init();

                /** Set attribute
                 *
                 * @param att widget attribute
                 * @param value widget value
                 */
                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLMESH_H_ */
