/*
 * SwitchedPort.h
 *
 *  Created on: 19 апр. 2016 г.
 *      Author: sadko
 */

#ifndef UI_SWITCHEDPORT_H_
#define UI_SWITCHEDPORT_H_

#include <ui/ui.h>

namespace lsp
{
    class UISwitchedPort: public IUIPort, public IUIPortListener
    {
        protected:
            enum token_type_t
            {
                TT_END      = 0,
                TT_STRING   = 's',
                TT_INDEX    = 'i'
            };

            #pragma pack(push, 1)
            typedef struct token_t
            {
                char    type;
                char    data[];
            } token_t;

        protected:
            plugin_ui  *pUI;
            size_t      nDimensions;
            IUIPort   **vControls;
            IUIPort    *pReference;
            char       *sName;
            token_t    *sTokens;

        protected:
            static token_t     *tokenize(const char *str);
            static token_t     *next_token(token_t *token);
            void                rebind();
            void                destroy();
            inline IUIPort     *current()
            {
                if (pReference == NULL)
                    rebind();
                return pReference;
            };

        public:
            inline UISwitchedPort(plugin_ui *ui): IUIPort(NULL)
            {
                pUI             = ui;
                nDimensions     = 0;
                vControls       = NULL;
                pReference      = NULL;
                sName           = NULL;
                sTokens         = NULL;
            }

            virtual ~UISwitchedPort();

        public:
            bool compile(const char *id);
            inline const char *id() const { return sName; }

        public:
            virtual void write(const void *buffer, size_t size);
            virtual void *getBuffer();
            virtual float getValue();
            virtual void setValue(float value);
            virtual void notifyAll();
            virtual void notify(IUIPort *port);
    };

} /* namespace lsp */

#endif /* UI_SWITCHEDPORT_H_ */
