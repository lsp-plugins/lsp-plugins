/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 9 окт. 2019 г.
 *
 * lsp-plugins is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * lsp-plugins is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with lsp-plugins. If not, see <https://www.gnu.org/licenses/>.
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
