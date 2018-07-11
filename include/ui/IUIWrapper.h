/*
 * IUIWrapper.h
 *
 *  Created on: 25 апр. 2016 г.
 *      Author: sadko
 */

#ifndef IUIWRAPPER_H_
#define IUIWRAPPER_H_

namespace lsp
{
    
    class IUIWrapper
    {
        public:
            IUIWrapper();
            virtual ~IUIWrapper();

        public:
            /** Callback method, executes when the UI has been shown
             *
             */
            virtual void ui_activated();

            /** Callback method, executes when the UI has been hidden
             *
             */
            virtual void ui_deactivated();
    };

} /* namespace lsp */

#endif /* IUIWRAPPER_H_ */
