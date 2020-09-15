/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 11 нояб. 2017 г.
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

#ifndef INCLUDE_UI_CTL_CTLSWITCHEDPORT_H_
#define INCLUDE_UI_CTL_CTLSWITCHEDPORT_H_

namespace lsp
{
    namespace ctl
    {
        class CtlSwitchedPort: public CtlPort, public CtlPortListener
        {
            public:
                static const ctl_class_t metadata;

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
                #pragma pack(pop)

            protected:
                plugin_ui  *pUI;
                size_t      nDimensions;
                CtlPort   **vControls;
                CtlPort    *pReference;
                char       *sName;
                token_t    *sTokens;

            protected:
                static token_t     *tokenize(const char *path);
                static token_t     *next_token(token_t *token);
                void                rebind();
                void                destroy();
                inline CtlPort     *current()
                {
                    if (pReference == NULL)
                        rebind();
                    return pReference;
                };

            public:
                explicit CtlSwitchedPort(plugin_ui *ui);
                virtual ~CtlSwitchedPort();

            public:
                bool compile(const char *id);
                virtual const char *id() const;

            public:
                virtual void write(const void *buffer, size_t size);
                virtual void *get_buffer();
                virtual float get_value();
                virtual float get_default_value();
                virtual void set_value(float value);
                virtual void notify_all();
                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* INCLUDE_UI_CTL_CTLSWITCHEDPORT_H_ */
