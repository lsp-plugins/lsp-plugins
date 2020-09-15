/*
 * Copyright (C) 2020 Linux Studio Plugins Project <https://lsp-plug.in/>
 *           (C) 2020 Vladimir Sadovnikov <sadko4u@gmail.com>
 *
 * This file is part of lsp-plugins
 * Created on: 07 мая 2018 г.
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

#ifndef UI_CTL_CTLLOADFILE_H_
#define UI_CTL_CTLLOADFILE_H_

namespace lsp
{
    namespace ctl
    {
        class CtlLoadFile: public CtlWidget
        {
            public:
                static const ctl_class_t metadata;

            protected:
                CtlPort        *pFile;
                CtlPort        *pStatus;
                CtlPort        *pCommand;
                CtlPort        *pProgress;
                CtlPort        *pPath;
                CtlExpression   sFormat;

                char           *pPathID;

            protected:
                void        update_state();
                status_t    commit_state();

            protected:
                static status_t slot_on_activate(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_close(LSPWidget *sender, void *ptr, void *data);
                static status_t slot_on_file_submit(LSPWidget *sender, void *ptr, void *data);
                void        update_path();

            public:
                explicit CtlLoadFile(CtlRegistry *reg, LSPLoadFile *load);
                virtual ~CtlLoadFile();

            public:
                virtual void set(widget_attribute_t att, const char *value);

                virtual void end();

                virtual void notify(CtlPort *port);
        };
    
    } /* namespace ctl */
} /* namespace lsp */

#endif /* UI_CTL_CTLLOADFILE_H_ */
