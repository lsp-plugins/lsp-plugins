/*
 * LSPFloatProperty.h
 *
 *  Created on: 9 окт. 2019 г.
 *      Author: sadko
 */

#ifndef UI_TK_SYS_LSPFLOAT_H_
#define UI_TK_SYS_LSPFLOAT_H_

namespace lsp
{
    namespace tk
    {
        
        class LSPFloat
        {
            private:
                LSPFloat & operator = (const LSPFloat *);
                
            protected:
                class Listener: public IStyleListener
                {
                    private:
                        LSPFloat   *pProperty;
                        LSPStyle   *pStyle;
                        ui_atom_t   aValue;

                    public:
                        explicit Listener(LSPFloat *property);
                        virtual ~Listener();

                    public:
                        virtual void    notify(ui_atom_t property);
                        void            sync();
                        void            unbind();
                        status_t        bind(LSPDisplay *dpy, LSPStyle *style, const char *property);
                };

            protected:
                LSPWidget  *pWidget;
                float       fValue;
                Listener    sListener;
                
            public:
                explicit LSPFloat();
                explicit LSPFloat(LSPWidget *widget);
                virtual ~LSPFloat();

            public:
                status_t    bind(const char *property);
                status_t    bind(LSPStyle *style, const char *property);
                status_t    bind(LSPDisplay *dpy, LSPStyle *style, const char *property);
                inline void unbind() { sListener.unbind(); };

            public:
                inline float    get() const { return fValue; }
                float           set(float v);
        };
    
    } /* namespace calc */
} /* namespace lsp */

#endif /* INCLUDE_UI_TK_SYS_LSPFLOATPROPERTY_H_ */
